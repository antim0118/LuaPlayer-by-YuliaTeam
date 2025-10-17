cmake_minimum_required(VERSION 3.10)
project(LuaPlayerYT)

# Исходные файлы
set(SRC_OBJS
    libs/intra/intraFont.c
    libs/intra/libccc.c
    libs/ctrl/controls.c
    libs/g2d/glib2d.c
    libs/intra/libtext.c
    libs/pmp/mpegbase.S
    libs/pmp/audiodecoder.c
    libs/pmp/avc.c
    libs/pmp/mem64.c
    libs/pmp/pmp.c
    libs/pmp/pmp_decode.c
    libs/pmp/pmp_file.c
    libs/pmp/pmp_play.c
    libs/pmp/pmp_read.c
    libs/aalib/pspaalibwav.c
    libs/aalib/pspaalibeffects.c
    libs/aalib/pspaalibat3.c
    libs/aalib/pspaalib.c
)

set(LUA_SRC_OBJECTS
    lua/src/lapi.c
    lua/src/lauxlib.c
    lua/src/lbaselib.c
    lua/src/lcode.c
    lua/src/ldblib.c
    lua/src/ldebug.c
    lua/src/ldo.c
    lua/src/ldump.c
    lua/src/lfunc.c
    lua/src/lgc.c
    lua/src/linit.c
    lua/src/liolib.c
    lua/src/llex.c
    lua/src/lmathlib.c
    lua/src/lmem.c
    lua/src/loadlib.c
    lua/src/lobject.c
    lua/src/lopcodes.c
    lua/src/loslib.c
    lua/src/lparser.c
    lua/src/lstate.c
    lua/src/lstring.c
    lua/src/lstrlib.c
    lua/src/ltable.c
    lua/src/ltablib.c
    lua/src/ltm.c
    lua/src/lundump.c
    lua/src/lvm.c
    lua/src/lzio.c
    lua/src/print.c
)

set(LUA_OBJS
    lua/graphics.c
    lua/LUA.c
    lua/ctrl.c
    lua/system.c
    lua/timer.c
    lua/audio.c
    lua/usb.c
)

set(OBJS
    ${SRC_OBJS}
    ${LUA_SRC_OBJECTS}
    ${LUA_OBJS}
    LP.c
)

# Исполняемый файл
add_executable(${PROJECT_NAME} ${OBJS})

# Подключение библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    png
    jpeg
    z
    pspgu
    pspvram
    pspkubridge
    pspaudiolib
    pspaudiocodec
    pspmpeg
    pmp
    vorbisidec
    pspsystemctrl_kernel
    pspatrac3
    pspmp3
    psprtc
    pspaudio
    m
    pspgum
    pspvfpu
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH "res/icon_cassette-2.png"
    BACKGROUND_PATH "res/image_14.png"
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)