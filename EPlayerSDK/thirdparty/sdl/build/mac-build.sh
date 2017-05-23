#!/bin/bash

SDL_V="2.0.5"
CUR_DIR=$(pwd)
SRC_DIR=$(pwd)/../src/"SDL2-"${SDL_V}
BIN_OUT=$(pwd)/../export/bin/mac
HEADERS_OUT=$(pwd)/../export/include
OUT_PREFIX=${SRC_DIR}/build_out/mac

function do_pre_build_clean
{
    echo "clean all before build."
    if [ -d "${OUT_PREFIX}" ]; then
        cd ${OUT_PREFIX}
        rm -rf *
    else
        cd ${SRC_DIR}
        if [ -d "build_out" ]; then
            cd build_out
            mkdir mac
        else
            mkdir build_out
            cd build_out
            mkdir mac
        fi
    fi
}

function do_build_out_copy
{
    echo "Copy all libs and header files."
    if [ -d "${BIN_OUT}" ]; then
        cd ${BIN_OUT}
        rm -rf *
        cp -r ${OUT_PREFIX}/lib/libSDL2.a .
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

function do_sdl_config
{
cd ${SRC_DIR}
echo "now doing configure before build ffmpeg ..."
./configure \
    CFLAGS="-D__APPLE__" \
    --prefix=${OUT_PREFIX} \
    --enable-shared \
    --enable-static \
    --enable-audio \
    --enable-video \
    --enable-render \
    --enable-events \
    --enable-video-cocoa \
    --disable-esd \
    --disable-nas \
    --disable-mmx \
    --disable-sse \
    --disable-oss \
    --disable-ime \
    --disable-ibus \
    --disable-sse2 \
    --disable-alsa \
    --disable-dbus \
    --disable-arts \
    --disable-power \
    --disable-fcitx \
    --disable-rpath \
    --disable-3dnow \
    --disable-sndio \
    --disable-loadso \
    --disable-haptic \
    --disable-timers \
    --disable-ssemath \
    --disable-cpuinfo \
    --disable-esdtest \
    --disable-joystick \
    --disable-assembly \
    --disable-diskaudio \
    --disable-esd-shared \
    --disable-pulseaudio \
    --disable-dummyaudio \
    --disable-nas-shared \
    --disable-alsa-shared \
    --disable-arts-shared \
    --disable-sndio-shared \
    --disable-pulseaudio-shared \
    $ADDITIONAL_CONFIGURE_FLAG
}

function do_sdl_build
{
    cd ${SRC_DIR}
    make clean
    make -j8
    make install
}

function do_build_all
{
    do_pre_build_clean
    if do_sdl_config 0; then
        if do_sdl_build 0; then
            do_build_out_copy
            do_build_post_clean
            echo "Make done"
        else
            echo "Make failed !!!"
        fi
    else
        echo "FFmpeg configure faild !!!"
    fi
    cd ${CUR_DIR}
}

do_build_all
