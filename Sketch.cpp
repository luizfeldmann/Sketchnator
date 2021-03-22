/// Sketchnator
/// Copyright (C) 2021 Luiz Gustavo Pfitscher e Feldmann
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <wx/msgdlg.h>

#include <set>
#include <algorithm>
#include <Eigen/Eigen>
#include <iostream>
#include "sketch.h"
#include "ContainerSerialization.h"

// SERIALIZATION
// ====================================================================================================
BEGIN_SERIALIZATION_SCHEME(Sketch)
    SERIALIZATION_FIELD(points)
    SERIALIZATION_FIELD(lines)
    SERIALIZATION_FIELD(circles)
    SERIALIZATION_FIELD(constraints)
    SERIALIZATION_FIELD(annotations)
END_SERIALIZATION_SCHEME()

//#define DEBUG_SKETCH_SOLVER

// CONTEXT
// Encapsulates the solver state (equations, parameters etc)
// ====================================================================================================
template <class Iter>
bool set_common(Iter first1, Iter last1, Iter first2, Iter last2)
{
    for (Iter i = first1; i != last1; ++i)
        for (Iter j = first2; j != last2; ++j)
            if (*i == *j)
                return true;

    return false;
}

struct SolverContext
{
public:
    using Param = ConstraintEquation::Param;

protected:
    // class cannot be copied or assigned - it must be created from an existing sketch
    SolverContext() = default;

    SolverContext(const SolverContext& other) = delete; // non-copyable
    SolverContext& operator=(const SolverContext& other) = delete; // non-assignable

    // the original values of the parameters are saved, in case we desire to revert it back to previous state
    std::map<Param*, Param> original_parameters;

    // equations and parameters are managed under the hood - cannot be accessed directly
    // because we must guarantee that every parameter in 'equations' is listed in 'parameters'
    // and 'parameters' does not contain any parameter not used by 'equations'
    // any mismatch could make the matrix singular (for example, unused parameter would cause column of zeros)
    std::list<std::unique_ptr<ConstraintEquation>> equations;
    std::set<Param*> parameters;

public:
    SolverContext(SolverContext&& other) = default; // movable
    SolverContext(Sketch& sketch, const std::vector<Param*>& constantParameters = {})
    {
        // insert the equations present in the sketch
        for (std::unique_ptr<SketchConstraint>& constraint : sketch.constraints)
        {
            ConstraintEquation* equ = constraint->GetEquation();
            if (!equ)
                continue;

            equations.emplace_back( std::unique_ptr<ConstraintEquation>( equ ) );

            // get all the parameters it's gradient exposes
            for (const ConstraintEquationDerivative& der : equ->get_gradient())
            {
                // check if this parameter is to be made constant
                if (std::find(constantParameters.cbegin(), constantParameters.cend(), der.parameter) != constantParameters.cend())
                    continue; // if constant, the gradient parameter is not listed

                // if the parameter is being inserted (first time encountered)...
                if (parameters.insert( der.parameter ).second)
                    original_parameters.insert({der.parameter, *der.parameter}); // then save it's present value
            }
        }
    }

    // transfers content from other to inside this
    // other is left empty
    void splice(SolverContext& other)
    {
        // move the equations (they will never repeat because unique ownership)
        equations.splice(equations.end(), other.equations);

        // move the parameters
        for (Param* p : other.parameters)
            if (parameters.insert( p ).second)
                original_parameters.insert({p, *p});

        other.parameters.clear();
        other.original_parameters.clear();
    }

    // splits an instance into multiple, where each one has exactly one equation
    static void UnitSplit(SolverContext&& src, std::list<SolverContext>& dst)
    {
        for (std::unique_ptr<ConstraintEquation>& constraint : src.equations)
        {
            // sanity check
            if (!constraint)
                continue;

            // create a new context in the list
            dst.emplace_back( SolverContext() );
            SolverContext& newContext = dst.back();

            // copy the used parameters
            const ConstraintEquationGradient& grad = constraint.get()->get_gradient();
            for (Param* p : src.parameters) // from the parameters present in the original context
            {
                // ignore those that are not in the gradient of the equation we're taking
                if (std::find(grad.cbegin(), grad.cend(), p) == grad.cend())
                    continue;

                if (newContext.parameters.insert( p ).second)
                    newContext.original_parameters.insert({p, *p});
            }

            // move the equation to that context
            newContext.equations.emplace_back( std::move(constraint) );
        }

        // it's about to be deleted - not necessary
        //equations.clear();
        //parameters.clear();
    }

    // splits one instance into blocks, where equations sharing parameters are grouped in a block, and no blocks share a parameters
    static void BlockSplit(SolverContext&& src, std::list<SolverContext>& dst)
    {
        // one list entry per equation
        src.UnitSplit(std::move(src), dst);

        // now group those sharing parameters
        bool bMerge;
        do {
            bMerge = false;

            for (auto i = dst.begin(); i != dst.end(); ++i)
            {
                for (auto j = dst.begin(); j != dst.end(); ++j)
                {
                    if (i == j)
                        continue;

                    if (!set_common(i->parameters.cbegin(), i->parameters.cend(), j->parameters.cbegin(), j->parameters.cend()))
                        continue;

                    i->splice(*j);

                    auto newj = std::next(j); // j will be invalidated below; incrementing it in for loop will crash
                    dst.erase(j); // erase
                    j = std::prev(newj); // get a valid iterator to be incremented in the loop

                    bMerge = true;
                }
            }

        } while (bMerge);
    }

    // assigns each managed parameter pointer it's original value (from when it was first encountered)
    void RollBack()
    {
        for (auto& pair : original_parameters)
            *pair.first = pair.second;
    }

    // gets sizes
    inline size_t size_equations () const { return equations.size(); }
    inline size_t size_parameters() const { return parameters.size();}

    // iterates for the equations and parameters
    inline decltype(equations)::iterator begin_equ() { return equations.begin(); }
    inline decltype(equations)::iterator end_equ() { return equations.end(); }
    inline decltype(equations)::const_iterator cbegin_equ() const { return equations.cbegin(); }
    inline decltype(equations)::const_iterator cend_equ() const { return equations.cend(); }

    inline decltype(parameters)::iterator begin_par() { return parameters.begin(); }
    inline decltype(parameters)::iterator end_par() { return parameters.end(); }
    inline decltype(parameters)::const_iterator cbegin_par() const { return parameters.cbegin(); }
    inline decltype(parameters)::const_iterator cend_par() const { return parameters.cend(); }
};

// PARAMETERS
// Configurations, convergence criteria
// ====================================================================================================
const static size_t max_iter = 40; // Newton-Raphson usually converges really quickly
const static SolverContext::Param absError = 1E-4; // if the sketch is in m units, precision is 1mm
const static SolverContext::Param relError = 1E-4; // if the sketch is in mm unit, precision is 1um

// NEWTON-RAPHSON
// ====================================================================================================
static bool LeftInvertMatrix(const Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& in, Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& out)
{
    Eigen::FullPivLU<Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>> lu(in.transpose() * in);
    if (!lu.isInvertible())
    {
        #ifdef DEBUG_SKETCH_SOLVER
        std::cerr << "Tall matrix left pseudo-inverse: Matrix is singular" << std::endl;
        std::cout << in << std::endl;
        #endif

        return false;
    }

    out = lu.inverse() * in.transpose();
    return true;
}

bool RightInvertMatrix(const Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& in, Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& out)
{
    Eigen::FullPivLU<Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>> lu(in * in.transpose());
    if (!lu.isInvertible())
    {
        #ifdef DEBUG_SKETCH_SOLVER
        std::cerr << "Wide matrix right pseudo-inverse: Matrix is singular" << std::endl;
        std::cout << in << std::endl;
        #endif

        return false;
    }

    out = in.transpose() * lu.inverse();
    return true;
}

static bool SquareInvertMatrix(const Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& in, Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& out)
{
    Eigen::FullPivLU<Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>> lu(in);
    if (!lu.isInvertible())
    {
        #ifdef DEBUG_SKETCH_SOLVER
        std::cerr << "Square matrix LU inversion: Matrix is singular" << std::endl;
        std::cout << in << std::endl;
        #endif

        return false;
    }

    out = lu.inverse();
    return true;
}

static bool InvertMatrix(const Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& in, Eigen::Matrix<SolverContext::Param, Eigen::Dynamic, Eigen::Dynamic>& out)
{
    if (in.rows() == in.cols())
        return SquareInvertMatrix(in, out);
    else if (in.rows() > in.cols())
        return LeftInvertMatrix(in, out);
    else
        return RightInvertMatrix(in, out);
}

template<typename T>
class Tensor : protected Eigen::Matrix<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Dynamic, 1>
{
protected:
    using ParentType = Eigen::Matrix<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Dynamic, 1>;

public:
    Tensor(unsigned int rows, unsigned int columns, unsigned int levels) :
        ParentType(levels)
    {
        for(unsigned int i = 0; i < levels; i++)
            ParentType::operator()(i, 0).resize(rows, columns);
    }

    inline unsigned int levels() const {
        return ParentType::rows();
    }

    inline unsigned int rows() const {
        return ParentType::operator()(0, 0).rows();
    }

    inline unsigned int cols() const {
        return ParentType::operator()(0, 0).cols();
    }

    auto& operator()(unsigned int r, unsigned int c, unsigned int l)
    {
        return ParentType::operator()(l, 0).operator()(r, c);
    }

    const auto& operator()(unsigned int r, unsigned int c, unsigned int l) const
    {
        return ParentType::operator()(l, 0).operator()(r, c);
    }

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> operator*(const Eigen::Matrix<T, Eigen::Dynamic, 1>& other) const
    {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> output(rows(), levels());

        for(unsigned int i = 0; i < output.cols(); i++)
            output.col(i) = ParentType::operator()(i, 0) * other;

        return output;
    }

    std::ostream& operator<<(std::ostream& os) const
    {
        for(unsigned int i = 0; i < levels(); i++)
            os << ParentType::operator()(i, 0) << std::endl << std::endl;

        return os;
    }
};


class NewtonRaphson
{
public:
    using Param = SolverContext::Param;
    SolverContext& context;

    NewtonRaphson(SolverContext& c) :
        context(c),
        jacobian(context.size_equations(), context.size_parameters()),
        hessian(jacobian.cols(), jacobian.cols(), jacobian.rows()),
        equations(jacobian.rows()),
        parameters(jacobian.cols())
        { }

    bool Iterate(size_t count = max_iter)
    {
        if (jacobian.rows() == 0 || jacobian.cols() == 0)
            return true; // empty system - nothing to do

        // initialize state (parameters)
        ReadParameters();

        Param maxerror; // keep track of the error - used of stop criterion
        for (size_t iter_count = 0; iter_count < count; iter_count++) {
            if (!CalculateDelta(maxerror)) // cannot used richmond method on first step
                return false;

            parameters += delta;

            if (almost_zero(maxerror, absError))
            {
                #ifdef DEBUG_SKETCH_SOLVER
                std::cout << "Converged by absolute error in iteration " << iter_count << std::endl;
                #endif

                return true; // stop iterations - solution reached
            }

            if (SaveParameters() < relError)
            {
                #ifdef DEBUG_SKETCH_SOLVER
                std::cout << "Converged by relative error in iteration " << iter_count << std::endl;
                #endif

                return true;
            }
        }

        #ifdef DEBUG_SKETCH_SOLVER
        std::cout << "Exceeded maximum " << count << " iterations with error " << maxerror << std::endl;
        #endif

        return false;
    }

protected:
    Eigen::Matrix<Param, Eigen::Dynamic, Eigen::Dynamic> jacobian; // functions on lines, derivatives on columns
    Eigen::Matrix<Param, Eigen::Dynamic, Eigen::Dynamic> inverse;  // inverse of J or inverse of J + H*delta
    Tensor<Param> hessian;  // Mijk = d2F(i)/d(j) d(k) for each equation, it's second derivatives with respect to all combinations of parameters

    Eigen::Matrix<Param, Eigen::Dynamic, 1> equations;             // evaluation of functions (goal is to approach zero)
    Eigen::Matrix<Param, Eigen::Dynamic, 1> parameters;            // current values of parameters
    Eigen::Matrix<Param, Eigen::Dynamic, 1> delta;                 // correction for the iteration step

    // samples all the parameters from the references in context
    void ReadParameters()
    {
        unsigned int column = 0;
        for (auto iter = context.cbegin_par(); iter != context.cend_par(); column++, iter++)
            parameters(column) = *(*iter);
    }

    // updates the parameters on the sketch using current values in the vector, and returns the maximum relative difference between new-old
    Param SaveParameters()
    {
        Param maxerror = 0;
        unsigned int column = 0;

        for (auto iter = context.begin_par(); iter != context.end_par(); column++, iter++)
        {
            maxerror = std::fmax(maxerror, std::fabs(
                (*(*iter) - parameters(column))/parameters(column)
            ));

            *(*iter) = parameters(column);
        }

        return maxerror;
    }

    // calculates the correction for one iteration step, returns maximum error of the evaluated equations
    bool CalculateDelta(Param& maxerror, bool bSecondOrder = true)
    {
        maxerror = 0;

        // go over the rows (equations/functions) forming the sytem
        unsigned int row = 0;
        for (auto riter = context.cbegin_equ(); riter != context.cend_equ(); row++, riter++)
        {
            // update value of equation
            equations(row) = riter->get()->current_value();
            maxerror = std::fmax(maxerror, std::fabs( equations(row) ));

            // get gradient function (row)
            const ConstraintEquationGradient grad = riter->get()->get_gradient();

            // go over all parameters (columns)
            unsigned int column = 0;
            for (auto citer = context.cbegin_par(); citer != context.cend_par(); column++, citer++)
            {
                // ConstraintEquationGradient::operator() returns zero if not a variable of this gradient
                // fill the jacobian with derivative of equation(row) with respect to parameter (column)
                jacobian(row, column) = grad((const Param*)*citer);
            }
        }

        // invert jacobian
        if (!InvertMatrix(jacobian, inverse))
            return false;

        delta = -inverse*equations;

        if (bSecondOrder)
        {
            // build hessian matrix
            unsigned int k = 0;
            for (auto row = context.cbegin_equ(); row != context.cend_equ(); row++, k++)
            {
                const ConstraintEquationHessian hess = row->get()->get_hessian();

                unsigned int i = 0;
                for (auto iter = context.cbegin_par(); iter != context.cend_par(); i++, iter++)
                {
                    // fill the hessian with second derivatives by pairing the parameters
                    unsigned int j = 0;
                    for (auto jter = context.cbegin_par(); jter != context.cend_par(); j++, jter++)
                        hessian(i, j, k) = hess((const Param*)*iter, (const Param*)*jter);
                }
            }

            // internal (sub)iteration
            // refine delta using second order terms
            for (size_t internalIterationCount = 0; internalIterationCount < max_iter; internalIterationCount++)
            {
                if (!InvertMatrix(jacobian + Param(0.5)*(hessian*delta).transpose(), inverse))
                    break;

                decltype(delta) old_delta = delta;
                delta = -inverse*equations;

                if ((delta - old_delta).lpNorm<1>() < relError)
                    break;
            }
        }

        return true;
    }
};

// SOLVER
// ====================================================================================================
static bool SolveSketch(SolverContext& context)
{
    std::list<SolverContext> blocks;

    if (context.size_equations() > 1)
        SolverContext::BlockSplit(std::move(context), blocks);
    else
        blocks.emplace_back( std::move(context) );

    #ifdef DEBUG_SKETCH_SOLVER
    std::cout << "Sketch divided into " << blocks.size() << " blocks" << std::endl;
    #endif

    for (auto block = blocks.begin(); block != blocks.end(); block++)
    {
        if (NewtonRaphson(*block).Iterate())
            continue; // solution success

        // failed - must roll back all previous blocks (in reverse order)
        for(;;block--)
        {
            block->RollBack();

            if (block == blocks.begin())
                break;
        }

        return false;
    }

    return true;
}

bool Sketch::Solve(const std::vector<ConstraintEquation::Param*>& constantParameters)
{
    SolverContext context(*this);
    return SolveSketch(context);
}

// SAFE ADDING OF CONSTRAINT
// ====================================================================================================
static const char* msg_overconstrain = "Adding this constraint would over-constrain the sketch.";

SketchConstraintList::iterator Sketch::TryAddConstraint(std::unique_ptr<SketchConstraint> newConstraint, bool msgBox)
{
    // add the new constraint to the sketch
    constraints.emplace_back( std::move(newConstraint) );
    SketchConstraintList::iterator result = std::prev(constraints.end());

    // create context to solve the system
    SolverContext context(*this);
    if (!SolveSketch(context))
    {
        // unable to solve, remove bad constraint, returns end()
        constraints.erase(result);
        result = constraints.end();

        if (msgBox)
            wxMessageBox(msg_overconstrain, wxString::FromAscii(wxMessageBoxCaptionStr), wxICON_ERROR | wxCANCEL);
    }

    return result;
}

std::vector<SketchConstraintList::iterator> Sketch::TryAddConstraints(std::unique_ptr<SketchConstraint>(&&newConstraints)[], size_t count, bool msgBox)
{
    // add the new constraints to sketch
    /*std::vector<SketchConstraintList::iterator> added;
    added.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        constraints.emplace_back( std::move(newConstraints[i]) );
        added.push_back( std::prev(constraints.end()) );
    }

    // all std::unique_ptr in the provided list are now NULLed

    // attempt solving the sketch
    SolverContext context(*this);
    if (SolveSketch(context))
        return added; // success

    // failed to solve
    // all or nothing - erase all the constraints we just added
    while (!added.empty())
    {
        constraints.erase(added.back());
        added.pop_back();
    }

    if (msgBox)
        wxMessageBox(msg_overconstrain, wxString::FromAscii(wxMessageBoxCaptionStr), wxICON_ERROR | wxCANCEL);

    return added;*/
    return std::vector<SketchConstraintList::iterator>();
}
