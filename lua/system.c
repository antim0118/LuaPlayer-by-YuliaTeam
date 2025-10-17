/*--------------------------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#------------- This file is part of: ---------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#----------  ___  --------------------------------------  ________      ___  -----------------------------------------------------------------#
#---------- |   | -------------------------------------- |         \   |   | -----------------------------------------------------------------#
#---------- |   | -------------------------------------- |______    \  |   | -----------------------------------------------------------------#
#---------- |   | --------------------------------------         \   \ |   | -----------------------------------------------------------------#
#---------- |   |          ___     ___     _________      ______ /   / |   |    _________   ___     ___   ___________   __________  ----------#
#---------- |   |         |   |   |   |   /         |    |          /  |   |   /         | |   |   |   | |    ____   | |           \ ---------#
#---------- |   |         |   |   |   |  /    __    |    |    ____ /   |   |  /    __    | |   |   |   | |   |____|  | |    ____   / ---------#
#---------- |   |         |   |   |   | /    /  |   |    |   |         |   | /    /  |   | |   |   |   | |    _______| |   |    \_/ ----------#
#---------- |   |_______  |   |___|   | \    \  |   |    |   |         |   | \    \  |   | |   |___|   | |   |_______  |   |        ----------#
#---------- |           |  \          |  \    | |   |    |   |         |   |  \    | |   |  \          | |           \ |   |        ----------#
#---------- |___________|   \_________|   \___| |___|    |___|         |___|   \___| |___|   \_____    | |___________/ |___|        ----------#
#------------------------------------------------------------------------------------------------- |   | -------------------------------------#
#------------------------------------------------------------------------------------------- ______|   | -------------------------------------#
#---------------------------------------------  ___     ___   ___________  ---------------- /         /  -------------------------------------#
#--------------------------------------------- \   \   /   / |           | ---------------- \________/   -------------------------------------#
#---------------------------------------------- \   \ /   /  |___     ___|--------------------------------------------------------------------#
#----------------------------------------------- \       /       |   | -----------------------------------------------------------------------#
#------------------------------------------------ \     /        |   | -----------------------------------------------------------------------#
#------------------------------------------------- |   |         |   | -----------------------------------------------------------------------#
#------------------------------------------------- |   |         |   | -----------------------------------------------------------------------#
#------------------------------------------------- |   |         |   | -----------------------------------------------------------------------#
#------------------------------------------------- |   |         |   | -----------------------------------------------------------------------#
#------------------------------------------------- |___|         |___| -----------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#------------------------------------------------------------------------------------------ Made by Kodilo -----------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#- Special Thanks to: ------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#- BenHur for intraFont ----------------------------------------------------------------------------------------------------------------------#
#- Geecko for gLib2D -------------------------------------------------------------------------------------------------------------------------#
#- Arshia001 for PSPAALIB --------------------------------------------------------------------------------------------------------------------#
#- jonny & Raphael for PMP Mod ---------------------------------------------------------------------------------------------------------------#
#- InsertWittyName & MK2k for PGE source code ------------------------------------------------------------------------------------------------#
#- Rinnegatamante & Nanni for example of awesome Lua Player ----------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#--------------------------------------------------------------------------------------------------------------------------------------------*/

#include "system.h"

PspUtilitySavedataListSaveNewData luaData;
static char key[] = "QTAK319JQKJ952HA";

char LPYTGameID[256];
char LPYTGameTitle[256];

typedef struct
{
    char signature[4];
	int version;
	int offset[8];

} LPYTEboot;

char LPYTSaveNames[][20] =
{
	"0000",
	"0001",
	"0002",
	"0003",
	"0004",
    "0005",
	"0006",
	"0007",
	"0008",
	"0009",
    ""
};

unsigned char SYSTEM_FileExist(const char *filename)
{
    SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    if (fd <= 0) return 0;
    sceIoClose(fd);
    return 1;
}

static int SYSTEM_getCFWVersion(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getOSV() takes no arguments.");
	
    char buf[16];
	sprintf(buf, "%x", sceKernelDevkitVersion());
	buf[1] = '.'; buf[3] = buf[4];
	buf[4] = 0;
	lua_pushstring(L, buf);

	return 1;
}

static int SYSTEM_Rename(lua_State *L)
{
    if(lua_gettop(L) != 2) 
        return luaL_error(L, "System.rename(file, newName) takes 2 arguments");

    if(sceIoRename(luaL_checkstring(L, 1), luaL_checkstring(L, 2)) < 0)
        return luaL_error(L, "System.rename() error renaming");

    return 0;
}

static int SYSTEM_removeFile(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "System.removeFile(path) takes 1 argument");

    if(sceIoRemove(luaL_checkstring(L, 1)) < 0)
        return luaL_error(L, "System.removeFile() error");

    return 0;
}

static int SYSTEM_removeDir(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "System.removeDir(path) takes 1 argument");

    if(sceIoRmdir(luaL_checkstring(L, 1)) < 0)
        return luaL_error(L, "System.removeDir() error");
    
    return 0;
}

static int SYSTEM_createDir(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "System.createDir(path) takes 1 argument");

    if(sceIoMkdir(luaL_checkstring(L, 1), 0777) < 0)
        return luaL_error(L, "System.createDir() error creating directory");
    
    return 0;
}

static int SYSTEM_curDir(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "System.currentDir(newDirectory) takes 1 argument");

    char directory[256];
    getcwd(directory, 256);
    lua_pushstring(L, directory);

    if(lua_gettop(L))
        chdir(luaL_checkstring(L, 1));
    
    return 1;
}

static int SYSTEM_getBattPerc(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getBatteryPercent() takes no arguments");

    lua_pushnumber(L, scePowerGetBatteryLifePercent());

    return 1;
}

static int SYSTEM_getBattLifeTime(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getBatteryLifeTime() takes no arguments");

    lua_pushnumber(L, scePowerGetBatteryLifeTime());
    return 1;
}

int SYSTEMint_getMODEL()
{
    return (kuKernelGetModel()+1);
}

void SYSTEM_getTITLEID(const char *path, char *title, char *id)
{
    int index = 0;
    char buffer[256];
    FILE *EBOOT = fopen(path, "rb");

    fseek(EBOOT, 0x134, SEEK_SET);

    char c;
    while (fread(&c, 1, 1, EBOOT) == 1 && c != 0x00 && index < sizeof(buffer)-1)
        buffer[index++] = c;
        
    buffer[index] = '\0';

    strcpy(id, buffer);

    index = 0;

    fseek(EBOOT, 0x158, SEEK_SET);

    while (fread(&c, 1, 1, EBOOT) == 1 && c != 0x00 && index < sizeof(buffer)-1)
        buffer[index++] = c;
        
    buffer[index] = '\0';

    strcpy(title, buffer);

    fclose(EBOOT);
}

static int SYSTEM_getModel(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getModel() takes no arguments");
	
    int rev = SYSTEMint_getMODEL();
    char model[64];

    switch (rev)
    {
    case 1:
        sprintf(model, "PSP-100X (0%dg) [FAT]", rev);
        break;
    case 2:
        sprintf(model, "PSP-200X (0%dg) [SLIM]", rev);
        break;
    case 3:
        sprintf(model, "PSP-300X (0%dg) [BRIGHT]", rev);
        break;
    case 4:
        sprintf(model, "PSP-300X (0%dg) [BRIGHT]", rev);
        break;
    case 5:
        sprintf(model, "PSP-N100X (0%dg) [GO]", rev);
        break;
    case 7:
        sprintf(model, "PSP-300X (0%dg) [BRIGHT]", rev);
        break;
    case 9:
        sprintf(model, "PSP-300X (0%dg) [BRIGHT]", rev);
        break;
    case 11:
        sprintf(model, "PSP-E100X (0%dg) [STREET]", rev);
        break;
    default:
        sprintf(model, "Unknown (0%dg) [UNKNOWN]", rev);
        break;
    }
    
	lua_pushfstring(L, model);

	return 1;
}

static int SYSTEM_getCPU(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getCPU() takes no arguments");
    
    lua_pushnumber(L, scePowerGetCpuClockFrequency());

    return 1;
}

static SceIoDirent dir;

static int SYSTEM_isFile(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "System.isFile(path) takes 1 argument");

    int rez = SYSTEM_FileExist((const char*)luaL_checkstring(L, 1));

    if (rez)
        lua_pushboolean(L, TRUE);
    else
        lua_pushboolean(L, FALSE);

    return 1;
}

static int SYSTEM_isDir(lua_State *L)
{
    if (lua_gettop(L) != 0 && lua_gettop(L) != 1)
        return luaL_error(L, "System.isDir([path]) takes 0 or 1 argument");

    const char* path = lua_gettop(L) == 1 ? luaL_checkstring(L, 1) : "";

    int fd = sceIoDopen(path);
    
    if (fd < 0)
    {
        lua_pushboolean(L, FALSE);
    }
    else
    {
        lua_pushboolean(L, TRUE);
        sceIoDclose(fd);
    }

    return 1;
}


static int SYSTEM_listDir(lua_State *L)
{
    if(lua_gettop(L) != 0 && lua_gettop(L) != 1)
        return luaL_error(L, "System.listDir([path]) takes 0 or 1 argument");

    int fd = sceIoDopen(lua_gettop(L) == 1 ? luaL_checkstring(L, 1) : "");
    if(fd < 0)
        return luaL_error(L, "System.listDir() error listing directory");

    lua_newtable(L);
    int i = 0;
    
    while(sceIoDread(fd, &dir) > 0)
    {
        lua_pushnumber(L, i++);

        lua_newtable(L);
            lua_pushstring(L, "name");
            lua_pushstring(L, dir.d_name);
            lua_settable(L, -3);
        
            lua_pushstring(L, "size");
            lua_pushnumber(L, dir.d_stat.st_size);
            lua_settable(L, -3);
        lua_settable(L, -3);
    }
    
    sceIoDclose(fd);

    return 1;
}

static int SYSTEM_memclean(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.GC() takes no arguments");
    
    lua_gc(L, LUA_GCCOLLECT, 0L);

    return 0;
}

/*static int SYSTEM_LowCPU(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.LowCPU() takes no arguments");
    
    if(scePowerGetCpuClockFrequency() != 100)
    {
        scePowerSetCpuClockFrequency(100);
        scePowerSetBusClockFrequency(50);
    }

    return 0;
}

static int SYSTEM_MedCPU(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.MedCPU() takes no arguments");
    
    if(scePowerGetCpuClockFrequency() != 222)
    {
        scePowerSetCpuClockFrequency(222);
        scePowerSetBusClockFrequency(111);
    }
    
    return 0;
}

static int SYSTEM_HighCPU(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.HighCPU() takes no arguments");
    
    if(scePowerGetCpuClockFrequency() != 333)
        scePowerSetClockFrequency(333, 333, 166);
        
    return 0;
}*/

void pushFormattedTime(const char *key, int value, lua_State *L)
{
    lua_pushstring(L, key);
    char buffer[3];
    if (value < 10)
    {
        snprintf(buffer, sizeof(buffer), "0%d", value);
        lua_pushstring(L, buffer);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "%d", value);
        lua_pushstring(L, buffer);
    }
    
    lua_settable(L, -3);
}

static int SYSTEM_getTime(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getTime() takes no arguments");

    pspTime time;
    sceRtcGetCurrentClockLocalTime(&time);

    lua_newtable(L);

    pushFormattedTime("hour", time.hour, L);
    pushFormattedTime("minutes", time.minutes, L);
    pushFormattedTime("seconds", time.seconds, L);
    pushFormattedTime("month", time.month, L);
    pushFormattedTime("day", time.day, L);
    
    lua_pushstring(L, "microseconds");
    lua_pushnumber(L, time.microseconds);
    lua_settable(L, -3);

    lua_pushstring(L, "year");
    lua_pushnumber(L, time.year);
    lua_settable(L, -3);
    
    return 1;
}

static int SYSTEM_GetGAMEID(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getGameID() takes no arguments");

    lua_pushstring(L, LPYTGameID);

    return 1;
}

void SaveData_Init(SceUtilitySavedataParam* LPYTSaveDataParams, int mode, void* savedata, const char* subTitle, const char* description, const char *key, unsigned int datasize, const char *ebootpath, const char *icon0path, char* AutoSaveFolder)
{
    memset(LPYTSaveDataParams, 0, sizeof(SceUtilitySavedataParam));
    LPYTSaveDataParams->base.size = sizeof(SceUtilitySavedataParam);

    void *icon0data = NULL;
    int icon0size = 0;
    void *pic1data = NULL;
    int pic1size = 0;
    void *snd0data = NULL;
    int snd0size = 0;
    void *icon1data = NULL;
    int icon1size = 0;
    void *clean0data = NULL;
    int clean0size = 0;
    
    char *titleshow;

    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &LPYTSaveDataParams->base.language);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &LPYTSaveDataParams->base.buttonSwap);

    if (LPYTSaveDataParams->base.language == 8)
        titleshow = "Новое сохранение";
    else
        titleshow = "New Save";

    LPYTSaveDataParams->base.graphicsThread = 17;
    LPYTSaveDataParams->base.accessThread = 19;
    LPYTSaveDataParams->base.fontThread = 18;
    LPYTSaveDataParams->base.soundThread = 16;

    LPYTSaveDataParams->mode = mode;
    LPYTSaveDataParams->overwrite = 1;

    if (mode == PSP_UTILITY_SAVEDATA_LISTLOAD || mode == PSP_UTILITY_SAVEDATA_LISTDELETE)
        LPYTSaveDataParams->focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;
    else 
        LPYTSaveDataParams->focus = PSP_UTILITY_SAVEDATA_FOCUS_FIRSTEMPTY; 


#if _PSP_FW_VERSION >= 200
    strncpy(LPYTSaveDataParams->key, key, 16);
#endif
    strcpy(LPYTSaveDataParams->gameName, LPYTGameID);
    strcpy(LPYTSaveDataParams->saveName, "<>");

    if (mode == PSP_UTILITY_SAVEDATA_AUTOSAVE || mode == PSP_UTILITY_SAVEDATA_AUTOLOAD)
        strcpy(LPYTSaveDataParams->saveName, AutoSaveFolder);
    else
        LPYTSaveDataParams->saveNameList = LPYTSaveNames;

    strcpy(LPYTSaveDataParams->fileName, "DATA.BIN");

    LPYTSaveDataParams->dataBuf = malloc(datasize);
    memset(LPYTSaveDataParams->dataBuf, 0, datasize);
    LPYTSaveDataParams->dataBufSize = datasize;
    LPYTSaveDataParams->dataSize = datasize;

    if (mode == PSP_UTILITY_SAVEDATA_LISTSAVE || mode == PSP_UTILITY_SAVEDATA_AUTOSAVE)
    {
        strcpy(LPYTSaveDataParams->dataBuf, savedata);
        
        strcpy(LPYTSaveDataParams->sfoParam.title, LPYTGameTitle);
        strcpy(LPYTSaveDataParams->sfoParam.savedataTitle, subTitle);
        strcpy(LPYTSaveDataParams->sfoParam.detail, description);
        LPYTSaveDataParams->sfoParam.parentalLevel = 1;

        LPYTSaveDataParams->focus = PSP_UTILITY_SAVEDATA_FOCUS_FIRSTEMPTY;

        if (icon0path != NULL)
        {
            if (SYSTEM_FileExist(icon0path))
            {
                SceUID fd = sceIoOpen(icon0path, PSP_O_RDONLY, 0777);
                icon0size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                sceIoLseek32(fd, 0, PSP_SEEK_SET);
                
                icon0data = malloc(icon0size);
                sceIoRead(fd, icon0data, icon0size);
                sceIoClose(fd);
            }  
        }

        if ((strstr(ebootpath, "EBOOT.PBP") || strstr(ebootpath, "eboot.pbp")) > 0)
        {
            LPYTEboot EBOOTFile;

            int fd;
            int ebootlength;
            int filesize;
            
            fd = sceIoOpen(ebootpath, PSP_O_RDONLY, 0777);
            ebootlength = sceIoLseek32(fd, 0, PSP_SEEK_END);

            sceIoLseek32(fd, 0, PSP_SEEK_SET);

            sceIoRead(fd, &EBOOTFile, sizeof(EBOOTFile));
            
            
            filesize = EBOOTFile.offset[1 + 1] - EBOOTFile.offset[1];

            if (filesize > 0)
            {
                if (icon0data == NULL)
                {
                    sceIoLseek32(fd, EBOOTFile.offset[1], PSP_SEEK_SET);
                    icon0data = malloc(filesize);
                    memset(icon0data, 0, filesize);
                    sceIoRead(fd, icon0data, filesize);
                    icon0size = filesize;
                }
                    sceIoLseek32(fd, EBOOTFile.offset[1], PSP_SEEK_SET);
                    clean0data = malloc(filesize);
                    sceIoRead(fd, clean0data, filesize);
                    clean0size = filesize;
            }
            
            filesize = EBOOTFile.offset[2 + 1] - EBOOTFile.offset[2];

            if (filesize > 0)
            {
                sceIoLseek32(fd, EBOOTFile.offset[2], PSP_SEEK_SET);
                icon1data = malloc(filesize);
                sceIoRead(fd, icon1data, filesize);
                icon1size = filesize;
            }

            filesize = EBOOTFile.offset[4 + 1] - EBOOTFile.offset[4];

            if (filesize > 0)
            {
                sceIoLseek32(fd, EBOOTFile.offset[4], PSP_SEEK_SET);
                pic1data = malloc(filesize);
                sceIoRead(fd, pic1data, filesize);
                pic1size = filesize;
            }

            filesize = EBOOTFile.offset[5 + 1] - EBOOTFile.offset[5];

            if (filesize > 0)
            {
                sceIoLseek32(fd, EBOOTFile.offset[5], PSP_SEEK_SET);
                snd0data = malloc(filesize);
                sceIoRead(fd, snd0data, filesize);
                snd0size = filesize;
            }

            sceIoClose(fd);
        }
        else
        {
            char fileName[512];

            unsigned char slh = ebootpath[strlen(ebootpath)-1] == '/';

            if (icon0data == NULL)
            {
                sprintf(fileName, slh ? "%sICON0.PNG" : "%s/ICON0.PNG", ebootpath);

                if (SYSTEM_FileExist(fileName))
                {
                    SceUID fd = sceIoOpen(fileName, PSP_O_RDONLY, 0777);
                    icon0size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                    sceIoLseek32(fd, 0, PSP_SEEK_SET);

                    icon0data = malloc(icon0size);
                    memset(icon0data, 0, icon0size);
                    sceIoRead(fd, icon0data, icon0size);
                    sceIoClose(fd);
                }
            }

            sprintf(fileName, slh ? "%sICON1.PMF" : "%s/ICON1.PMF", ebootpath);

            if (SYSTEM_FileExist(fileName))
            {
                SceUID fd = sceIoOpen(fileName, PSP_O_RDONLY, 0777);
                icon1size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                sceIoLseek32(fd, 0, PSP_SEEK_SET);

                icon1data = malloc(icon1size);
                sceIoRead(fd, icon1data, icon1size);
                sceIoClose(fd);
            }

            sprintf(fileName, slh ? "%sPIC1.PNG" : "%s/PIC1.PNG", ebootpath);

            if (SYSTEM_FileExist(fileName))
            {
                SceUID fd = sceIoOpen(fileName, PSP_O_RDONLY, 0777);
                pic1size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                sceIoLseek32(fd, 0, PSP_SEEK_SET);

                pic1data = malloc(pic1size);
                sceIoRead(fd, pic1data, pic1size);
                sceIoClose(fd);
            }

            sprintf(fileName, slh ? "%sSND0.AT3" : "%s/SND0.AT3", ebootpath);

            if (SYSTEM_FileExist(fileName))
            {
                SceUID fd = sceIoOpen(fileName, PSP_O_RDONLY, 0777);
                snd0size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                sceIoLseek32(fd, 0, PSP_SEEK_SET);

                snd0data = malloc(snd0size);
                sceIoRead(fd, snd0data, snd0size);
                sceIoClose(fd);
            }

            sprintf(fileName, slh ? "%sSAVE.PNG" : "%s/SAVE.PNG", ebootpath);

            if (SYSTEM_FileExist(fileName))
            {
                SceUID fd = sceIoOpen(fileName, PSP_O_RDONLY, 0777);
                clean0size = sceIoLseek32(fd, 0, PSP_SEEK_END);
                sceIoLseek32(fd, 0, PSP_SEEK_SET);

                clean0data = malloc(clean0size);
                sceIoRead(fd, clean0data, clean0size);
                sceIoClose(fd);
            }
        }
    
        LPYTSaveDataParams->icon1FileData.buf = icon1data;
        LPYTSaveDataParams->icon1FileData.bufSize = icon1size;
        LPYTSaveDataParams->icon1FileData.size = icon1size;

        LPYTSaveDataParams->pic1FileData.buf = pic1data;
        LPYTSaveDataParams->pic1FileData.bufSize = pic1size;
        LPYTSaveDataParams->pic1FileData.size = pic1size;

        LPYTSaveDataParams->icon0FileData.buf = icon0data;
        LPYTSaveDataParams->icon0FileData.bufSize = icon0size;
        LPYTSaveDataParams->icon0FileData.size = icon0size;

        LPYTSaveDataParams->snd0FileData.buf = snd0data;
        LPYTSaveDataParams->snd0FileData.bufSize = snd0size;
        LPYTSaveDataParams->snd0FileData.size = snd0size;
        
        luaData.title = titleshow;
        luaData.icon0.buf = clean0data;
        luaData.icon0.bufSize = clean0size;
        luaData.icon0.size = clean0size;

        LPYTSaveDataParams->newData = &luaData;
    }
}

static char* LPYTSaveDataData = NULL;
static char* LPYTSaveDataNum = NULL;

g2dImage* capture_screenshot(int screen_width, int screen_height)
{
    g2dImage *tex = _g2dTexCreate(screen_width, screen_height, true);

    int orig_width = g2d_disp_buffer.w;  
    int orig_height = g2d_disp_buffer.h;
    
    int x, y;
    for (y = 0; y < screen_height; y++)
    {
        int orig_y = (y * orig_height) / screen_height;

        g2dColor *row = &g2d_disp_buffer.data[orig_y * 512];

        for (x = 0; x < screen_width; x++)
        {
            int orig_x = (x * orig_width) / screen_width;

            uint32_t original_color = row[orig_x]; 

            uint8_t r = G2D_GET_R(original_color);
            uint8_t g = G2D_GET_G(original_color);
            uint8_t b = G2D_GET_B(original_color);
            
            uint32_t rgba_color = (r) | (g << 8) | (b << 16) | 0xFF000000; 
            tex->data[x + y * tex->tw] = rgba_color;
        }
    }

    sceKernelDcacheWritebackAll();
    
    printf("Data saving completed.\n");

    return tex;
}

int SaveDataDialogUpdate(int mode, void *saveData, const char *subTitle, const char* description, const char* key, unsigned int datasize, const char *ebootpath, const char *icon0path, bool cleanBG, char* bgpath, char* AutoSaveFolder)
{
    SceUtilitySavedataParam LPYTSavedataParam;
    
    g2dImage *bg = NULL;
    
    if (cleanBG)
    {
        bg = capture_screenshot(480, 272);
    }
    else if (bgpath)
        bg = g2dTexLoad(bgpath, NULL, 0, G2D_VOID);

    SaveData_Init(&LPYTSavedataParam, mode, saveData, subTitle, description, key, datasize, ebootpath, icon0path, AutoSaveFolder);

    sceUtilitySavedataInitStart(&LPYTSavedataParam);

    int done = 0;
    int result = -1;
    
    while (!done)
    {
        g2dClear(BLACK);
        
        if (bg != NULL)
        {
            g2dBeginRects(bg);
            g2dAdd();
            g2dEnd();
        }
            
        sceGuFinish();
        sceGuSync(0, 0);
        
        switch(sceUtilitySavedataGetStatus())
        {
            case PSP_UTILITY_DIALOG_NONE:
                done = 1;
                break;
            
            case PSP_UTILITY_DIALOG_VISIBLE:
                sceUtilitySavedataUpdate(1);
                break;
            
            case PSP_UTILITY_DIALOG_QUIT:
                sceUtilitySavedataShutdownStart();

                if (LPYTSavedataParam.icon0FileData.buf != NULL)
                {
                    free(LPYTSavedataParam.icon0FileData.buf);
                    LPYTSavedataParam.icon0FileData.buf = NULL;
                }

                if (LPYTSavedataParam.pic1FileData.buf != NULL)
                {
                    free(LPYTSavedataParam.pic1FileData.buf);
                    LPYTSavedataParam.pic1FileData.buf = NULL;
                }

                if (LPYTSavedataParam.icon1FileData.buf != NULL)
                {    
                    free(LPYTSavedataParam.icon1FileData.buf);
                    LPYTSavedataParam.icon1FileData.buf = NULL;
                }
                
                if (LPYTSavedataParam.snd0FileData.buf != NULL)
                {
                    free(LPYTSavedataParam.snd0FileData.buf);
                    LPYTSavedataParam.snd0FileData.buf = NULL;
                }

                if (LPYTSavedataParam.base.result == 0)
                {
                    if (mode == PSP_UTILITY_SAVEDATA_LISTSAVE || mode == PSP_UTILITY_SAVEDATA_LISTLOAD)
                        memcpy(LPYTSaveDataNum, LPYTSavedataParam.saveName, 512);

                    if (mode == PSP_UTILITY_SAVEDATA_LISTLOAD || mode == PSP_UTILITY_SAVEDATA_AUTOLOAD)
                        memcpy(LPYTSaveDataData, LPYTSavedataParam.dataBuf, 512);
                }

                if (LPYTSavedataParam.dataBuf != NULL)
                {
                    free(LPYTSavedataParam.dataBuf);
                    LPYTSavedataParam.dataBuf = NULL;
                }

                if (luaData.icon0.buf != NULL)
                {
                    free(luaData.icon0.buf);
                    luaData.icon0.buf = NULL;
                }

                result = LPYTSavedataParam.base.result;
                break;
            default:
                break;
        }

        g2dFlip(G2D_SWIZZLE);
    }

    if (bg != NULL)
        g2dTexFree(&bg);

    return result;
}

static int SYSTEM_SaveData(lua_State *L)
{
    if(lua_gettop(L) < 4 || lua_gettop(L) > 7)
        return luaL_error(L, "System.SaveData(saveData, subTitle, description, ebootpath, [icon0path], [cleanBG], [bgpath]) takes 4, 5, 6 or 7 arguments");

    const char *saveData = luaL_checkstring(L, 1);
    const char *subTitle = luaL_checkstring(L, 2);
    const char *description = luaL_checkstring(L, 3);
    const char *ebootpath = luaL_checkstring(L, 4);
    void *icon0path = (lua_gettop(L) >= 5 && !lua_isnil(L, 5)) ? (char*)luaL_checkstring(L, 5) : NULL;
    bool cleanBG = lua_toboolean(L, 6);
    char *bgpath = (lua_gettop(L) == 7) ? (char*)luaL_checkstring(L, 7) : NULL;
    
    if (cleanBG && bgpath)
        return luaL_error(L, "System.SaveData(): you can't use cleanBG and bgpath at the same time");

    if (LPYTSaveDataNum)
        free(LPYTSaveDataNum);

    LPYTSaveDataNum = (char*)malloc(512);

    if (!LPYTSaveDataNum)   
        return luaL_error(L, "System.SaveData() internal error");

    if (SaveDataDialogUpdate(PSP_UTILITY_SAVEDATA_LISTSAVE, (void*)saveData, subTitle, description, key, SaveDataBuff, ebootpath, icon0path, cleanBG, bgpath, NULL) == 0)
    {
        lua_pushstring(L, LPYTSaveDataNum);
        free(LPYTSaveDataNum);
        LPYTSaveDataNum = NULL;
    }
    else
        lua_pushnil(L);

    return 1;
}

static int SYSTEM_LoadData(lua_State *L)
{
    int argc = lua_gettop(L);
    if(argc != 0 && argc != 1 && argc != 2)
        return luaL_error(L, "System.LoadData([cleanBG], [bgpath]) takes 0, 1 or 2 arguments");
    
    bool cleanBG = lua_toboolean(L, 1);
    char *bgpath = (argc == 2) ? (char*)luaL_checkstring(L, 2) : NULL;
    
    if (cleanBG && bgpath)
        return luaL_error(L, "System.LoadData(): you can't use cleanBG and bgpath at the same time");

    if (LPYTSaveDataNum)
        free(LPYTSaveDataNum);

    LPYTSaveDataNum = (char*)malloc(512);

    if (!LPYTSaveDataNum)   
        return luaL_error(L, "System.LoadData() internal error"); 

    if (LPYTSaveDataData)
        free(LPYTSaveDataData);

    LPYTSaveDataData = (char*)malloc(512);

    if (!LPYTSaveDataData)
        return luaL_error(L, "System.LoadData() internal error");

    if (SaveDataDialogUpdate(PSP_UTILITY_SAVEDATA_LISTLOAD, NULL, NULL, NULL, key, SaveDataBuff, NULL, NULL, cleanBG, bgpath, NULL) == 0)
    {   
        lua_newtable(L);
        lua_pushstring(L, "id");
        lua_pushstring(L, LPYTSaveDataNum);
        lua_settable(L, -3);

        lua_pushstring(L, "data");
        lua_pushstring(L, LPYTSaveDataData);
        lua_settable(L, -3);

        free(LPYTSaveDataNum);
        LPYTSaveDataNum = NULL;
        free(LPYTSaveDataData);
        LPYTSaveDataData = NULL;
    }
    else
        lua_pushnil(L);

    return 1;
}

static int SYSTEM_DeleteData(lua_State *L)
{
    int argc = lua_gettop(L);
    if(argc != 0 && argc != 1 && argc != 2)
        return luaL_error(L, "System.DeleteData([cleanBG], [bgpath]) takes 0, 1 or 2 arguments");

    bool cleanBG = lua_toboolean(L, 1);
    char *bgpath = (argc == 2) ? (char*)luaL_checkstring(L, 2) : NULL;

    if (cleanBG && bgpath)
        return luaL_error(L, "System.DeleteData(): you can't use cleanBG and bgpath at the same time");

    SaveDataDialogUpdate(PSP_UTILITY_SAVEDATA_LISTDELETE, NULL, NULL, NULL, key, SaveDataBuff, NULL, NULL, cleanBG, bgpath, NULL);
    
    return 1;
}

static int SYSTEM_AutoSave(lua_State *L)
{
    if(lua_gettop(L) < 5 || lua_gettop(L) > 6)
        return luaL_error(L, "System.AutoSave(saveData, subTitle, description, ebootpath, AutoSaveFolder, [icon0path]) takes 5 or 6 arguments");

    const char *saveData = luaL_checkstring(L, 1);
    const char *subTitle = luaL_checkstring(L, 2);
    const char *description = luaL_checkstring(L, 3);
    const char *ebootpath = luaL_checkstring(L, 4);
    const char *AutoSaveFolder = luaL_checkstring(L, 5);
    void *icon0path = (lua_gettop(L) == 6) ? (char*)luaL_checkstring(L, 6) : NULL;
    
    SaveDataDialogUpdate(PSP_UTILITY_SAVEDATA_AUTOSAVE, (void*)saveData, subTitle, description, key, SaveDataBuff, ebootpath, icon0path, TRUE, NULL, (char*)AutoSaveFolder);

    return 0;
}

static int SYSTEM_AutoLoad(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "System.AutoLoad(AutoSaveFolder) takes 1 argument");
    
    const char* AutoSaveFolder = luaL_checkstring(L, 1);

    if (LPYTSaveDataData)
        free(LPYTSaveDataData);

    LPYTSaveDataData = (char*)malloc(512);

    if (!LPYTSaveDataData)
        return luaL_error(L, "System.AutoLoad() internal error");

    if (SaveDataDialogUpdate(PSP_UTILITY_SAVEDATA_AUTOLOAD, NULL, NULL, NULL, key, SaveDataBuff, NULL, NULL, TRUE, NULL, (char*)AutoSaveFolder) == 0)
    {   
        lua_newtable(L);
        lua_pushstring(L, "data");
        lua_pushstring(L, LPYTSaveDataData);
        lua_settable(L, -3);

        free(LPYTSaveDataData);
        LPYTSaveDataData = NULL;
    }
    else
        lua_pushnil(L);

    return 1;
}

void UniToUtf8(char* outtext, unsigned short* unicodeStr)
{
    int i = 0, j = 0;
    while (unicodeStr[i] != L'\0')
    {
        if (unicodeStr[i] <= 0x7F)
        {
            outtext[j] = (char)(unicodeStr[i] & 0xFF);
            j++;
        }
        else if (unicodeStr[i] <= 0x7FF)
        {
            outtext[j] = (char)(((unicodeStr[i] >> 6) & 0x1F) | 0xC0);
            outtext[j + 1] = (char)((unicodeStr[i] & 0x3F) | 0x80);
            j += 2;
        }
        else
        {
            outtext[j] = (char)(((unicodeStr[i] >> 12) & 0x0F) | 0xE0);
            outtext[j + 1] = (char)(((unicodeStr[i] >> 6) & 0x3F) | 0x80);
            outtext[j + 2] = (char)((unicodeStr[i] & 0x3F) | 0x80);
            j += 3;
        }
        i++;
    }
    outtext[j] = '\0';
}

void Utf8ToUni(unsigned short* unicodeStr, char* utf8Str) {
    int i = 0, j = 0;
    while (utf8Str[i] != '\0')
    {
        if ((utf8Str[i] & 0x80) == 0x00)
        {
            unicodeStr[j] = utf8Str[i];
            j++;
        }
        else if ((utf8Str[i] & 0xE0) == 0xC0)
        {
            unicodeStr[j] = ((utf8Str[i] & 0x1F) << 6) | (utf8Str[i + 1] & 0x3F);
            j++;
        }
        else if ((utf8Str[i] & 0xF0) == 0xE0)
        {
            unicodeStr[j] = ((utf8Str[i] & 0x0F) << 12) | ((utf8Str[i + 1] & 0x3F) << 6) | (utf8Str[i + 2] & 0x3F);
            j++;
        }
        i++;
    }
    unicodeStr[j] = L'\0';
}

void createOsk(SceUtilityOskData *data, unsigned short *input, unsigned short *desc)
{		
	unsigned short outtext[512] = { 0 };
	memset(data, 0, sizeof(SceUtilityOskData));
	data->language = PSP_UTILITY_OSK_LANGUAGE_DEFAULT;
	data->lines = 1;
	data->unk_24 = 1;
	data->inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL;
	data->desc = desc;
	data->intext = input;
	data->outtextlength = maxLen;
	data->outtextlimit = 64;
	data->outtext = outtext;
	
	SceUtilityOskParams params;
	memset(&params, 0, sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
	params.base.graphicsThread = 17;
	params.base.accessThread = 19;
	params.base.fontThread = 18;
	params.base.soundThread = 16;
	params.datacount = 1;
	params.data = data;
	
	
	sceUtilityOskInitStart(&params);
}

static int SYSTEM_OSK(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc < 2 || argc > 4)
        return luaL_error(L, "System.OSK(text, desc, [cleanBG], [bgpath]): takes 2, 3 or 4 arguments");
	
	unsigned short* intext = memalign(16, sizeof(unsigned short)*(strlen(luaL_checkstring(L, 1))+1));
	Utf8ToUni(intext, (char*)luaL_checkstring(L, 1));
	unsigned short* desc = memalign(16, sizeof(unsigned short)*(strlen(luaL_checkstring(L, 2))+1));
	Utf8ToUni(desc, (char*)luaL_checkstring(L, 2));
    bool cleanBG = lua_toboolean(L, 3);
    char *bgpath = (lua_gettop(L) == 4) ? (char*)luaL_checkstring(L, 4) : NULL;
	
    if (cleanBG && bgpath)
        return luaL_error(L, "System.OSK(): you can't use cleanBG and bgpath at the same time");

	int done = 0;
	
	g2dImage *bg = NULL;
    
    if (cleanBG)
    {
        bg = capture_screenshot(480, 272);
    }
    else if (bgpath)
        bg = g2dTexLoad(bgpath, NULL, 0, G2D_VOID);

    SceUtilityOskData data;
	createOsk(&data, intext, desc);
    
    while(!done)
	{
		g2dClear(BLACK);

        if (bg != NULL)
        {
            g2dBeginRects(bg);
            g2dAdd();
            g2dEnd();
        }

		sceGuFinish();
		sceGuSync(0,0);
		
		switch(sceUtilityOskGetStatus())
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
				
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;
				
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
				
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				
			default :
				break;
		}
		
		g2dFlip(G2D_VSYNC);
	}
	
    if (bg != NULL)
        g2dTexFree(&bg);   

	char *outtext = (char*) memalign(16, data.outtextlength*sizeof(char)+1);
	UniToUtf8(outtext, data.outtext);

	lua_pushstring(L, outtext);
	
	free(desc);
	free(outtext);
	free(intext);
	
	return 1;
}

pspUtilityMsgDialogParams dialog;

void configureDialog(pspUtilityMsgDialogParams *dialog, size_t dialog_size)
{
    memset(dialog, 0, dialog_size);
	
    dialog->base.size = dialog_size;
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE,
								&dialog->base.language); // Prompt language
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN,
								&dialog->base.buttonSwap); // X/O button swap
	
    dialog->base.graphicsThread = 0x11;
    dialog->base.accessThread = 0x13;
    dialog->base.fontThread = 0x12;
    dialog->base.soundThread = 0x10;
}

void messageCreate(pspUtilityMsgDialogParams *dialog, const char* message, int mode, int opts)
{
	configureDialog(dialog, sizeof(pspUtilityMsgDialogParams));
    dialog->mode = mode;
	dialog->options = opts;
	
    strcpy(dialog->message, message);
    sceUtilityMsgDialogInitStart(dialog);
}

void showMessage(pspUtilityMsgDialogParams *dialog, bool cleanBG, char* bgpath)
{	
    sceUtilityMsgDialogInitStart(dialog);
	
    int done = 0;
	
    g2dImage *bg = NULL;

    if (cleanBG)
    {
        bg = capture_screenshot(480, 272);
    }
    else if (bgpath)
        bg = g2dTexLoad(bgpath, NULL, 0, G2D_VOID);

    while (!done) 
	{
		g2dClear(BLACK);
		
        if (bg != NULL)
        {
            g2dBeginRects(bg);
            g2dAdd();
            g2dEnd();
        }

        sceGuFinish();
        sceGuSync(0, 0);

		switch(sceUtilityMsgDialogGetStatus()) 
		{				
			case 2:
				sceUtilityMsgDialogUpdate(1);
				break;
				
			case 3:
				sceUtilityMsgDialogShutdownStart();
				break;
				
			case 0:
				done = 1;
				break;;	
		}

        g2dFlip(G2D_VSYNC);
    }

    if (bg != NULL)
        g2dTexFree(&bg);
}

int SYSTEM_message(const char* str, int mode, bool cleanBG, char* bgpath, lua_State *L)
{
    if (cleanBG && bgpath)
        return luaL_error(L, "System.message(): you can't use cleanBG and bgpath at the same time");

    int options = PSP_UTILITY_MSGDIALOG_OPTION_TEXT;
	
	if(mode)
		options |= PSP_UTILITY_MSGDIALOG_OPTION_YESNO_BUTTONS;	
	
	messageCreate(&dialog, str, PSP_UTILITY_MSGDIALOG_MODE_TEXT, options);	
	
	strcpy(dialog.message, str);
	
    showMessage(&dialog, cleanBG, bgpath);

    return 0;
}

static int LUA_SYSTEM_MESSAGE(lua_State *L)
{
    int argc = lua_gettop(L);
	if (argc < 1 || argc > 4)
        return luaL_error(L, "System.message(message, [mode], [cleanBG], [bgpath]): takes 1, 2, 3 or 4 arguments.");
	
    const char *str = luaL_checkstring(L, 1);
    bool mode = lua_toboolean(L, 2); 
    bool cleanBG = lua_toboolean(L, 3);
    char *bgpath = (lua_gettop(L) == 4) ? (char*)luaL_checkstring(L, 4) : NULL;

    SYSTEM_message(str, mode, cleanBG, bgpath, L);

    return 0;
}

static int SYSTEM_buttonPressed(lua_State *L)
{	
	if (dialog.buttonPressed == PSP_UTILITY_MSGDIALOG_RESULT_YES)
        lua_pushstring(L, "Yes");
	
    else if(dialog.buttonPressed == PSP_UTILITY_MSGDIALOG_RESULT_NO)
        lua_pushstring(L, "No");
	
    else
        lua_pushstring(L, "Back");
	
	return 1;
}

static int SYSTEM_getNickname(lua_State *L)
{	
	char nick[256];
	
	sceUtilityGetSystemParamString(PSP_SYSTEMPARAM_ID_STRING_NICKNAME, nick, 256);
	
	lua_pushstring(L, nick);
	
	return 1;
}

char *System_langs [12] = {
	"Jap",
	"Eng",
	"Fr",
	"Span",
	"Ger",
	"It",
	"Dut",
	"Por",
	"Rus",
	"Kor",
	"ChTrad",
	"ChSimpl"
};

static int SYSTEM_getLang(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "System.getLang() takes 0 arguments");

    int lang;

    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &lang);

    lua_pushstring(L, System_langs[lang]);

    return 1;
}

int countLinesInFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return -1;

    int count = 0;
    int prev = '\0';
    int c;

    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n' || c == '\r')
        {
            count++;

            if (c == '\r')
            {
                char next = fgetc(file);
                if (next != '\n') 
                    ungetc(next, file);
            }
        }
        prev = c;
    }

    if (prev != '\n' && prev != '\r')
        count++;

    fclose(file);
    return count;
}

char* dynamicReadLine(FILE *file)
{
    char *line = NULL;
    int len = 0;
    int capacity = 16;
    char c;

    line = (char*)malloc(capacity * sizeof(char));

    if (line == NULL)
        return NULL;

    while (1)
    {
        c = fgetc(file);

        if (c == EOF || c == '\n' || c == '\r')
        {
            if (c == '\r')
            {
                char next = fgetc(file);
                if (next != '\n')
                    ungetc(next, file);
            }
            break;
        }

        if (len == capacity - 1)
        {
            capacity *= 2;

            if (c == '\r')
            {
                char next = fgetc(file);
                if (next != '\n')
                    ungetc(next, file);
            }

            char *temp = (char*)realloc(line, capacity * sizeof(char));

            if (temp == NULL)
            {
                free(line);
                return NULL;
            }

            line = temp;
        }

        line[len++] = c;
    }

    char *resized_line = (char*)realloc(line, (len + 1) * sizeof(char));
    
    if (resized_line == NULL)
    {
        free(line);
        return NULL;
    }

    resized_line[len] = '\0';
    return resized_line;
}

char** readFileLines(char *filename, int linesCount)
{
    FILE *file = fopen(filename, "r");
    char **lines = (char**)malloc(sizeof(char*) * linesCount+1);
    int i;

    if (file == NULL) 
        return NULL;

    lines[0] = "PLACEHOLDER";

    for (i = 1; i <= linesCount; i++)
        lines[i] = NULL;

    for (i = 1; i <= linesCount; i++)
    {
        lines[i] = dynamicReadLine(file);
        if (lines[i] == NULL)
            break;
    }

    fclose(file);
    return lines;
}

static int SYSTEM_dumpFileCreate(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "System.fileDumpCreate(file) takes 1 argument");

    const char *filename = lua_tostring(L, 1);

    int linesCount = countLinesInFile((char*)filename);
    char **fileLines = readFileLines((char*)filename, linesCount);

    lua_newtable(L);
    char ***ptr = (char***)lua_newuserdata(L, sizeof(char***));
    *ptr = fileLines;
    lua_setfield(L, -2, "pointer");

    lua_pushnumber(L, linesCount);
    lua_setfield(L, -2, "linesCount");

    return 1;
}

static int SYSTEM_dumpFileRemove(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "System.fileDumpRemove(filePointer, linesCount) takes 2 arguments");

    char ***ptr = (char***)lua_touserdata(L, 1);
    char **fileLines = *ptr;
    int i;

    int linesCount = luaL_checknumber(L, 2);

    fileLines[0] = NULL;

    for (i = 1; i <= linesCount; i++)
    {
        if (fileLines[i])
        {
            free(fileLines[i]);
            fileLines[i] = NULL; 
        }
    }

    free(fileLines);
    *ptr = NULL;

    lua_pop(L, 1);
    return 0;
}

static int SYSTEM_dumpFileGetLine(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "System.fileDumpGetLine(filePointer, pos) takes 2 arguments");

    char ***ptr = (char***)lua_touserdata(L, 1); 
    char **fileLines = *ptr;
    int index = luaL_checknumber(L, 2);

    if (fileLines[index])
        lua_pushstring(L, fileLines[index]); 
    else
        lua_pushnil(L);

    return 1;
}

static int SYSTEM_PowerTick(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "System.PowerTick() takes 0 arguments");

    scePowerTick(PSP_POWER_TICK_ALL);

    return 0;
}

static pspUtilityNetconfData netConfData;

static int SYSTEM_Net(lua_State *L)
{
    memset(&netConfData, 0, sizeof(netConfData));
    netConfData.base.size = sizeof(netConfData);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &netConfData.base.language);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &netConfData.base.buttonSwap);

    netConfData.base.graphicsThread = 17;
    netConfData.base.accessThread = 19;
    netConfData.base.fontThread = 18;
    netConfData.base.soundThread = 16;
    netConfData.action = 0;

    struct pspUtilityNetconfAdhoc adhocparam;
    memset(&adhocparam, 0, sizeof(adhocparam));
    netConfData.adhocparam = &adhocparam;

    sceUtilityNetconfInitStart(&netConfData);
    
    int done = 0;

    while (!done)
    {
        g2dClear(BLACK);

        sceGuFinish();
        sceGuSync(0, 0);

        switch(sceUtilityNetconfGetStatus())
        {
        case PSP_UTILITY_DIALOG_NONE :
            break;
        case PSP_UTILITY_DIALOG_VISIBLE :
            sceUtilityNetconfUpdate(1);
            break;
        case PSP_UTILITY_DIALOG_QUIT :
            sceUtilityNetconfShutdownStart();
            break;
        case PSP_UTILITY_DIALOG_FINISHED :
            break;
        default:
            break;
        }

        g2dFlip(G2D_VSYNC);
    }

    return 1;
}

static const luaL_Reg SYSTEM_methods[] = {
    {"rename",             SYSTEM_Rename},
    {"removeFile",         SYSTEM_removeFile},
    {"removeDir",          SYSTEM_removeDir},
    {"createDir",          SYSTEM_createDir},
    {"getBatteryPercent",  SYSTEM_getBattPerc},
    {"getBatteryLifeTime", SYSTEM_getBattLifeTime},
    {"getCPU",             SYSTEM_getCPU},
    {"getModel",           SYSTEM_getModel},
    {"getTime",            SYSTEM_getTime},
    {"getOSV",             SYSTEM_getCFWVersion},
    {"getGameID",          SYSTEM_GetGAMEID},
    {"GC",                 SYSTEM_memclean},
    {"currentDir",         SYSTEM_curDir},
    {"listDir",            SYSTEM_listDir},
    {"isFile",             SYSTEM_isFile},
    {"isDir",              SYSTEM_isDir},
    //{"HighCPU",            SYSTEM_HighCPU},
    //{"MedCPU",             SYSTEM_MedCPU},
    //{"LowCPU",             SYSTEM_LowCPU},
    {"SaveData",           SYSTEM_SaveData},
    {"LoadData",           SYSTEM_LoadData},
    {"DeleteData",         SYSTEM_DeleteData},
    {"AutoSave",           SYSTEM_AutoSave},
    {"AutoLoad",           SYSTEM_AutoLoad},
    {"OSK",                SYSTEM_OSK},
    {"fileDumpCreate",     SYSTEM_dumpFileCreate},
    {"fileDumpRemove",     SYSTEM_dumpFileRemove},
    {"fileDumpGetLine",    SYSTEM_dumpFileGetLine},
    {"message",            LUA_SYSTEM_MESSAGE},
    {"buttonPressed",      SYSTEM_buttonPressed},
    {"getNickname",        SYSTEM_getNickname},
    {"getLang",            SYSTEM_getLang},
    {"PowerTick",          SYSTEM_PowerTick},
    {"Net",                SYSTEM_Net},
    {0, 0}
};

int SYSTEM_init(lua_State *L)
{
    luaL_register(L, "System", SYSTEM_methods);

    return 1;
}