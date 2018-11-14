#!/bin/sh

ZLIB_VERSION=1.2.8
LIBPNG_VERSION=1.6.21
IMAGEMAGICK_VERSION=7.0.8-14

export CHOST=arm-linux-gnueabihf
export PREFIX=${PWD}/deps
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
export CPPFLAGS=-I${PREFIX}/include
export LDFLAGS=-L${PREFIX}/lib
export PNG_COPTS="-I${PREFIX}/include -L${PREFIX}/lib"

set -ex

download() {
	cd ${PREFIX}/src
	if ! [ -f $(basename $1) ]; then
		wget $1
	fi
}

version_check() {
	if [ $(pkg-config --modversion "$1") = "$2" ]; then
		return 0
	fi
	return 1
}

build() {
	cd ${PREFIX}/src
	tar xvzf ${1}*.tar.gz
	cd ${1}-*
	shift
	./configure --prefix=${PREFIX} ${1+$@}
	make
	make install
}

build_imagemagick() {
	download https://imagemagick.org/download/ImageMagick.tar.gz
	if ! version_check MagickCore ${1}; then
		build ImageMagick \
			--host=${CHOST} \
			--without-utilities \
			--disable-shared \
			--disable-openmp \
			--disable-docs \
			--with-pkgconfigdir=${PKG_CONFIG_PATH}

	fi
}

build_libpng() {
	download https://downloads.sourceforge.net/project/libpng/libpng16/older-releases/${1}/libpng-${1}.tar.gz
	if ! version_check libpng ${1}; then
		build libpng \
			--host=${CHOST} \
			--with-pkgconfigdir=${PKG_CONFIG_PATH}
	fi
}

build_zlib() {
	download http://www.zlib.net/fossils/zlib-${ZLIB_VERSION}.tar.gz
	if ! version_check zlib ${1}; then
		build zlib
		rm ${PREFIX}/lib/libz.so
	fi
}

mkdir -p ${PREFIX}/src
build_zlib $ZLIB_VERSION
build_libpng $LIBPNG_VERSION
build_imagemagick $IMAGEMAGICK_VERSION
