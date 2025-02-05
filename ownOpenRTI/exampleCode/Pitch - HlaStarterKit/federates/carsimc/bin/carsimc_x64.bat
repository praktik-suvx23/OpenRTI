@echo off
title Car Sim C
setlocal

IF "%PRTI1516E_HOME%"=="" echo WARNING! PRTI1516E_HOME environment variable has not been set

rem ********************************************
rem * CONNECTING TO THE RTI USING THE C++ API  *
rem ********************************************

set PATH=%PRTI1516E_HOME%\jre\bin\server;%PRTI1516E_HOME%\lib\vc140_64;%PATH%

CarSimC.exe

pause
endlocal