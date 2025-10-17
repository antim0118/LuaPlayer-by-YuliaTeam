TARGET = LuaPlayerYT

SRC_OBJS = libs/intra/intraFont.o libs/intra/libccc.o libs/ctrl/controls.o \
	libs/g2d/glib2d.o libs/intra/libtext.o libs/vfpu_math.o \
	libs/aalib/pspaalib.o libs/aalib/pspaalibat3.o \
	libs/aalib/pspaalibogg.o libs/aalib/pspaalibflac.o libs/aalib/pspaalibcommon.o \
	libs/aalib/pspaalibeffects.o libs/aalib/pspaalibwav.o \
	libs/pmp/mpegbase.o libs/pmp/audiodecoder.o libs/pmp/avc.o libs/pmp/mem64.o libs/pmp/pmp.o \
	libs/pmp/pmp_decode.o libs/pmp/pmp_file.o libs/pmp/pmp_play.o libs/pmp/pmp_read.o \
	libs/Mp4/atom.o libs/Mp4/util.o libs/Mp4/mp4info.o

LUA_SRC_OBJECTS = lua/src/lapi.o lua/src/lauxlib.o lua/src/lbaselib.o \
	lua/src/lcode.o lua/src/ldblib.o lua/src/ldebug.o lua/src/ldo.o \
	lua/src/ldump.o lua/src/lfunc.o lua/src/lgc.o lua/src/linit.o \
	lua/src/liolib.o lua/src/llex.o lua/src/lmathlib.o lua/src/lmem.o \
	lua/src/loadlib.o lua/src/lobject.o lua/src/lopcodes.o lua/src/loslib.o \
	lua/src/lparser.o lua/src/lstate.o lua/src/lstring.o lua/src/lstrlib.o \
	lua/src/ltable.o lua/src/ltablib.o lua/src/ltm.o lua/src/lundump.o \
	lua/src/lvm.o lua/src/lzio.o lua/src/print.o

LUA_OBJS = lua/graphics.o lua/LUA.o lua/ctrl.o lua/system.o lua/timer.o lua/audio.o lua/usb.o lua/vfpu_math.o

OBJS = $(SRC_OBJS) $(LUA_SRC_OBJECTS) $(LUA_OBJS) LP.o

INCDIR =
CFLAGS = -O3 -G0 -Wall -Wno-trigraphs
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

#PSP Stuff
BUILD_PRX = 1
PSP_FW_VERSION = 660

LIBS = -lpge -lpng -ljpeg -lpsppower -lz -lpspgu -lm -lc -lFLAC -lvorbisidec -lpspaudio \
       -lpspaudiocodec -lpspaudiolib -lpspatrac3 -lpspmp3 -lpspmpeg \
       -lpspsystemctrl_kernel -lpsprtc -lpspgum -lpspvfpu -lpspusb -lpspusbstor \
       -lpspkubridge -lpspvram -lbz2 -lfreetype

EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = LPYT PMP TEST
PSP_EBOOT_TITLE = LuaPlayerYT 0.5
PSP_EBOOT_ICON = res/icon_cassette-2.png
PSP_EBOOT_SND0 = res/fitgirl.at3
PSP_EBOOT_PIC1 = res/image_14.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak