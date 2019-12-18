DIR= ../..
GAME=cmake $(DIR) -DEDITOR=false
MFLG=--no-print-directory
Default: linux web editor
all:	editor linux windows web android
	
editor:
	cd build/Editor && cmake $(DIR) -DMAKE_EDITOR=true ;make $(MFLG) && ../../Engine/raccoon
linux:
	cd build/linux && $(GAME) ;make $(MFLG)
windows:
	cd build/windows && $(GAME) -DCMAKE_TOOLCHAIN_FILE=../modules/Windows.cmake && make $(MFLG)
web:
	cd build/html5  && make $(MFLG)
android:
	cd build/android
clean:
	rm -rf build/Editor/* build/linux/* build/html5/build/* build/windows/* build/android/*
cleanall: clean
	rm Engine/raccoon ;rm Engine/bin/*/*;rm -r Engine/example/out/*

deb:
	gdb Engine/raccoon
val:
	valgrind --leak-check=full Engine/raccoon
v:
	valgrind  Engine/raccoon
vf:
	valgrind -v --track-origins=yes Engine/raccoon
setup:
	sudo apt install cmake libgl-dev libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev libxi-dev libopenal-dev libglu-dev
