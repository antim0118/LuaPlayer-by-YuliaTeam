/*--------------------------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#------------- This file is part of: ---------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#----------  ________      ___  -------------------------------  ___  ------------------------------------------------------------------------#
#---------- |         \   |   | ------------------------------- |   | ------------------------------------------------------------------------#
#---------- |______    \  |   | ------------------------------- |   | ------------------------------------------------------------------------#
#----------         \   \ |   | ------------------------------- |   | ------------------------------------------------------------------------#
#----------  ______ /   / |   |   ___     ___     _________     |   |           _________   ___     ___   ___________   __________  ----------#
#---------- |          /  |   |  |   |   |   |   /         |    |   |          /         | |   |   |   | |    ____   | |           \ ---------#
#---------- |    ____ /   |   |  |   |   |   |  /    __    |    |   |         /    __    | |   |   |   | |   |____|  | |    ____   / ---------#
#---------- |   |         |   |  |   |   |   | /    /  |   |    |   |        /    /  |   | |   |   |   | |    _______| |   |    \_/ ----------#
#---------- |   |         |   |  |   |___|   | \    \  |   |    |   |_______ \    \  |   | |   |___|   | |   |_______  |   |        ----------#
#---------- |   |         |   |   \          |  \    | |   |    |           | \    | |   |  \          | |           \ |   |        ----------#
#---------- |___|         |___|    \_________|   \___| |___|    |___________|  \___| |___|   \_____    | |___________/ |___|        ----------#
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
#------------------------------------------------------------------------------------------ Made by илья xdddd -------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#- Special Thanks to: ------------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#- BenHur for intraFont ----------------------------------------------------------------------------------------------------------------------#
#- Geecko for gLib2D -------------------------------------------------------------------------------------------------------------------------#
#- Arshia001 for PSPAALIB --------------------------------------------------------------------------------------------------------------------#
#- jonny & Raphael for PMP Mod ---------------------------------------------------------------------------------------------------------------#
#- InsertWittyName & MK2k for PGE source code ------------------------------------------------------------------------------------------------#
#- Rinnegatamante & Nanni for example of awesome Lua Player ----------------------------------------------------------------------------------#
#- Andrei - govna popei ----------------------------------------------------------------------------------------------------------------------#
#---------------------------------------------------------------------------------------------------------------------------------------------#
#--------------------------------------------------------------------------------------------------------------------------------------------*/

#include "libs/callbacks.h"

#include "lua/LUA.h"
//#include "lua/asyncCycle.h"
#include "lua/audio.h"
#include "lua/ctrl.h"
#include "lua/graphics.h"
#include "lua/pmp.h"
//#include "lua/particles.h"
#include "lua/system.h"
#include "lua/timer.h"
#include "lua/usb.h"
#include "lua/vfpu_math.h"
#include "lua/lgn.h"
#include "lua/batch.h"
#include "lua/gameobject.h"
#include "lua/gamemaker.h"

#define SIMPLE_ERROR_MODE 1

#if SIMPLE_ERROR_MODE <= 0
// #include "libs/include_res/output_png.c"
#include "libs/include_res/oksiminog.c"
#include "libs/include_res/error_placeholder.c"
#endif

#define LPYT_MAJOR 0
#define LPYT_MINOR 5
PSP_MODULE_INFO("LuaPlayerYT", 0, LPYT_MAJOR, LPYT_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
int sce_newlib_heap_kb_size = (-1024);

extern const unsigned short _ctype_b[];
const unsigned short *__ctype_ptr__ = _ctype_b;

typedef struct
{
    char main_script[256];
    int debug_intro;
    int razgon_bilya;
} LPYT_Config;

void trim(char *str) {
    if (!str || !*str) return;

    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;

    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

int ReadConfig(const char *filename, LPYT_Config *config) {
    FILE *file = fopen(filename, "r");
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
            char *separator = strchr(line, '=');
            if (separator) {
                *separator = '\0';
                char *key = line;
                char *value = separator + 1;

                // Удаляем пробелы вокруг ключа и значения
                trim(key);
                trim(value);

                // Заполняем конфиг
                if (strcmp(key, "MAIN_SCRIPT") == 0) {
                    strncpy(config->main_script, value, sizeof(config->main_script) - 1);
                } else if (strcmp(key, "DEBUG_INTRO") == 0) {
                    config->debug_intro = atoi(value);
                } else if (strcmp(key, "RAZGON_BILYA") == 0) {
                    config->razgon_bilya = atoi(value);
                }
            }
        }
    }

    fclose(file);
    return 1;
}

void initEngine(lua_State *L) {
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
    LGN_init(L);
    BATCH_init(L);
    GAMEOBJECT_init(L);
    GAMEMAKER_init(L);
    TIMER_init(L);
    USB_init(L);
    VFPU_init(L);
}

void showError(lua_State *L, char *error) {
    bool USB_ACTIVE = FALSE;

    g2dSetCameraXY(0, 0);
    intraFontSetStyle(luaFont, 1.f, BLACK, 0, 0.f, 0);

    //chdir("/");
    //save_to_file("temp_error_bg.png", error_data, size_error_data);
    //g2dImage *ERR = g2dTexLoad("temp_error_bg.png", G2D_VOID);
    //remove("temp_error_bg.png");
#if SIMPLE_ERROR_MODE <= 0
    g2dImage *OKSI = g2dTexLoad(NULL, oksiminog, size_oksiminog, G2D_VOID);
    g2dImage *ERR = g2dTexLoad(NULL, error_placeholder, size_error_placeholder, G2D_VOID);
    int frame = 0; // 0-100

    AalibPlay(PSPAALIB_CHANNEL_WAV_32);
#endif

    while (1) {
        controls_read();

        if (controls_pressed(PSP_CTRL_CROSS))
            return;

        if (controls_pressed(PSP_CTRL_TRIANGLE)) {
            if (USB_ACTIVE)
                USB_deactivate();
            else
                USB_activate();

            USB_ACTIVE = !USB_ACTIVE;
        }

#if SIMPLE_ERROR_MODE <= 0
        float rot = 100 - frame;
        float scale = frame / 100.0f;

        float x = 480.0f / 2.0f;
        float y = 272.0f / 2.0f - (rot / 2.0f);

        int alpha = frame * 4;
        if (alpha > 255) alpha = 255;
#endif

        g2dClear(WHITE);

#if SIMPLE_ERROR_MODE <= 0
        g2dBeginRects(OKSI);
        g2dAdd();
        g2dEnd();

        g2dBeginRects(ERR);
        g2dSetOriginXY(0.5f, 0.5f);
        g2dSetAlpha(alpha);
        g2dSetCoordXY(x, y);
        g2dSetScale(scale, scale);
        g2dSetRotation(rot * 5);
        g2dAdd();
        g2dEnd();
#endif

#if SIMPLE_ERROR_MODE <= 0
        intraFontSetStyle(luaFont, scale, G2D_RGBA(0, 0, 0, alpha), 0, rot, INTRAFONT_ALIGN_LEFT);
        intraFontActivate(luaFont, true);
        intraFontPrintColumn(luaFont, x - 100, y - 65, 300, error);
#else
        intraFontActivate(luaFont, true);
        intraFontPrintColumn(luaFont, 20, 10 + intraFontTextHeight(luaFont), 480 - 50 * 2, error);
#endif

        //printf("freeRam: %d\n", get_freeRam());
        //intraFontPrintColumn(luaFont,230,94,220,error);

        g2dFlip(G2D_VSYNC);

#if SIMPLE_ERROR_MODE <= 0
        if (frame < 100)
            frame += 3.0f;
        else
            frame = 100;
#endif
    }
    lua_pop(L, 1);
}

#define START_TIMER(TIMERNAME)      clock_t TIMERNAME = clock();
#define STOP_TIMER(TIMERNAME)       printf("[TIMER:" #TIMERNAME "] %.2f sec.\n", (float)(clock() - TIMERNAME) / CLOCKS_PER_SEC);
// #define STOP_TIMER(TIMERNAME)   clock_t startTime_TIMERNAME = clock();

int main() {
    START_TIMER(LPYT_InitTime);

    LPYT_Config config;

    strncpy(config.main_script, "script.lua", sizeof(config.main_script) - 1);

    //if (!ReadConfig("lpyt.ini", &config))
    //    sceKernelExitGame();

    SetupCallbacks();

    lua_State *L = lua_open();

    luaL_openlibs(L);
    initEngine(L);

    SYSTEM_getTITLEID("EBOOT.PBP", LPYTGameTitle, LPYTGameID);

    if (scePowerGetCpuClockFrequency() != 333)
        scePowerSetClockFrequency(333, 333, 166);

    FILE *f = fopen(config.main_script, "r");
    if (!f) sceKernelExitGame();
    fclose(f);

    STOP_TIMER(LPYT_InitTime);

    if (luaL_loadfile(L, config.main_script) == 0) {
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) { //error
            FILE *f = fopen("error_log.txt", "a");
            fprintf(f, "%s\n", lua_tostring(L, -1));
            fclose(f);

            char error[255];
            sprintf(error, "%s.", lua_tostring(L, -1));

            showError(L, error);
        }
    }

    lua_close(L);
    LPYT_FastFinish();
    return 0;
}