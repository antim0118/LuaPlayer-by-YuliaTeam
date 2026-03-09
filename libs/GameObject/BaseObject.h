#include <pspkernel.h>
#include <pspdebug.h>
#include <psprtc.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <kubridge.h>
#include <pspintrman.h>
#include <pspthreadman.h>
#include <psploadexec_kernel.h>
#include <pspopenpsid.h>

#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"

#include <psppower.h>
#include <psptypes.h>
#include <malloc.h>
#include <psputils.h>
#include <png.h>
#include <pspgu.h>
#include <zlib.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspsdk.h>

#include "Structs.h"


int CreateBaseObject(lua_State *L);
BaseObject *GetBaseObjectByName(const char *name);