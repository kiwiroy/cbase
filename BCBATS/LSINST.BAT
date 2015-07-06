@echo off
rem lseq installation batch file------------------------------------------------
rem lsinst.bat	1.5 - 91/09/23

rem ----------------------------------------------------------------------------
rem NAME
rem      install.bat - lseq library installation batch file for DOS
rem
rem SYNOPSIS
rem      install model [x]
rem
rem DESCRIPTION
rem     install.bat performs the installation of the lseq library for
rem     DOS.  model specifies the memory model as one of the following.
rem
rem          s        small model
rem          m        medium model
rem          c        compact model
rem          l        large model
rem          h        huge model
rem
rem     The library file is named Mlseq.lib, where M would correspond
rem     to the memory model of the library.
rem
rem     If specified, the second parameter causes the reference manual to
rem     be extracted from the source code.  The reference manual is placed
rem     in the file lseq.man.
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
rem              response file, lseq.rsp, contains a list of all the
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

rem check if blkio installed----------------------------------------------------
if exist \usr\include\blkio.h goto blkio
echo The blkio library must be installed first.
goto end
:blkio

rem extract the reference manual------------------------------------------------
if not "%2" == "x" goto skipman
echo Extracting reference manual into lseq.man.
if not exist lseq.man goto man
echo lseq.man exists.  ^C to exit, any other key to continue.
pause
:man
if not exist tmp goto tmp
echo tmp exists.  ^C to exit, any other key to continue.
pause
:tmp
echo on
type lseq.h | manx -c >lseq.man
copy lsclose.c/a+lscreate.c+lscursor.c+lsdelcur.c+lsfirst.c+lsgetcur.c tmp
type tmp | manx -c >> lseq.man
copy lsgetlck.c/a+lsgetr.c+lsgetrf.c+lsinsert.c+lslast.c+lslock.c tmp
type tmp | manx -c >> lseq.man
copy lsnext.c/a+lsopen.c+lsprev.c+lsputr.c+lsputrf.c+lsreccnt.c tmp
type tmp | manx -c >> lseq.man
copy lsrecsiz.c/a+lssearch.c+lssetbuf.c+lssetcur.c+lssetvbu.c+lssync.c tmp
type tmp | manx -c >> lseq.man
del tmp
@echo off
:skipman

rem compile all lseq source files-----------------------------------------------
echo on
bcc -c -O -G -A -C- -m%1 lsclose.c  lscreate.c lsdelcur.c lsfirst.c  lsgetcur.c lsgetlck.c
bcc -c -O -G -A -C- -m%1 lsgetr.c   lsgetrf.c  lsinsert.c lslast.c   lslock.c   lsnext.c
bcc -c -O -G -A -C- -m%1 lsopen.c   lsprev.c   lsputr.c   lsputrf.c  lssearch.c lssetbuf.c
bcc -c -O -G -A -C- -m%1 lssetcur.c lssetvbu.c lssync.c
bcc -c -O -G -A -C- -m%1 lsops.c    rcops.c
@echo off

rem build the lseq library archive----------------------------------------------
if exist %1lseq.lib del %1lseq.lib
echo on
tlib %1lseq @lseq.rsp
@echo off

rem install the lseq library----------------------------------------------------
if not exist \usr\include\lseq.h goto h
echo \usr\include\lseq.h exists.  ^C to exit, any other key to continue.
pause
:h
echo on
copy lseq.h \usr\include\lseq.h
@echo off

if not exist \usr\lib\%1lseq.lib goto lib
echo \usr\lib\%1lseq.lib exists.  ^C to exit, any other key to continue.
pause
:lib
echo on
copy %1lseq.lib \usr\lib\%1lseq.lib
@echo off

rem end of lseq installation batch file-----------------------------------------
:end
