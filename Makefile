all: 
	gcc -c -fpic -Wall edid.c -g -D VERBOSE=0
	gcc -shared -o libedid.so edid.o -lm
	gcc -o test_edidlib test_edidlib.c -Wall -g -L$(PWD) -ledid
	gcc -o test_edidlib_drm test_edidlib_drm.c -Wall -I/usr/include/drm -g -ldrm -lm -L$(PWD) -ledid

clean:
	rm -rf libedid.so test_edidlib test_edidlib_drm *.o

lib:
	gcc -c -fpic -Wall edid.c libedid-api.c -g
	gcc -shared -o libedid.so edid.o libedid-api.o -lm

clean-lib:
	rm -rf edid.o libedid-api.o libedid.so

test: 
	gcc -o test_edidlib test_edidlib.c -Wall -g -L$(PWD) -ledid

clean-test:
	rm -rf test_edidlib

test-drm:
	gcc -o test_edidlib_drm test_edidlib_drm.c  -Wall -I/usr/include/drm -g -ldrm -lm -L$(PWD) -ledid

clean-test-drm:
	rm -rf test_edidlib_drm

test-api:
	gcc -o test-api test-libedid-api.c -g -L$(PWD) -ledid

verbose:
	gcc -c -fpic -Wall edid.c libedid-api.c -g -D VERBOSE=1
	gcc -shared -o libedid.so edid.o libedid-api.o -lm
	gcc -o test-api test-libedid-api.c -g -L$(PWD) -ledid
	gcc -o test_edidlib test_edidlib.c -Wall -g -L$(PWD) -ledid
	gcc -o test_edidlib_drm test_edidlib_drm.c -Wall -I/usr/include/drm -g -ldrm -lm -L$(PWD) -ledid
	
