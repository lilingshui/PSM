@echo off

echo @******************************************************
echo @   Compiling and Linking batch program for TP900S C program
echo @	
echo @	Copyright (c) 2010 by Thinta Co.LTD
echo @	All rights reserved
echo @******************************************************

SET INSTALL_PATH=C:\TP850_C
SET INC_PATH=%INSTALL_PATH%\INC
SET LIB_PATH=%INSTALL_PATH%\LIB
SET ICF_FILE=%INSTALL_PATH%\CMD\ram.scf

SET ICC=%INSTALL_PATH%\CMD\armcc
SET ILINK=%INSTALL_PATH%\CMD\armlink
SET IELF=%INSTALL_PATH%\CMD\fromelf

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
if exist %SRC%.bxe del %SRC%.bxe

echo.
echo Compiling Sorce Files...
echo.

%ICC% -c -g+ -O0 -Ec -errors err_cpl.txt %SRC%.c -DEMBEDDED -I %INC_PATH%  

if not exist %SRC%.o goto ErrorCompile

echo.
echo Linking Object Files...
echo.

%ILINK% -o  %SRC%.axf   -scov %ICF_FILE%  -errors err_lnk.txt   -first start.o(Init)   -scf -elf   -libpath %LIB_PATH%  %INSTALL_PATH%\cmd\start.o %INSTALL_PATH%\cmd\TP850_Lib  %SRC%.o
if not exist %SRC%.axf goto ErrorLink

echo.
echo Creating Image...
echo.

%IELF% %SRC%.axf -bin  %OUTPUT_PATH%\%SRC%.bxe
del %SRC%.axf

:Succed
echo ___________________Hello! Congratulation!___________________
goto End

:ErrorCompile
type err_cpl.txt
goto End

:ErrorLink
type err_lnk.txt

:End
if exist %SRC%.o del %SRC%.o
pause