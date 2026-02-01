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

#include "pmp.h"
#include "../libs/pmp/pmp_play.h"

#define MAX_SUBS 100
#define MAX_TEXT_LENGTH 512

typedef struct {
    char startTime[15]; // Временные метки в формате "00:00:01,600"
    char endTime[15];   // Временные метки в формате "00:00:01,600"
    char text[MAX_TEXT_LENGTH]; // Текст субтитра
} Subtitle;

Subtitle subtitles[MAX_SUBS];
int subtitle_count = 0;

static char* LPYTVideoPath = NULL;
static int LPYTVideoPlay = 0;

void parseSRT(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Cannot open SRT file");
        return;
    }

    while (subtitle_count < MAX_SUBS)
    {
        Subtitle sub;
        
        if (fscanf(file, "%*d\n") != 0)
            break;
        

        if (fscanf(file, "%[^ ] --> %[^ \n]\n", sub.startTime, sub.endTime) != 2)
            break;

        
        sub.text[0] = '\0';

        char line[MAX_TEXT_LENGTH];
        while (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0)
            strncat(sub.text, line, MAX_TEXT_LENGTH - strlen(sub.text) - 1);

        if (strlen(sub.text) > 0)
            subtitles[subtitle_count++] = sub;
    }

    fclose(file);
}


void parseASS(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
	{
        perror("Cannot open ASS file");
        return;
    }

    char line[1024];
    int inDialogueSection = 0;

    while (subtitle_count < MAX_SUBS && fgets(line, sizeof(line), file))
	{
        // Найдем секцию с диалогами
        if (strncmp(line, "[Dialogue]", 10) == 0)
		{
            inDialogueSection = 1;
            continue; 
        }
        
        if (inDialogueSection)
		{
            // Параметры: Marked|Start|End|Style|Name|MarginL|MarginR|MarginV|Effect|Text
            // Разделяем строку на части
            char *token = strtok(line, ",");
            if (!token) continue;

            // Получаем временные метки
            token = strtok(NULL, ",");
            if (token) strcpy(subtitles[subtitle_count].startTime, token + 1); // Убираем [ 
            
            token = strtok(NULL, ",");
            if (token) strcpy(subtitles[subtitle_count].endTime, token + 1); // Убираем [
            
            // Пропускаем стиль, имя, маргины
			int i;
            for (i = 0; i < 5; i++)
                token = strtok(NULL, ",");

            // Получаем текст
            if (token)
			{
                strcpy(subtitles[subtitle_count].text, token + 1); // Убираем [
                subtitles[subtitle_count].text[strcspn(subtitles[subtitle_count].text, "\n")] = 0; // Убираем новую строку
            }

            subtitle_count++;
        }
    }

    fclose(file);
}

void clear_subtitles()
{
	int i;
    for (i = 0; i < MAX_SUBS; i++)
	{
        subtitles[i].startTime[0] = '\0'; // очистить временную метку начала
        subtitles[i].endTime[0] = '\0';   // очистить временную метку окончания
        subtitles[i].text[0] = '\0';       // очистить текст субтитра
    }
    subtitle_count = 0; // сбросить количество субтитров
}

void parseSubs(char* path)
{
    size_t len = strlen(path);

        if (strcmp(path + len - 4, ".srt") == 0)
			parseSRT(path);
		else if (strcmp(path + len - 4, ".ass") == 0)
			parseSRT(path);

		printf("Before clearing:\n");
    	int i;
		for (i = 0; i < subtitle_count; i++)
		{
        	printf("Subtitle %d:\n", i + 1);
        	printf("Start: %s\n", subtitles[i].startTime);
        	printf("End: %s\n", subtitles[i].endTime);
        	printf("Text: %s\n\n", subtitles[i].text);
    	}
}

static int PMP_play(lua_State *L)
{
    if (LPYTVideoPlay || pmp_isplaying())
        return 1;

    int args = lua_gettop(L);
    if(args < 1 && args > 4)
        return luaL_error(L, "PMP.play(path, getPointer, [interruptButton], [subtitlePath]) takes 1, 2, 3 or 4 arguments"); 

    
    char* path = (char*)luaL_checkstring(L, 1);
    bool getPointer = lua_toboolean(L, 2);
    PMP_INTERRUPT_BUTTON = (args >= 3 && !lua_isnil(L, 3)) ? luaL_checkint(L, 3) : 69;
    char* subtitlepath = (args == 4) ? (char*)luaL_checkstring(L, 4) : NULL; 

    if (getPointer)
    {
        if (LPYTVideoPath)
        {
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
        }

        LPYTVideoPath = (char*)malloc(512);

        if (!LPYTVideoPath)   
            return luaL_error(L, "PMP.play() internal error");

        memcpy(LPYTVideoPath, path, 512);

        g2dImage *tex = _g2dTexCreate(480, 272, true);

        g2dImage **image = pushG2D(L);

    }

    if (subtitlepath)
    {
        size_t len = strlen(subtitlepath);

        if (strcmp(subtitlepath + len - 4, ".srt") != 0 || !strcmp(subtitlepath + len - 4, ".ass") != 0)
            return luaL_error(L, "PMP.play() error: you're trying to load non-srt/ass subtitle file");

        parseSubs(subtitlepath);
    }

    char *rez = pmp_play(path, 1, GU_PSM_8888); 
    if(rez != 0)
        return luaL_error(L, "PMP.play() error: file \"%s\" doesn't exist or some internal error", path);

    while(pmp_isplaying())
    {}
    
    pmp_stop();

    return 1;
}

static int PMP_setVolume(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "PMP.setVolume(volume) takes 1 argument");

    int vol = setInterval(luaL_checknumber(L, 1), 0, 100);
    
    PMP_AUDIO_VOLUME = ceil((32768/100)*vol);

    return 1;
}

static int PMP_getLength(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getLength() takes no arguments");

    if (pmp_isplaying())
    {
        char rez[20];
        sprintf(rez, "%.2f",(PMP_NUMBER_OF_FRAMES/29.97)/60);
        lua_pushstring(L, rez);
    }
    else
        lua_pushnil(L);

    return 1;
}

static int PMP_getTimeCode(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getTimeCode() takes no arguments");

    if (pmp_isplaying())
    {
        char rez[20];
        sprintf(rez, "%.2f",(PMP_CURRENT_FRAME/29.97));
        lua_pushstring(L, rez);
    }
    else
        lua_pushnil(L);

    return 1;
}

static const luaL_Reg PMP_methods[] = {
    {"play",        PMP_play},
    {"setVolume",   PMP_setVolume},
    {"getLength",   PMP_getLength},
    {"getTimeCode", PMP_getTimeCode},
    {0, 0}
};

int VIDEO_init(lua_State *L)
{
    pmp_init();
    PMP_AUDIO_VOLUME = 32768;
    PMP_INTERRUPT_BUTTON = PSP_CTRL_START;
    PMP_GAME_EXIT = 0;

    luaL_register(L, "PMP", PMP_methods);

    return 1;
}