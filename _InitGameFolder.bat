@echo off

set /p isDel="Will delete unnecessary folder(regenerated as needed), are you sure? (Y/N):"

IF %isDel%==Y (
 goto delete

) ELSE IF %isDel%==y (
 goto delete

) ELSE (
 echo;
 echo Canceled delete.
 timeout /t -1
 goto :EOF
)

:delete
 echo;
echo Start deleting.
set dir=Game

cd %dir%/

echo;
rd /s /q .vs
echo %dir%/.vs Completed.
echo;
rd /s /q Binaries
echo %dir%/Binaries Completed.
echo;
rd /s /q Build
echo %dir%/Build Completed.
echo;
rd /s /q DerivedDataCache
echo %dir%/DerivedDataCache Completed.
echo;
rd /s /q Intermediate
echo %dir%/Intermediate Completed.
echo;
rd /s /q Saved
echo %dir%/Saved Completed.
echo;
del %dir%.sln
echo %dir%/%dir%.sln Completed.
echo;
echo Delete completed.
timeout /t -1
