@echo off

set program=ember.exe
set bin_dir=.\bin\
set lnk_dir=.\obj\
set defines=/DDEBUG
set options=/W4 /Zi /Fe%bin_dir%%program% /Fo:%lnk_dir% %defines%

if not exist %bin_dir% mkdir %bin_dir%
if not exist %lnk_dir% mkdir %lnk_dir%

cl %options% ./engine/build_engine.c
