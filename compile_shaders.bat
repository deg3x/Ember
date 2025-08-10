@echo off

set compile=%VULKAN_SDK%/Bin/glslc
set out_dir=.\bin\
set shd_dir=.\engine\shaders\

%compile% %shd_dir%triangle.vert -o %out_dir%triangle_vert.spv
%compile% %shd_dir%triangle.frag -o %out_dir%triangle_frag.spv
