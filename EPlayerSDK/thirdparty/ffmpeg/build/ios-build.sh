#!/bin/bash

FF_V="3.2"
CUR_DIR=$(pwd)
SRC_DIR=$(pwd)/../src/"ffmpeg-n"${FF_V}
BIN_OUT=$(pwd)/../export/bin/ios
HEADERS_OUT=$(pwd)/../export/include
OUT_PREFIX=${SRC_DIR}/build_out/ios

SDKVERSION="10.1"
ARCHS="armv7 armv7s x86_64"
DEVELOPER=`xcode-select -print-path`
INTERDIR="${OUT_PREFIX}/archs"

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
    if [ -d "${SRC_DIR}" ]; then
        cd ${SRC_DIR}
        make clean
    fi
    if [ -d "${INTERDIR}" ]; then
        rm -r ${INTERDIR}
    fi
}

function do_ffmpeg_config
{
    echo "now doing configure before build ffmpeg ..."
    ./configure \
        --prefix="${INTERDIR}/${ARCH}" \
        --enable-pic \
        --enable-small \
        --enable-static \
        --enable-network \
        --enable-cross-compile \
        --enable-memalign-hack \
        --disable-doc \
        --disable-vda \
        --disable-sdl2 \
        --disable-iconv \
        --disable-muxers \
        --disable-ffmpeg \
        --disable-symver \
        --disable-shared \
        --disable-ffprobe \
        --disable-ffserver \
        --disable-avdevice \
        --disable-encoders \
        --disable-audiotoolbox \
        --disable-videotoolbox \
        --disable-decoder=tiff \
        --disable-swscale-alpha \
        --disable-securetransport \
        --target-os=darwin \
        --extra-cxxflags="$CPPFLAGS" \
        --sysroot=${SYS_ROOT} \
        --cc="${DEVELOPER}/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang" \
        --extra-cflags="${EXTRA_CFLAGS} -miphoneos-version-min=${SDKVERSION}" \
        --extra-ldflags="-arch ${ARCH} ${EXTRA_LDFLAGS} -miphoneos-version-min=${SDKVERSION}" ${EXTRA_CONFIG}
}

function do_ffmpeg_build
{
    mkdir -p $INTERDIR
    cd ${SRC_DIR}

    for ARCH in ${ARCHS}
    do
        echo "Start build ${ARCH} ffmpeg."
        if [ "${ARCH}" == "x86_64" ]; then
            PLATFORM="iPhoneSimulator"
            EXTRA_CONFIG="--arch=x86_64 --disable-asm  --cpu=x86_64"
            EXTRA_CFLAGS="-arch x86_64"
            SYS_ROOT=$(xcrun --sdk iphonesimulator --show-sdk-path)
        else
            PLATFORM="iPhoneOS"
            EXTRA_CONFIG="--arch=arm --cpu=cortex-a8 --disable-armv5te"
            EXTRA_CFLAGS="-w -arch ${ARCH}"
            SYS_ROOT=$(xcrun --sdk iphoneos --show-sdk-path)
        fi
        mkdir -p "${INTERDIR}/${ARCH}"
        do_ffmpeg_config
        make -j8 && make install && make clean
        echo "Build ${ARCH} ffmpeg done."
    done
}

function create_all_archs_lib
{
    mkdir -p "${INTERDIR}/universal/lib"
    cd "${INTERDIR}/armv7/lib"
    for file in *.a
    do
        cd ${INTERDIR}
        xcrun -sdk iphoneos lipo -output universal/lib/$file  -create -arch armv7 armv7/lib/$file -arch armv7s armv7s/lib/$file -arch x86_64 x86_64/lib/$file
    done
    cp -r ${INTERDIR}/armv7/include ${INTERDIR}/universal/

    if [ -d "${OUT_PREFIX}" ]; then
        echo "bbb"
        cd ${OUT_PREFIX}
        cp -r ${INTERDIR}/universal/* .
    fi
}

function do_build_all
{
    do_pre_build_clean
    if do_ffmpeg_build 0; then
        create_all_archs_lib
        do_build_out_copy
        do_build_post_clean
        echo "All make done"
    else
        echo "FFmpeg build faild !!!"
    fi
    cd ${CUR_DIR}
}

do_build_all
