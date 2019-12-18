RPATH=./
GPATH=../example/out/
MAKEFLAGS += --no-print-directory
web:
	cp $(RPATH)/html5/out.js $(RPATH)/html5/out.js.mem -r $(RPATH)/html5/html/* $(GPATH)
win:
	cp $(RPATH)/windows/Game.exe $(GPATH)
lin:
	cp $(RPATH)/linux/Game $(GPATH)

#cd $(GPATH)/ &&mkdir html5 ;emcc -O3 $(RPATH)bin/html5/*.a -s USE_GLFW=3  -o html5/out.js && cp -r $(RPATH)bin/html5/* html5