if not exist bin (
    mkdir bin
)
C:/VulkanSDK/1.3.236.0/Bin/glslc.exe default.vert -o bin/default_vert.spv
C:/VulkanSDK/1.3.236.0/Bin/glslc.exe default.frag -o bin/default_frag.spv
pause