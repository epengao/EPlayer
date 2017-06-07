#!/bin/bash

FF_V="3.2"
CUR_DIR=$(pwd)
SRC_DIR=$(pwd)/../src/"ffmpeg-n"${FF_V}
BIN_OUT=$(pwd)/../export/bin/ios
HEADERS_OUT=$(pwd)/../export/include
OUT_PREFIX=${SRC_DIR}/build_out/ios

DEPLOYMENT_TARGET="10.1"
ARCHS=("arm64" "x86_64")
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
        --arch=$ARCH \
        --cc="$CC" \
        --as="$AS" \
        --extra-cflags="$CFLAGS" \
        --extra-ldflags="$LDFLAGS"
}

function do_ffmpeg_build
{
    mkdir -p $INTERDIR
    cd ${SRC_DIR}

    for ARCH in ${ARCHS[*]}
    do
        CFLAGS="-arch $ARCH"
        if [ $ARCH = "i386" -o $ARCH = "x86_64" ]
        then
            PLATFORM="iPhoneSimulator"
            CFLAGS="$CFLAGS -mios-simulator-version-min=$DEPLOYMENT_TARGET"
        else
            PLATFORM="iPhoneOS"
            CFLAGS="$CFLAGS -mios-version-min=$DEPLOYMENT_TARGET -fembed-bitcode"
            if [ $ARCH = "arm64" ]
            then
                EXPORT="GASPP_FIX_XCODE5=1"
                # force "configure" to use "gas-preprocessor.pl" (FFmpeg 3.3)
                AS="gas-preprocessor.pl -arch aarch64 -- $CC"
            fi
        fi
        XCRUN_SDK=`echo $PLATFORM | tr '[:upper:]' '[:lower:]'`
        CC="xcrun -sdk $XCRUN_SDK clang"
        AS="$CC"
        CXXFLAGS="$CFLAGS"
        LDFLAGS="$CFLAGS"

        mkdir -p "${INTERDIR}/${ARCH}"
        do_ffmpeg_config
        make -j8 && make install && make clean
        echo "Build ${ARCH} ffmpeg done."
    done
}

function create_all_archs_lib
{
    mkdir -p "${INTERDIR}/universal/lib"
    archCount=${#ARCHS[*]}
    if [ ${archCount} -gt 0 ]; then
        cd "${INTERDIR}/${ARCHS[0]}/lib"
        for file in *.a
        do
            cd ${INTERDIR}
                PAK_ARCS=""
                for ARCH in ${ARCHS[*]}
                do
                    if [ $ARCH = "armv7" ]
                    then
                        PAK_ARCS=${PAK_ARCS}" -create -arch armv7 armv7/lib/"${file}
                    elif  [ $ARCH = "armv7s" ]
                    then
                        PAK_ARCS=${PAK_ARCS}" -create -arch armv7s armv7s/lib/"${file}
                    elif  [ $ARCH = "arm64" ]
                    then
                        PAK_ARCS=${PAK_ARCS}" -create -arch arm64 arm64/lib/"${file}
                    elif  [ $ARCH = "i386" ]
                    then
                        PAK_ARCS=${PAK_ARCS}" -create -arch i386 i386/lib/"${file}
                    elif  [ $ARCH = "x86_64" ]
                    then
                        PAK_ARCS=${PAK_ARCS}" -create -arch x86_64 x86_64/lib/"${file}
                    fi
                done
            xcrun -sdk iphoneos lipo -output universal/lib/$file $PAK_ARCS
        done
        cp -r ${INTERDIR}/${ARCHS[0]}/include ${INTERDIR}/universal/

        if [ -d "${OUT_PREFIX}" ]; then
            cd ${OUT_PREFIX}
            cp -r ${INTERDIR}/universal/* .
        fi
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
