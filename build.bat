@echo off

set program=ember.exe
set bin_dir=.\bin\
set lnk_dir=.\obj\
set defines=/DBUILD_DEBUG=1 /DPLATFORM_WINDOWS=1 /DEMBER_ASSERT_ENABLED=1 /DRHI_VULKAN=1 /DRHI_VK_VALIDATIONS_ENABLED=1
set cmp_opt=/W4 /Zi /Fe%bin_dir%%program% /Fo:%lnk_dir% /I%VULKAN_SDK%\Include %defines%
set lnk_opt=/link /SUBSYSTEM:WINDOWS /INCREMENTAL:NO /LIBPATH:%VULKAN_SDK%\Lib
set depends=user32.lib shell32.lib vulkan-1.lib

if not exist %bin_dir% mkdir %bin_dir%
if not exist %lnk_dir% mkdir %lnk_dir%

cl %cmp_opt% ./engine/build_engine.c %depends% %lnk_opt%
