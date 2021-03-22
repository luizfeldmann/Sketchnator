# Sketchnator

Sketchnator is an *open-source* 2D parametric sketcher.


# Features

* **PARAMETRIC**: Dimensions are inputs, not just annotations
* **CONSTRAINT SOLVER**: You may loosely specify shapes and then apply constraints to converge to the final geometry. The following constraints are currently implemented:
	* Coincident points
	* Point coincident on line
	* Point coincident on circumference
	* Same-length lines
	* Horizontal line
	* Vertical line
	* Parallel lines
	* Orthogonal lines
	* Colinear lines
	* Line tangent to circle
	* Concentric circles
	* Circle tangent to other circle
	* Equal circle radius
* **PRECISE INPUT:** Mouse pointer "snaps" to key features, such as endpoints, centerpoints, midpoints, quadrants, intersections, etc...

## Dependencies

**GUI**: wxWidgets ([https://github.com/wxWidgets/wxWidgets](https://github.com/wxWidgets/wxWidgets))
**Math:** GLM ([https://github.com/g-truc/glm](https://github.com/g-truc/glm)) & Eigen ( [https://eigen.tuxfamily.org/index.php](https://eigen.tuxfamily.org/index.php) ) **[both header-only]**
**Project I/O**: RapidJSON ([https://github.com/Tencent/rapidjson/](https://github.com/Tencent/rapidjson/)) **[header-only]**

## Building

No build system yet. For the moment, please use *Code::Blocks* or just download the release.
Tested only on GCC (8.1).