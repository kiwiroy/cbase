@echo off
rem rolodeck installation batch file--------------------------------------------
rem rdinst.bat	1.5 - 91/09/23

rem ----------------------------------------------------------------------------
rem NAME
rem      install.bat - rolodeck installation batch file for DOS
rem
rem SYNOPSIS
rem      install model [x]
rem
rem DESCRIPTION
rem     install.bat performs the installation of the rolodeck program for
rem     DOS.  model specifies the memory model as one of the following.
rem
rem          S        small model
rem          M        medium model
rem          C        compact model
rem          L        large model
rem          H        huge model
rem
rem     If specified, the second parameter causes the reference manual to
rem     be extracted from the source code.  The reference manual is placed
rem     in the file rolodeck.man.
rem
rem SEE ALSO
rem      makefile
rem
rem NOTES
rem      This batch file is written for use with Microsoft C.  To convert
rem      it for use with another compiler, make the following
rem      modifications:
rem           1. Replace \usr\include with the include directory used by
rem              the new compiler.
rem           2. Replace cl with the command to invoke the compiler being
rem              used, replacing the switches also, if necessary.  Below
rem              are listed the Microsoft C switches used and their
rem              meanings.
rem                   -Oalt     optimization
rem                             a - relax alias checking
rem                             l - enable loop optimization
rem                             t - favor execution speed
rem                   -Za       ANSI keywords only
rem                   -A        memory model
rem
rem ----------------------------------------------------------------------------

rem verify arguments------------------------------------------------------------
if "%1" == "S" goto arg1
if "%1" == "M" goto arg1
if "%1" == "C" goto arg1
if "%1" == "L" goto arg1
if "%1" == "H" goto arg1
echo usage:  install model [x]
echo Valid values for model are S (small), M (medium), C (compact),
echo   L (large), and H (huge).  Model must be upper case.
echo If x is specified, the reference manual will be extracted.
goto end
:arg1

if "%2" == "" goto arg2
if "%2" == "x" goto arg2
echo usage:  install model [x]
echo Invalid second argument.  Valid value is lowercase x.
goto end
:arg2

if "%3" == "" goto arg3
echo usage:  install model [x]
echo Too many arguments specified.
goto end
:arg3

rem check if required libraries installed---------------------------------------
if exist \usr\include\cbase.h goto cbase
echo The cbase library must be installed first.
goto end
:cbase

rem extract the reference manual------------------------------------------------
if not "%2" == "x" goto skipman
echo Extracting reference manual into rolodeck.man.
if not exist rolodeck.man goto man
echo rolodeck.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
copy rolodeck.c/a+cvtss.c+fdcset.c+fml.c tmp
type tmp | manx -c > rolodeck.man
del tmp
@echo off
:skipman

rem make rolodeck program-------------------------------------------------------
echo on
cl -c -Oalt -A%1 cvtss.c fdcset.c
cl -Oalt -Za -A%1 rolodeck.c fml.c cvtss.obj %1cbase.lib %1btree.lib %1lseq.lib %1blkio.lib
@echo off

rem end of rolodeck installation batch file-------------------------------------
:end
