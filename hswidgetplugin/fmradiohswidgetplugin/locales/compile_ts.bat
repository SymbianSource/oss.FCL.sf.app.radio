@echo off

set QM_FILE=fmradiohswidgetplugin.qm
set QM_FILE_EN_US=fmradiohswidgetplugin_en_us.qm
set QM_LOCATION=./


ECHO Creating QM file
call lrelease -idbased fmradiohswidgetplugin.ts -qm %QM_LOCATION%%QM_FILE%
REM call lrelease -idbased fmradiohswidgetplugin_en_us.ts -qm %QM_LOCATION%%QM_FILE_EN_US%
