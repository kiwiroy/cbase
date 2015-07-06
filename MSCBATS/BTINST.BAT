@echo off
rem btree installation batch file-----------------------------------------------
rem btinst.bat	1.5 - 91/09/23

rem ----------------------------------------------------------------------------
rem NAME
rem      install.bat - btree library installation batch file for DOS
rem
rem SYNOPSIS
rem      install model [x]
rem
rem DESCRIPTION
rem     install.bat performs the installation of the btree library for
rem     DOS.  model specifies the memory model as one of the following.
rem
rem          S        small model
rem          M        medium model
rem          C        compact model
rem          L        large model
rem          H        huge model
rem
rem     The library file is named Mbtree.lib, where M would correspond
rem     to the memory model of the library.
rem
rem     If specified, the second parameter causes the reference manual to
rem     be extracted from the source code.  The reference manual is placed
rem     in the file btree.man.
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
rem           2. Replace \usr\lib with the library directory used by the
rem              new compiler.
rem           3. Replace cl with the command to invoke the compiler being
rem              used, replacing the switches also, if necessary.  Below
rem              are listed the Microsoft C switches used and their
rem              meanings.
rem                   -c        compile but don't link
rem                   -Oalt     optimization
rem                             a - relax alias checking
rem                             l - enable loop optimization
rem                             t - favor execution speed
rem                   -Za       ANSI keywords only
rem                   -A        memory model
rem           4. The command to build the library archive from the object
rem              modules may vary more drastically.  Microsoft C uses the
rem              lib command with what is called a response file.  This
rem              response file, btree.rsp, contains a list of all the
rem              object modules in the library.
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

rem check if blkio installed----------------------------------------------------
if exist \usr\include\blkio.h goto blkio
echo The blkio library must be installed first.
goto end
:blkio

rem extract the reference manual------------------------------------------------
if not "%2" == "x" goto skipman
echo Extracting reference manual into btree.man.
if not exist btree.man goto man
echo btree.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
type btree.h | manx -c > btree.man
copy btclose.c/a+btcreate.c+btcursor.c+btdelcur.c+btdelete.c+btfirst.c tmp
type tmp | manx -c >> btree.man
copy btfix.c/a+btgetcur.c+btgetk.c+btgetlck.c+btinsert.c+btkeycmp.c tmp
type tmp | manx -c >> btree.man
copy btkeycnt.c/a+btkeysiz.c+btlast.c+btlock.c+btnext.c+btopen.c tmp
type tmp | manx -c >> btree.man
copy btprev.c/a+btsearch.c+btsetbuf.c+btsetcur.c+btsetvbu.c+btsync.c tmp
type tmp | manx -c >> btree.man
del tmp
@echo off
:skipman

rem compile all btree source files----------------------------------------------
echo on
cl -c -Oalt -Za -A%1 btclose.c  btcreate.c btdelcur.c btdelete.c btfirst.c  btfix.c
cl -c -Oalt -Za -A%1 btgetcur.c btgetk.c   btgetlck.c btinsert.c btkeycmp.c btlast.c
cl -c -Oalt -Za -A%1 btlock.c   btnext.c   btopen.c   btprev.c   btsearch.c btsetbuf.c
cl -c -Oalt -Za -A%1 btsetcur.c btsetvbu.c btsync.c
cl -c -Oalt -Za -A%1 btops.c    dgops.c    kyops.c    ndops.c
@echo off

rem build the btree library archive---------------------------------------------
if exist %1btree.lib del %1btree.lib
echo on
lib %1btree @btree.rsp
@echo off

rem install the btree library---------------------------------------------------
if not exist \usr\include\btree.h goto h
echo \usr\include\btree.h exists.  ^C to exit, any other key to continue.
pause
:h
echo on
copy btree.h \usr\include\btree.h
@echo off

if not exist \usr\lib\%1btree.lib goto lib
echo \usr\lib\%1btree.lib exists.  ^C to exit, any other key to continue.
pause
:lib
echo on
copy %1btree.lib \usr\lib\%1btree.lib
@echo off

rem make btdemo program---------------------------------------------------------
cl -Oalt -Za -A%1 btdemo.c %1btree.lib %1blkio.lib

rem end of btree installation batch file----------------------------------------
:end
