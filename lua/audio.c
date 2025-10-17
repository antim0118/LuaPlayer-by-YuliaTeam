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

#include "audio.h"

static int pgeMp3Vol;

struct channels
{
    char* name;
    int number;
};

struct channels audiochannels[] = {
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"MP3", PSPAALIB_CHANNEL_SCEMP3_1},
    {"NONE1", PSPAALIB_CHANNEL_NONE},
    
    {"NONE2", PSPAALIB_CHANNEL_NONE},
    {"NONE3", PSPAALIB_CHANNEL_NONE},
    
    {"AT3_1", PSPAALIB_CHANNEL_AT3_1},
    {"AT3_2", PSPAALIB_CHANNEL_AT3_2},

    /*{"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},
    {"NONE", PSPAALIB_CHANNEL_NONE},*/

    {"OGG_1", PSPAALIB_CHANNEL_OGG_1},
    {"OGG_2", PSPAALIB_CHANNEL_OGG_2},
    {"OGG_3", PSPAALIB_CHANNEL_OGG_3},
    {"OGG_4", PSPAALIB_CHANNEL_OGG_4},
    {"OGG_5", PSPAALIB_CHANNEL_OGG_5},
    {"OGG_6", PSPAALIB_CHANNEL_OGG_6},
    {"OGG_7", PSPAALIB_CHANNEL_OGG_7},
    {"OGG_8", PSPAALIB_CHANNEL_OGG_8},
    {"OGG_9", PSPAALIB_CHANNEL_OGG_9},
    {"OGG_10", PSPAALIB_CHANNEL_OGG_10},

    {"WAV_1", PSPAALIB_CHANNEL_WAV_1},
    {"WAV_2", PSPAALIB_CHANNEL_WAV_2},
    {"WAV_3", PSPAALIB_CHANNEL_WAV_3},
    {"WAV_4", PSPAALIB_CHANNEL_WAV_4},
    {"WAV_5", PSPAALIB_CHANNEL_WAV_5},
    {"WAV_6", PSPAALIB_CHANNEL_WAV_6},
    {"WAV_7", PSPAALIB_CHANNEL_WAV_7},
    {"WAV_8", PSPAALIB_CHANNEL_WAV_8},
    {"WAV_9", PSPAALIB_CHANNEL_WAV_9},
    {"WAV_10", PSPAALIB_CHANNEL_WAV_10},
    {"WAV_11", PSPAALIB_CHANNEL_WAV_11},
    {"WAV_12", PSPAALIB_CHANNEL_WAV_12},
    {"WAV_13", PSPAALIB_CHANNEL_WAV_13},
    {"WAV_14", PSPAALIB_CHANNEL_WAV_14},
    {"WAV_15", PSPAALIB_CHANNEL_WAV_15},
    {"WAV_16", PSPAALIB_CHANNEL_WAV_16},
    {"WAV_17", PSPAALIB_CHANNEL_WAV_17},
    {"WAV_18", PSPAALIB_CHANNEL_WAV_18},
    {"WAV_19", PSPAALIB_CHANNEL_WAV_19},
    {"WAV_20", PSPAALIB_CHANNEL_WAV_20},
    {"WAV_21", PSPAALIB_CHANNEL_WAV_21},
    {"WAV_22", PSPAALIB_CHANNEL_WAV_22},
    {"WAV_23", PSPAALIB_CHANNEL_WAV_23},
    {"WAV_24", PSPAALIB_CHANNEL_WAV_24},
    {"WAV_25", PSPAALIB_CHANNEL_WAV_25},
    {"WAV_26", PSPAALIB_CHANNEL_WAV_26},
    {"WAV_27", PSPAALIB_CHANNEL_WAV_27},
    {"WAV_28", PSPAALIB_CHANNEL_WAV_28},
    {"WAV_29", PSPAALIB_CHANNEL_WAV_29},
    {"WAV_30", PSPAALIB_CHANNEL_WAV_30},
    {"WAV_31", PSPAALIB_CHANNEL_WAV_31},
    {"NONE14", PSPAALIB_CHANNEL_NONE},

    {"FLAC_1", PSPAALIB_CHANNEL_FLAC_1},
    {"FLAC_2", PSPAALIB_CHANNEL_FLAC_2},
    {"FLAC_3", PSPAALIB_CHANNEL_FLAC_3},
    {"FLAC_4", PSPAALIB_CHANNEL_FLAC_4},
};

static char* LPYTMP3PATH = NULL;

static int AALIB_load(lua_State *L)
{
    int args = lua_gettop(L);

    if (args < 2 || args > 3)
        return luaL_error(L, "sound.load(path, channel, [toRam]) takes 2 or 3 arguments");

    const char* path = luaL_checkstring(L, 1);
    int channel = luaL_checknumber(L, 2);
    bool toRam = (args >= 3 && (kuKernelGetModel() + 1) > 1) ? lua_toboolean(L, 3) : FALSE;
    
    if (channel == PSPAALIB_CHANNEL_SCEMP3_1)
    {
        size_t len = strlen(path);
        if (len < 4 || strcmp(path + len - 4, ".mp3") != 0)
            return luaL_error(L, "sound.load() error: you're trying to load a non-mp3 file to a mp3 channel");

        if (LPYTMP3PATH)
        {
            free(LPYTMP3PATH);
            LPYTMP3PATH = NULL;
        }

        LPYTMP3PATH = (char*)malloc(512);

        if (!LPYTMP3PATH)
            return luaL_error(L, "sound.load() internal error");

        strcpy(LPYTMP3PATH, path);
    }
    else
    {
        size_t len = strlen(path);

        if (len < 4 || (strcmp(path + len - 4, ".wav") != 0 && (len < 4 || strcmp(path + len - 4, ".at3") != 0) && (len < 4 || strcmp(path + len - 4, ".ogg") != 0)))
            return luaL_error(L, "sound.load() error: you're trying to load a non-wav/non-at3 file to a wav/at3 channel");
        
        int rez = AalibLoad((char*)path, channel, toRam);

        if(rez != 0)
            return luaL_error(L, "sound.load() error loading \"%s\", %d", path, rez);
    }

    return 0;
}

static int AALIB_play(lua_State *L)
{
    int args = lua_gettop(L);
    
    if(args < 1 || args > 2)
        return luaL_error(L, "sound.play(channel, [loop]) takes 1 or 2 arguments");

    int channel = luaL_checkint(L, 1);
    bool loop = lua_toboolean(L, 2);
    
    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
    {
        if (LPYTMP3PATH == NULL)
            return luaL_error(L, "sound.play() channel isn't initialized");

        if(pgeMp3Play(LPYTMP3PATH) != 1)
            return luaL_error(L, "sound.play() internal error");
        
        pgeMp3Loop(loop);
    }
    else
    {
        if(AalibSetAutoloop(channel, loop) != 0)
            return luaL_error(L, "sound.play() loop error");

        if(AalibPlay(channel) != 0)
            return luaL_error(L, "sound.play() play error");
    }
    
    return 0;
}

static int AALIB_pause(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "sound.pause(channel) takes 1 argument");

    int channel = luaL_checknumber(L, 1);

    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
        pgeMp3Pause();
    else
    {
        if(AalibPause(channel) != 0)
            return luaL_error(L, "sound.pause() error");
    }
    
    return 0;
}

static int AALIB_stop(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "sound.stop(channel) takes 1 argument");

    int channel = luaL_checknumber(L, 1);

    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
        pgeMp3Stop();
    else
    {
        if(AalibStop(channel) != 0)
            return luaL_error(L, "sound.stop() error to stop \"%s\" channel", audiochannels[channel].name);
        
        if(AalibDisable(channel, PSPAALIB_EFFECT_VOLUME_MANUAL) != 0)
            return luaL_error(L, "sound.stop() volume error");
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    
    return 0;
}

static int AALIB_unload(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "sound.unload(channel) takes 1 argument");

    int channel = luaL_checknumber(L, 1);

    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
    {
        if (LPYTMP3PATH)
        {
            pgeMp3Stop();
            free(LPYTMP3PATH);
            LPYTMP3PATH = NULL;
        }
    }
    else
    {
        if (AalibGetStatus(channel) == -3)
            AalibStop(channel);

        if(AalibUnload(channel) != 0)
            return luaL_error(L, "sound.unload() error to unload sound from \"%s\"", audiochannels[channel].name);
    } 
    
    lua_gc(L, LUA_GCCOLLECT, 0);
    
    return 0;
}

static int AALIB_volume(lua_State *L)
{
    if(lua_gettop(L) != 3)
        return luaL_error(L, "sound.volume(channel, volumeL, volumeR) takes 3 arguments");

    int channel = luaL_checknumber(L, 1);
    
    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
    {
        int volume = setInterval(luaL_checkint(L, 2), 0, 100)*250;
        pgeMp3Vol = luaL_checkint(L, 2);
        pgeMp3Volume(volume);
    }
    else
    {
        AalibVolume volume = {(float)setInterval(luaL_checkint(L,2),0,100)/100,(float)setInterval(luaL_checkint(L,3),0,100)/100};
		
        if(AalibEnable(channel, PSPAALIB_EFFECT_VOLUME_MANUAL) != 0)
            return luaL_error(L, "sound.volume() error");

        if(AalibSetVolume(channel, volume) != 0)
            return luaL_error(L, "sound.volume() error to set volume");
    }

    return 0;
}

static int AALIB_GetStatus(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "sound.state(channel) takes 1 argument");

    int channel = luaL_checknumber(L, 1);

    if(channel == PSPAALIB_CHANNEL_SCEMP3_1)
    {
        int rez = pgeMp3IsPlaying();

        lua_newtable(L);
		lua_pushstring(L, "state");
        
        if(rez == 0)
            lua_pushstring(L, "stopped");
		else
            lua_pushstring(L, "playing");
		
        lua_settable(L, -3);

        lua_pushstring(L, "volume");
        lua_pushnumber(L, pgeMp3Vol);
        lua_settable(L, -3);
    }
    else
    {
        int res = AalibGetStatus(luaL_checkint(L, 1));
		if(!res)
            return luaL_error(L, "sound.state() state error");
		
        int free = AalibIsFree(luaL_checkint(L, 1));
		if(free != 0 && free != 1)
            return luaL_error(L, "sound.state() free error");

        lua_newtable(L);
		lua_pushstring(L, "state"); 

		if(res == -1)
		    lua_pushstring(L, "stopped");
		else if(res == -2)
		    lua_pushstring(L, "paused");
		else if(res == -3)
		    lua_pushstring(L, "playing");
		
        lua_settable(L, -3);
		
		lua_pushstring(L, "free");
		if(free == 0)
		    lua_pushboolean(L, TRUE);
		else if(free == 1)
		    lua_pushboolean(L, FALSE);
        lua_settable(L, -3);

        lua_pushstring(L, "volume");
        lua_pushnumber(L, AalibGetVolume(channel).right * 100);
        lua_settable(L, -3);
    }

    return 1;
}

static int AALIB_getInfo(lua_State *L)
{
	if(lua_gettop(L) != 0)
        return luaL_error(L, "sound.info() takes no arguments");

	pgeMp3Info f;

	if(pgeMp3IsPlaying() == 1)
		f = pgeMp3GetInfo();

	const char* title = (pgeMp3IsPlaying() == 1) ? f.title : "empty";
	const char* artist = (pgeMp3IsPlaying() == 1) ? f.artist : "empty";
	const char* album = (pgeMp3IsPlaying() == 1) ? f.album : "empty";
	const char* year = (pgeMp3IsPlaying() == 1) ? f.year : "empty";
	const char* genre = (pgeMp3IsPlaying() == 1) ? f.genretext : "empty";
	const char* id3v = (pgeMp3IsPlaying() == 1) ? f.id3version : "empty";
	float track = (pgeMp3IsPlaying() == 1) ? f.track : 0;

	lua_newtable(L);
	lua_pushstring(L, "title"); lua_pushstring(L, title); lua_settable(L, -3);
	lua_pushstring(L, "artist"); lua_pushstring(L, artist); lua_settable(L, -3);
	lua_pushstring(L, "album"); lua_pushstring(L, album); lua_settable(L, -3);
	lua_pushstring(L, "year"); lua_pushstring(L, year); lua_settable(L, -3);
	lua_pushstring(L, "genre"); lua_pushstring(L, genre); lua_settable(L, -3);
	lua_pushstring(L, "id3v"); lua_pushstring(L, id3v); lua_settable(L, -3);
	lua_pushstring(L, "track"); lua_pushnumber(L, track); lua_settable(L, -3);
	
    return 1;
}

static const luaL_Reg AUDIO_methods[] = {
    {"cloud",   AALIB_load},
    {"play",    AALIB_play},
    {"info",    AALIB_getInfo},
    {"pause",   AALIB_pause},
    {"volume",  AALIB_volume},
    {"stop",    AALIB_stop},
    {"state",   AALIB_GetStatus},
    {"unload",  AALIB_unload},
    {0, 0}
};

int AUDIO_init(lua_State *L)
{
    AalibInit();

    luaL_register(L, "sound", AUDIO_methods);

    lua_pushstring(L, "sound");
    lua_gettable(L, LUA_GLOBALSINDEX);

    int c;
    for(c = 0; c <= 52; c++)
    {
        Const(audiochannels[c].name, audiochannels[c].number);
    }

    pgeMp3Vol = 32768;

    return 0;
}