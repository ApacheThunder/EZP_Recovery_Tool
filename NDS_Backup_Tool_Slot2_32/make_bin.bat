pause
@echo off
call setenv_devkitPro.bat

:loop
cls
goto skipclean
del /Q arm7\build\*.*
del /Q arm9\build\*.*
:skipclean

del arm7\_BOOT_MP.arm7.elf
del arm9\_BOOT_MP.arm9.elf
del _BOOT_MP.bin
rem del "C:\APP\_Network\anhttpd\public\wifitemp.nds"
make
if exist _BOOT_MP.nds goto run
pause
goto loop

:run
rem ren _BOOT_MP.nds _BOOT_MP.bin
del _BOOT_MP.arm7
del _BOOT_MP.arm9
rem del _BOOT_MP.ds.gba
rem call cfcopy.bat
rem call wificopy.bat
pause
goto loop

