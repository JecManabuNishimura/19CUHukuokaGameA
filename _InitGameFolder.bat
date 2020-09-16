@echo off

set dir="Game"

cd %dir%/

echo;
rd /s .vs
echo;
rd /s Binaries
echo;
rd /s Build
echo;
rd /s DerivedDataCache
echo;
rd /s Intermediate
echo;
rd /s Saved
echo;
del /p Game.sln
