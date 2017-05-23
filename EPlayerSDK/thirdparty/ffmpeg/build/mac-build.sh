#!/bin/bash

FF_V="3.2"
CUR_DIR=$(pwd)
SRC_DIR=$(pwd)/../src/"ffmpeg-"${FF_V}
BIN_OUT=$(pwd)/../export/bin/mac
HEADERS_OUT=$(pwd)/../export/include
OUT_PREFIX=${SRC_DIR}/build_out/mac

function do_pre_build_clean
{
    echo "clean all before build."
    if [ -d "${OUT_PREFIX}" ]; then
        cd ${OUT_PREFIX}
        rm -rf *
    fi
}

function do_build_out_copy
{
    echo "Copy all libs and header files."
    if [ -d "${BIN_OUT}" ]; then
        cd ${BIN_OUT}
        rm -rf *
        cp -r ${OUT_PREFIX}/lib/*.a .
    fi
    if [ -d "${HEADERS_OUT}" ]; then
        cd ${HEADERS_OUT}
        rm -rf *
        cp -r ${OUT_PREFIX}/include/* .
    fi
}

function do_build_post_clean
{
    echo "clean all the build temp files."
    if [ -d "${HEADERS_OUT}" ]; then
        cd ${SRC_DIR}
        make clean
        rm -rf build_out
    fi
}

function do_ffmpeg_config
{
cd ${SRC_DIR}
echo "now doing configure before build ffmpeg ..."
./configure \
    --prefix=$OUT_PREFIX \
    --target-os=darwin \
    --enable-static \
    --enable-small \
    --enable-network \
    --enable-memalign-hack \
    --disable-doc \
    --disable-vda \
    --disable-sdl2 \
    --disable-iconv \
    --disable-muxers \
    --disable-ffmpeg \
    --disable-symver \
    --disable-shared \
    --disable-logging \
    --disable-ffprobe \
    --disable-ffserver \
    --disable-avdevice \
    --disable-encoders \
    --disable-audiotoolbox \
    --disable-videotoolbox \
    --disable-decoder=tiff \
    --disable-swscale-alpha \
    --disable-securetransport \
    --extra-ldflags="$ADDI_LDFLAGS" \
    $ADDITIONAL_CONFIGURE_FLAG
}

function do_ffmpeg_build
{
    cd ${SRC_DIR}
    make clean
    make
    make install
}

function do_build_all
{
    do_pre_build_clean
    if do_ffmpeg_config 0; then
        if do_ffmpeg_build 0; then
            do_build_out_copy
            do_build_post_clean
            echo "Make done"
        else
            echo "Make faild !!!"
        fi
    else
        echo "FFmpeg configure faild !!!"
    fi
    cd ${CUR_DIR}
}

do_build_all
