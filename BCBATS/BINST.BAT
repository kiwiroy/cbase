@echo off
rem blkio installation batch file-----------------------------------------------
rem binst.bat	1.5 - 91/09/23

rem ----------------------------------------------------------------------------
rem NAME
rem      install.bat - blkio library installation batch file for DOS
rem
rem SYNOPSIS
rem      install model [x]
rem
rem DESCRIPTION
rem     install.bat performs the installation of the blkio library for
rem     DOS.  model specifies the memory model as one of the following.
rem
rem          s        small model
rem          m        medium model
rem          c        compact model
rem          l        large model
rem          h        huge model
rem
rem     The library file is named Mblkio.lib, where M would correspond
rem     to the memory model of the library.
rem
rem     If specified, the second parameter causes the reference manual to
rem     be extracted from the source code.  The reference manual is placed
rem     in the file blkio.man.
rem
rem SEE ALSO
rem      makefile
rem
rem NOTES
rem      This batch file is written for use with Borland C++.  To
rem      convert it for use with another compiler, make the following
rem      modifications:
rem           1. Replace \usr\include with the include directory used by
rem              the new compiler.
rem           2. Replace \usr\lib with the library directory used by the
rem              new compiler.
rem           3. Replace bcc with the command to invoke the compiler being
rem              used, replacing the switches also, if necessary.  Below
rem              are listed the Borland C++ switches used and their meanings.
rem                   -O        jump optimization
rem                   -G        speed optimization
rem                   -c        compile but don't link
rem                   -A        ANSI keywords only
rem                   -C-       no nested comments
rem                   -m        memory model
rem           4. The command to build the library archive from the object
rem              modules may vary more drastically.  Borland C++ uses the tlib
rem              command with what is called a response file.  This
rem              response file, blkio.rsp, contains a list of all the
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
goto end
:arg2

if "%3" == "" goto arg3
echo usage:  install model [x]
echo Too many arguments specified.
goto end
:arg3

rem install boolean header file-------------------------------------------------
rem
if not exist \usr\include\bool.h goto bool
echo \usr\include\bool.h exists.  ^C to exit, any other key to continue.
pause
:bool
echo on
copy bool.h \usr\include\bool.h
@echo off

rem extract the reference manual------------------------------------------------
if not "%2" == "x" goto skipman
echo Extracting reference manual into blkio.man.
if not exist blkio.man goto man
echo blkio.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
copy blkio.h/a+bclose.c+bcloseal.c+bexit.c+bflpop.c+bflpush.c+bflush.c tmp
type tmp | manx -c > blkio.man
copy bgetb.c/a+bgetbf.c+bgeth.c+bgethf.c+bopen.c+bputb.c tmp
type tmp | manx -c >> blkio.man
copy bputbf.c/a+bputh.c+bputhf.c+bsetbuf.c+bsetvbuf.c+bsync.c+lockb.c tmp
type tmp | manx -c >> blkio.man
del tmp
@echo off
:skipman

rem compile all blkio source files----------------------------------------------
echo on
bcc -c -O -G -A -C- -m%1 bclose.c   bcloseal.c bexit.c    bflpop.c   bflpush.c  bflush.c
bcc -c -O -G -A -C- -m%1 bgetb.c    bgetbf.c   bgeth.c    bgethf.c   bopen.c    bputb.c
bcc -c -O -G -A -C- -m%1 bputbf.c   bputh.c    bputhf.c   bsetbuf.c  bsetvbuf.c bsync.c    lockb.c
bcc -c -O -G -A -C- -m%1 bops.c     buops.c
@echo off

rem build the blkio library archive---------------------------------------------
if exist %1blkio.lib del %1blkio.lib
echo on
tlib %1blkio @blkio.rsp
@echo off

rem install the blkio library---------------------------------------------------
if not exist \usr\include\blkio.h goto h
echo \usr\include\blkio.h exists.  ^C to exit, any other key to continue.
pause
:h
echo on
copy blkio.h \usr\include\blkio.h
@echo off

if not exist \usr\lib\%1blkio.lib goto lib
echo \usr\lib\%1blkio.lib exists.  ^C to exit, any other key to continue.
pause
:lib
echo on
copy %1blkio.lib \usr\lib\%1blkio.lib
@echo off

rem end of blkio installation batch file----------------------------------------
:end
