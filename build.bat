@echo off

set program=ember.exe
set bin_dir=.\bin\
set lnk_dir=.\obj\
set defines=/DDEBUG /DPLATFORM_WINDOWS /DEMBER_ASSERT_ENABLED
set cmp_opt=/W4 /Zi /Fe%bin_dir%%program% /Fo:%lnk_dir% %defines%
set lnk_opt=/link /SUBSYSTEM:WINDOWS /INCREMENTAL:NO
set depends=user32.lib shell32.lib

if not exist %bin_dir% mkdir %bin_dir%
if not exist %lnk_dir% mkdir %lnk_dir%

cl %cmp_opt% ./engine/build_engine.c %depends% %lnk_opt%
