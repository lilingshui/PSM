@echo off

echo @******************************************************
echo @   Compiling and Linking batch program for TP900S C program
echo @	
echo @	Copyright (c) 2010 by Thinta Co.LTD
echo @	All rights reserved
echo @******************************************************

SET INSTALL_PATH=C:\TP900S_SDE
SET INC_PATH=%INSTALL_PATH%\INC
SET LIB_PATH=%INSTALL_PATH%\LIB
SET ICF_FILE=%INSTALL_PATH%\CMD\RAM.icf

SET ICC=%INSTALL_PATH%\CMD\iccarm
SET ILINK=%INSTALL_PATH%\CMD\ilinkarm
SET IELF=%INSTALL_PATH%\CMD\ielftool
SET OUTPUT_PATH=%cd%\bin

echo ============== Install path ===========
echo %INSTALL_PATH%
echo ============== Include path ===========
echo %INC_PATH%
echo ============== Lib path ===============
echo %LIB_PATH%
echo ============== Icf path ===============
echo %ICF_FILE%
echo ============== Icc path ===============
echo %ICC%
echo ============== Ilink path =============
echo %ILINK%
echo ============== Ielf path ==============
echo %IELF%
echo ============== Output path ============
echo %OUTPUT_PATH%

SET SRC=hello
if not exist %OUTPUT_PATH% md %OUTPUT_PATH%

if exist err_cpl.txt del err_cpl.txt
if exist err_lnk.txt del err_lnk.txt
if exist %SRC%.o del %SRC%.o
if exist %SRC%.out del %SRC%.out
if exist %SRC%.dxe del %SRC%.dxe

echo.
echo Compiling Sorce Files...
echo.

%ICC% --silent --cpu cortex-m3 --cpu_mode thumb -e --endian little --dlib_config %INC_PATH%\DLib_Config_Full.h -o %SRC%.o %SRC%.c -I %INC_PATH%\ -I . -On > err_cpl.txt 2>&1

if not exist %SRC%.o goto ErrorCompile

echo.
echo Linking Object Files...
echo.

%ILINK% --silent %LIB_PATH%\TP900S_Lib.a %SRC%.o -o %SRC%.out --config %ICF_FILE% --semihosting --entry T12_program_start > err_lnk.txt 2>&1
if not exist %SRC%.out goto ErrorLink

echo.
echo Creating Image...
echo.

%IELF% --silent --bin %SRC%.out %OUTPUT_PATH%\%SRC%.dxe

:Succed
echo ___________________Hello! Congratulation!___________________
goto End

:ErrorCompile
type err_cpl.txt
goto End

:ErrorLink
type err_lnk.txt
goto End

:End
if exist %SRC%.o del %SRC%.o
if exist %SRC%.out del %SRC%.out
pause