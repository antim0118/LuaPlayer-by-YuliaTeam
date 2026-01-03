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

#include "libs/callbacks.h"
#include "libs/include_res/output_png.c"

#include "lua/LUA.h"
//#include "lua/asyncCycle.h"
#include "lua/audio.h"
#include "lua/ctrl.h"
#include "lua/graphics.h"
//#include "lua/particles.h"
#include "lua/system.h"
#include "lua/timer.h"
#include "lua/usb.h"
#include "lua/vfpu_math.h"

PSP_MODULE_INFO("Lua Player YT", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
int sce_newlib_heap_kb_size = (-1024);

extern const unsigned short _ctype_b[];
const unsigned short *__ctype_ptr__ = _ctype_b;

typedef struct {
    char main_script[256];
    int debug_intro;
    int razgon_bilya;
} LPYT_Config;

void trim(char* str)
{
    if (!str || !*str) return;

    char* start = str;
    while(*start && isspace((unsigned char)*start)) start++;
    
    char* end = str + strlen(str) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = '\0';
    
    if(start != str) {
        memmove(str, start, end - start + 2);
    }
}

int ReadConfig(const char* filename, LPYT_Config* config) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Не удалось открыть конфиг файл: %s\n", filename);
        return 0;
    }

    // Установка значений по умолчанию
    strcpy(config->main_script, "script.lua");
    config->debug_intro = 0;
    config->razgon_bilya = 0;

    char line[256];
    int in_general_section = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Удаляем символы перевода строки
        line[strcspn(line, "\r\n")] = '\0';
        
        // Пропускаем пустые строки и комментарии
        if (line[0] == '\0' || line[0] == '#') continue;
        
        // Проверяем секции
        if (line[0] == '[') {
            in_general_section = (strcmp(line, "[GENERAL]") == 0);
            continue;
        }
        
        // Парсим только если находимся в нужной секции
        if (in_general_section) {
            char* separator = strchr(line, '=');
            if (separator) {
                *separator = '\0';
                char* key = line;
                char* value = separator + 1;
                
                // Удаляем пробелы вокруг ключа и значения
                trim(key);
                trim(value);
                
                // Заполняем конфиг
                if (strcmp(key, "MAIN_SCRIPT") == 0) {
                    strncpy(config->main_script, value, sizeof(config->main_script) - 1);
                } 
                else if (strcmp(key, "DEBUG_INTRO") == 0) {
                    config->debug_intro = atoi(value);
                }
                else if (strcmp(key, "RAZGON_BILYA") == 0) {
                    config->razgon_bilya = atoi(value);
                }
            }
        }
    }

    fclose(file);
    return 1;
}

void LPYT_FastFinish()
{
    g2dTerm();
    intraFontShutdown();
    pmp_shutdown();
    sceKernelExitGame();
}

int exit_callback(int arg1, int arg2, void *common)
{
    LPYT_FastFinish();
    
    return 0;
}

int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    
    return 0;
}

int SetupCallbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);

    return thid;
}

int GETFREEMEMORY()
{
    if(SYSTEMint_getMODEL() == 1)
        return (-1024);
    else
        return (-1024);
}

void initEngine(lua_State *L)
{
    //ASYNC_init(L);
    AUDIO_init(L);
    COLOR_init(L);
    CTRL_init(L);
    INTRAFONT_init(L);
    LUA_init(L);
    GRAPHICS_init(L);
    //PARTICLE_init(L);
    PMP_init(L);
    SYSTEM_init(L);
    TIMER_init(L);
    USB_init(L);
    VFPU_init(L);
}

int main()
{
    LPYT_Config config;

    strncpy(config.main_script, "script.lua", sizeof(config.main_script) - 1);

    //if (!ReadConfig("lpyt.ini", &config))
    //    sceKernelExitGame();

    SetupCallbacks();

    lua_State *L = lua_open();
    
    luaL_openlibs(L);
    initEngine(L);
    
    SYSTEM_getTITLEID("EBOOT.PBP", LPYTGameTitle, LPYTGameID);
    
    if(scePowerGetCpuClockFrequency() != 333)
        scePowerSetClockFrequency(333, 333, 166);
    
    FILE* f = fopen(config.main_script, "r");
    if(!f) sceKernelExitGame();
    fclose(f);

    while(1)
    {
        if(luaL_loadfile(L, config.main_script) == 0)
        {   
            if(lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
            { //error
                FILE *f = fopen("error_log.txt", "a");
                fprintf(f, "%s\n", lua_tostring(L, -1));
                fclose(f);

                char error[50];
                sprintf(error, "%s.", lua_tostring(L, -1));
                
                bool USB_ACTIVE = FALSE;

                // set font, size, color of the error 
                //intraFontSetStyle(luaFont, 1.f, BLACK, 0, 0.f, 0); <- original
                intraFontSetStyle(luaFont, 1.f, WHITE, 0, 0.f, 0);
                
                // display error-pic

                //chdir("/");
                g2dImage *ERR = g2dTexLoad(NULL, error_data, size_error_data, G2D_VOID);

                AalibPlay(PSPAALIB_CHANNEL_WAV_32);
                
                while(1)
                {
                    controls_read();  

                    if(controls_pressed(PSP_CTRL_CROSS))
                        sceKernelExitGame();

                    if(controls_pressed(PSP_CTRL_TRIANGLE))
                    {
                        if(USB_ACTIVE)
                            USB_deactivate();
                        else
                            USB_activate();
                    
                        USB_ACTIVE = !USB_ACTIVE;
                    }

                    g2dClear(G2D_RGBA(255,255,255,255));
                    
                    g2dBeginRects(ERR);
                    g2dAdd();
                    g2dEnd();

                    intraFontActivate(luaFont, true);
                    // print lua font
                    //intraFontPrintColumn(luaFont,213,94,220,error); <- original
                    intraFontPrintColumn(luaFont,180,94,220,error);

                    //printf("freeRam: %d\n", get_freeRam());
                    //intraFontPrintColumn(luaFont,230,94,220,error);
                    
                    g2dFlip(G2D_VSYNC);

                }
                lua_pop(L, 1);
            }
        }
    }
    lua_close(L);
    free(config.main_script);
    LPYT_FastFinish();
    return 0;
}