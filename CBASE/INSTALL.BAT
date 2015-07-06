@echo off
rem cbase installation batch file-----------------------------------------------
rem install.bat	1.5 - 91/09/23

rem ----------------------------------------------------------------------------
rem NAME
rem      install.bat - cbase library installation batch file for DOS
rem
rem SYNOPSIS
rem      install model [x]
rem
rem DESCRIPTION
rem     install.bat performs the installation of the cbase library for
rem     DOS.  model specifies the memory model as one of the following.
rem
rem          s        small model
rem          m        medium model
rem          c        compact model
rem          l        large model
rem          h        huge model
rem
rem     The library file is named Mcbase.lib, where M would correspond
rem     to the memory model of the library.
rem
rem     If specified, the second parameter causes the reference manual to
rem     be extracted from the source code.  The reference manual is placed
rem     in the file cbase.man.
rem
rem SEE ALSO
rem      makefile
rem
rem NOTES
rem      This batch file is written for use with Borland Turbo C.  To
rem      convert it for use with another compiler, make the following
rem      modifications:
rem           1. Replace \usr\include with the include directory used by
rem              the new compiler.
rem           2. Replace \usr\lib with the library directory used by the
rem              new compiler.
rem           3. Replace tcc with the command to invoke the compiler being
rem              used, replacing the switches also, if necessary.  Below
rem              are listed the Turbo C switches used and their meanings.
rem                   -c        compile but don't link
rem                   -O        jump optimization
rem                   -G        speed optimization
rem                   -A        ANSI keywords only
rem                   -C-       no nested comments
rem                   -m        memory model
rem           4. The command to build the library archive from the object
rem              modules may vary more drastically.  Turbo C uses the tlib
rem              command with what is called a response file.  This
rem              response file, cbase.rsp, contains a list of all the
rem              object modules in the library.
rem
rem ----------------------------------------------------------------------------

rem verify arguments------------------------------------------------------------
if "%1" == "s" goto arg1
if "%1" == "m" goto arg1
if "%1" == "c" goto arg1
if "%1" == "l" goto arg1
if "%1" == "h" goto arg1
echo usage:  install model [x]
echo Valid values for model are s (small), m (medium), c (compact),
echo   l (large), and h (huge).  Model must be lower case.
echo If x is specified, the reference manual will be extracted.
goto end
:arg1

if "%2" == "" goto arg2
if "%2" == "x" goto arg2
echo usage:  install model [x]
echo Invalid second argument.  Valid value is lowercase x.
:arg2

if "%3" == "" goto arg3
echo usage:  install model [x]
echo Too many arguments specified.
:arg3

rem check if underlying libraries installed-------------------------------------
if exist \usr\include\lseq.h goto lseq
echo The lseq library must be installed first.
goto end
:lseq
if exist \usr\include\btree.h goto btree
echo The btree library must be installed first.
goto end
:btree

rem extract the reference manual------------------------------------------------
if not "%2" == "x" goto skipman
echo Extracting reference manual into cbase.man.
if not exist cbase.man goto man
echo cbase.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
type cbase.h | manx -c > cbase.man
copy cbclose.c/a+cbcreate.c+cbdelcur.c+cbexport.c+cbgetkcu.c+cbgetlck.c tmp
type tmp | manx -c >> cbase.man
copy cbgetr.c/a+cbgetrcu.c+cbgetrf.c+cbimport.c+cbinsert.c+cbkcurso.c tmp
type tmp | manx -c >> cbase.man
copy cbkeyali.c/a+cbkeyfir.c+cbkeylas.c+cbkeynex.c+cbkeypre.c+cbkeysrc.c tmp
type tmp | manx -c >> cbase.man
copy cblock.c/a+cbmkndx.c+cbopen.c+cbputr.c+cbrcurso.c+cbrecali.c tmp
type tmp | manx -c >> cbase.man
copy cbreccnt.c/a+cbrecfir.c+cbreclas.c+cbrecnex.c+cbrecpre.c+cbrecsiz.c tmp
type tmp | manx -c >> cbase.man
copy cbrmndx.c/a+cbsetkcu.c+cbsetrcu.c+cbsync.c tmp
type tmp | manx -c >> cbase.man
del tmp
@echo off
:skipman

rem compile all cbase source files----------------------------------------------
echo on
tcc -c -O -G -A -C- -m%1 cbclose.c  cbcreate.c cbdelcur.c cbexport.c cbgetkcu.c cbgetlck.c
tcc -c -O -G -A -C- -m%1 cbgetr.c   cbgetrcu.c cbgetrf.c  cbimport.c cbinsert.c cbkeyali.c
tcc -c -O -G -A -C- -m%1 cbkeyfir.c cbkeylas.c cbkeynex.c cbkeypre.c cbkeysrc.c cblock.c
tcc -c -O -G -A -C- -m%1 cbmkndx.c  cbopen.c   cbputr.c   cbrecali.c cbrecfir.c cbreclas.c
tcc -c -O -G -A -C- -m%1 cbrecnex.c cbrecpre.c cbrmndx.c  cbsetkcu.c cbsetrcu.c cbsync.c
tcc -c -O -G -A -C- -m%1 cbcmp.c    cbexp.c    cbimp.c    cbops.c
@echo off

rem build the cbase library archive---------------------------------------------
if exist %1cbase.lib del %1cbase.lib
echo on
tlib %1cbase @cbase.rsp
@echo off

rem install the cbase library---------------------------------------------------
if not exist \usr\include\cbase.h goto h
echo \usr\include\cbase.h exists.  ^C to exit, any other key to continue.
pause
:h
echo on
copy cbase.h \usr\include\cbase.h
@echo off

if not exist \usr\lib\%1cbase.lib goto lib
echo \usr\lib\%1cbase.lib exists.  ^C to exit, any other key to continue.
pause
:lib
echo on
copy %1cbase.lib \usr\lib\%1cbase.lib
@echo off

rem end of cbase installation batch file----------------------------------------
:end
