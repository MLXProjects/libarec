@echo off
pushd %~dp0

if exist out\obj\ del /f /q out\obj
if not exist out\obj\ mkdir out\obj\

popd