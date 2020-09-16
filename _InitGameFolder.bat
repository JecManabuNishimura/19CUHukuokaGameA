@echo off

echo --------------------------------------------------------------------------
echo 不要フォルダを削除します。
echo ビルドできない時、プロジェクトを起動できないときに使ってください。
echo;  
echo 削除が終わったら、Generate Visual Studio project filesを行ってください。
echo --------------------------------------------------------------------------
echo;

rem       ↓の名前はプロジェクトが入ってる場所になります
set dir="Game"

rem フォルダ階層の移動-------------------------
echo;
echo フォルダ %dir%/に移動します
cd %dir%/
rem -------------------------------------------

rem フォルダ削除-------------------------------
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
rem -------------------------------------------