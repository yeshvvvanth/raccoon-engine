def: cmake install
cmake:
	cd build&& cmake .. -DCMAKE_C_COMPILER=emcc -DCMAKE_CXX_COMPILER=em++ && make
install:
	emcc -O3 build/libegame.a build/bt/libbullet.a build/ft/libfreetype.a -s USE_GLFW=3  -o ../../Engine/bin/html5/out.js


