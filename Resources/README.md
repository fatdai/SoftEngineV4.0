1. homebrew 安装 sdl2 sdl2_image sdl2_ttf
2. Build Settings 下设置 Library Search Paths :
   /usr/local/Cellar/sdl2/2.0.3/lib
   /usr/local/Cellar/sdl2_image/2.0.0_1/lib
   /usr/local/Cellar/sdl2_ttf/2.0.12/lib
3. Build Settings 下设置 Header Search Paths :
   /usr/local/Cellar/sdl2/2.0.3/include/SDL2
   /usr/local/Cellar/sdl2_image/2.0.0_1/include/SDL2
   /usr/local/Cellar/sdl2_ttf/2.0.12/include/SDL2

4. Build Settings 下设置  Other Linker Flags :
   -lSDL2
   -lSDL2_image
   -lSDL2_ttf

5. 修改 test 文件夹 main 函数里面的 
     (!initEngine("/Users/user/work/xcode_workspace/SoftEngineV4.0/Resources/")
   这个路径