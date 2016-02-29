echo off
REM ----------------------------------------------------------------------
REM Batch file for handling AVR mcu's via eclipse and atprogram
REM ----------------------------------------------------------------------
REM
REM Commands:
REM
REM list 	- list all programmers attached to computer
REM info	- information from MCU
REM erase	- erase MCU
REM program	- program MCU
REM verify  - verify program in MCU
REM 


REM Settings
REM ----------------------------------------------------------------------

set file=Debug\EventQueue.elf

REM Select MCU type
SET mcu=atmega328

REM Select programmer type
SET tool=jtagICEmkii

REM Select interface type
REM ISP JTAG PDI TPI SWD debugWire 
SET interface=ISP

REM programming speed
SET clk=2mhz
REM ----------------------------------------------------------------------


echo -----------------------------------------
echo Devicetype: %mcu%
echo Tool:       %tool%
echo Interface:  %interface%
echo Speed:      %clk%

echo -----------------------------------------
rem echo argument 1 %1 %mcu

REM get current dir to a variable
cd > tempfile123
set /p PWD= < tempfile123
del tempfile123
echo %PWD%


if "%1"=="list"    GOTO list
if "%1"=="info"    GOTO info
if "%1"=="program" GOTO program
if "%1"=="erase"   GOTO erase
if "%1"=="version" GOTO version
if "%1"=="verify"  GOTO verify

REM List programming devices
:list
	tools\atprogram  list
	GOTO end
:info
	tools\atprogram -t %tool% -d %mcu% -i %interface% info
	GOTO end
:program
	echo on
	tools\atprogram -t %tool% -d %mcu% -i %interface% program  --verify --chiperase --flash --file %PWD%\%file% 
	GOTO end
:verify
	tools\atprogram -t %tool% -d %mcu% -i %interface% verify --file %PWD%\%file% 
	GOTO end
:erase
	atprogram -t %tool% -d %mcu% -i %interface% chiperase
	GOTO end
:version
	tools\atprogram version
	GOTO end

	
:end