ver = release1
obj = build/crypt_colortrain.o  build/bignum.o  build/platform.o  build/platform_util.o build/simple_rsa.o
CC = gcc
#CC = x86_64-w64-mingw32-gcc
include_path = \
-Imbedtls/include \
-Imbedtls/include/mbedtls

ifeq ($(ver),release)
 build/crypt_colortrain.exe:$(obj)
	$(CC) $(obj) -o  build/crypt_colortrain.exe
 build/crypt_colortrain.o:crypt_colortrain.c
	$(CC) -c crypt_colortrain.c -o build/crypt_colortrain.o
build/bignum.o:mbedtls/source/bignum.c
	$(CC) $(include_path) -c mbedtls/source/bignum.c -o build/bignum.o
build/platform.o:mbedtls/source/platform.c
	$(CC) $(include_path) -c mbedtls/source/platform.c -o build/platform.o
build/platform_util.o:mbedtls/source/platform_util.c
	$(CC) $(include_path) -c mbedtls/source/platform_util.c -o build/platform_util.o
build/simple_rsa.o:simple_rsa.c
	$(CC) $(include_path) -c simple_rsa.c -o build/simple_rsa.o
else
 build/crypt_colortrain.exe:$(obj)
	$(CC) $(obj)  -g -o  build/crypt_colortrain.exe
 build/crypt_colortrain.o:crypt_colortrain.c
	$(CC) -g -c crypt_colortrain.c -o build/crypt_colortrain.o
build/bignum.o:mbedtls/source/bignum.c
	$(CC) $(include_path) -g -c mbedtls/source/bignum.c -o build/bignum.o
build/platform.o:mbedtls/source/platform.c
	$(CC) $(include_path) -g -c mbedtls/source/platform.c -o build/platform.o
build/platform_util.o:mbedtls/source/platform_util.c
	$(CC) $(include_path) -g -c mbedtls/source/platform_util.c -o build/platform_util.o
build/simple_rsa.o:simple_rsa.c
	$(CC) $(include_path) -g -c simple_rsa.c -o build/simple_rsa.o
endif

.PHONY:clean
clean:
	-rm -rf build/* ColorTrainKey*
