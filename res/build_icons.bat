@echo OFF
SETLOCAL ENABLEDELAYEDEXPANSION

for %%d in (%~dp0..) do set ParentDirectory=%%~fd

SET targetDir=%ParentDirectory%\bin\Debug\
SET resourDir=%targetDir%res\
SET size=64

echo Top level target directory is "%targetDir%"
echo Resources output directory is "%resourDir%"

if not exist "%resourDir%" (
 mkdir "%resourDir%"
 echo Does not exist: created!
) else (
 echo Resources directory exists!
)

FOR %%I in ("%~dp0*.svg") DO (
 SET fullpath_svg="%%I"
 SET nameonly_svg="%%~nI%%~xI"
 SET fullpath_png="%resourDir%%%~nI.png"

 if not exist !fullpath_png! (
   echo Convert !fullpath_svg! to !fullpath_png!
   inkscape !fullpath_svg! --export-filename=!fullpath_png! --export-width=%size% --export-height=%size%
 ) else (
   echo Skipping !fullpath_svg!
 )
)