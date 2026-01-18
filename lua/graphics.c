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

#include "graphics.h"
#include "LUA.h"
#include "../libs/intra/libtext.h"
#include "../libs/pmp/pmp.h"
#include "../libs/pmp/pmp_play.h"

#include "../libs/include_res/subs_font.c"
#include "../libs/include_res/watermarkk.c"

g2dImage *watermarkk;

#define MAX_SUBS 256
#define MAX_TEXT_LENGTH 512

typedef struct {
    char startTime[15];
    char endTime[15];
    char text[MAX_TEXT_LENGTH];
} Subtitle;

Subtitle subtitles[MAX_SUBS];
int subtitle_count = 0;

/******************************************************************
*****************************Color*********************************
******************************************************************/

UserdataStubs(Color, g2dColor)

static int COLOR_create(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 3 && args != 4)
        return luaL_error(L, "Color.new(r, g, b, [a]) takes 3 or 4 arguments");

    int r = luaL_checkint(L, 1);
    int g = luaL_checkint(L, 2);
    int b = luaL_checkint(L, 3);
    int a = (args == 4) ? luaL_checkint(L, 4) : 255;

    //lua_pushnumber(L, G2D_RGBA(r, g, b, a));
    *pushColor(L) = G2D_RGBA(r, g, b, a);

    return 1;
}

static int COLOR_getC(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 1 && args != 2)
        return luaL_error(L, "Color.get(color, [R/G/B/A]) takes 1 or 2 arguments");

    u32 color = *toColor(L, 1);
    
    if(args == 1)
    {
        lua_newtable(L);
        lua_pushstring(L, "r"); lua_pushnumber(L, G2D_GET_R(color)); lua_settable(L, -3);
        lua_pushstring(L, "g"); lua_pushnumber(L, G2D_GET_G(color)); lua_settable(L, -3);
        lua_pushstring(L, "b"); lua_pushnumber(L, G2D_GET_B(color)); lua_settable(L, -3);
        lua_pushstring(L, "a"); lua_pushnumber(L, G2D_GET_A(color)); lua_settable(L, -3);
    }
    else
    {
        const char *str = luaL_checkstring(L, 2);
        if(strcmp(str, "R") == 0)
            lua_pushnumber(L, G2D_GET_R(color));
        
        else if(strcmp(str, "G") == 0)
            lua_pushnumber(L, G2D_GET_G(color));
        
        else if(strcmp(str, "B") == 0)
            lua_pushnumber(L, G2D_GET_B(color));
        
        else if (strcmp(str, "A") == 0)
            lua_pushnumber(L, G2D_GET_A(color));
        
        else
            return luaL_error(L, "Color.get(color, [R/G/B/A]) incorrect argument");
    }
    return 1;
}

static const luaL_Reg COLOR_methods[] = {
    {"new",    COLOR_create},
    {"get",    COLOR_getC},
    {0, 0}
};

int COLOR_init(lua_State *L)
{
    luaL_register(L, "Color", COLOR_methods);

    return 1;
}

/******************************************************************
***************************intraFont*******************************
******************************************************************/

UserdataConvert(intraFont, intraFont*, "Font")

//UserdataStubs(intraFont, intraFont*)

static intraFont **loadedFonts = NULL;
static int fontCount = 0;
static int fontCapacity = 0;

void checkCapacity(void ***array, int *count, int *capacity, int minCapacity)
{
    if (minCapacity > *capacity) {
        int newCapacity = (*capacity == 0) ? minCapacity : *capacity + (minCapacity - *capacity);

        if (newCapacity < minCapacity)
            newCapacity = minCapacity;

        if (*array == NULL)
        {
            *array = malloc(newCapacity * sizeof(void *));
            if (!*array) {
                printf("Failed to allocate memory");
                sceKernelExitGame();
            }
        }
        else
        {
            *array = realloc(*array, newCapacity * sizeof(void *));
            if (!*array) {
                printf("Failed to allocate memory");
                sceKernelExitGame();
            }
        }

        *capacity = newCapacity;
    }
}

static void printLoadedItems(void **array, int count, int capacity, const char *type) {
    printf("Loaded %s:\n", type);
    int i;
    for (i = 0; i < count; i++) {
        if (array[i] != NULL) {
            printf("Index %d: %p\n", i, (void*)array[i]);
        } else {
            printf("Index %d: NULL\n", i);
        }
    }

    for (i = count; i < capacity; i++) {
        printf("Index %d: NULL (unused)\n", i);
    }
}

static int INTRAFONT_load(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 1 && args != 2)
        return luaL_error(L, "intraFont.load(path, [size]) takes 1 or 2 argument");

    const char* path = luaL_checkstring(L, 1);
    float size = luaL_optnumber(L, 2, 11.f);
    intraFont *font;
    
    size_t len = strlen(path);

    if (strcasecmp(path + len - 4, ".ttf") == 0 || strcasecmp(path + len - 4, ".otf") == 0)
    {
        font = intraFontLoadTTF(path, INTRAFONT_STRING_UTF8, size);
    }
    else if (strcasecmp(path + len - 4, ".pgf") == 0)
    {
        font = intraFontLoad(path, INTRAFONT_CACHE_ALL | INTRAFONT_STRING_UTF8);
    }
    
    if(!font || font == NULL)
        return luaL_error(L, "intraFont.load() error loading \"%s\"", path);

    checkCapacity((void ***)&loadedFonts, &fontCount, &fontCapacity, fontCount + 1);
    loadedFonts[fontCount++] = font;

    intraFont **fnt = pushintraFont(L);
    *fnt = font;
    
    printLoadedItems((void**)loadedFonts, fontCount, fontCapacity, "Fonts");

    return 1;
}

void INTRA_fontUnloadAll()
{
    int i;
    for (i = fontCount - 1; i >= 0; i--)
    {
        if (loadedFonts[i] != NULL)
        {
            intraFontUnload(loadedFonts[i]);
            loadedFonts[i] = NULL;
        }
    }

    free(loadedFonts);
    loadedFonts = NULL;
    fontCapacity = 0;

    printLoadedItems((void**)loadedFonts, fontCount, fontCapacity, "Fonts");
}

intraFont *getFont(lua_State *L, int index)
{
    if (lua_isnil(L, index))
        return luaFont;

    intraFont *font = *getintraFont(L, index);
    if (font == NULL)
        return luaFont;
    
    return font;
}

static int isFontLoaded(intraFont *font)
{
    int i;
    for (i = 0; i < fontCount; i++)
    {
        if (loadedFonts[i] == font)
            return 1;
    }

    return 0;
}

static int INTRAFONT_width(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 2 && args != 3)
        return luaL_error(L, "intraFont.textW(font, text, size) takes 2 or 3 arguments");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1): luaFont;
    if (!font || (font != luaFont && !isFontLoaded(font)))
        return luaL_error(L, "intraFont.textW() font not found");
    
    const char* text = luaL_checkstring(L, 2);
    float size = luaL_optnumber(L, 3, font->size);
    
    intraFontSetStyle(font, size, font->color, 0, 0.0f, 0);

    lua_pushnumber(L, intraFontMeasureText(font, text));

    return 1;
}

static int INTRAFONT_height(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.textH(font)");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1): luaFont;
    if (!font || (font != luaFont && !isFontLoaded(font)))
        return luaL_error(L, "intraFont.textH() font not found");

    float rez = intraFontTextHeight(font);

    lua_pushnumber(L, rez);

    return 1;
}

static int INTRAFONT_setStyle(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 4 && args != 5)
        return luaL_error(L, "intraFont.setStyle(font, size, color, angle, [align])");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1): luaFont;
    if(!font || !isFontLoaded(font))
        return luaL_error(L, "intraFont.setStyle() font not found");

    float size = luaL_checknumber(L, 2);
    u32 color = *toColor(L, 3);
    float angle = luaL_checknumber(L, 4);
    int mode = luaL_optnumber(L, 5, INTRAFONT_ALIGN_LEFT);
    
    intraFontSetStyle(font, size, color, 0, angle, mode);

    return 0;
}

static int INTRAFONT_size(lua_State *L)
{
    if(lua_gettop(L) != 2)
        return luaL_error(L, "intraFont.size(font, size) takes 2 arguments");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1): luaFont;
    if(!font || !isFontLoaded(font))
        return luaL_error(L, "intraFont.size() font not found");

    font->size = luaL_checknumber(L, 2);

    lua_pushnumber(L, font->size);
    
    return 1;
}

static int INTRAFONT_print(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 9)
        return luaL_error(L, "intraFont.print(x, y, text, [textColor], [font], [size], [textAngle], [GU_LINEAR], [AlMode]) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 color = (args >= 4) ? *toColor(L, 4) : WHITE;
    intraFont *font = (args >= 5) ? getFont(L, 5): luaFont;
    float size = luaL_optnumber(L, 6, 1);
    float angle = luaL_optnumber(L, 7, 0);
    bool linear = (lua_toboolean(L, 8)) ? true : false;
    float alMode = luaL_optnumber(L, 9, INTRAFONT_ALIGN_LEFT);
    
    intraFontSetStyle(font, size, color, 0, angle, alMode);

    intraFontActivate(font, linear);

    lua_pushnumber(L, intraFontPrint(font, x, y+intraFontTextHeight(font), text));

    return 1;
}

static int INTRAFONT_printColumn(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 4 || args > 10)
        return luaL_error(L, "intraFont.printColumn(x, y, text, width, [textColor], [font], [size], [align], [GU_LINEAR] [scroll]) takes 4, 5, 6, 7, 8, 9 or 10 arguments");

    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    float width = luaL_checknumber(L, 4);
    u32 color = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float size = luaL_optnumber(L, 7, 1.f);
    int align = luaL_optnumber(L, 8, INTRAFONT_ALIGN_LEFT);
    bool linear = (lua_toboolean(L, 9)) ? true : false;
    int scroll = luaL_optnumber(L, 10, INTRAFONT_ALIGN_LEFT);

    intraFontSetStyle(font, size, color, 0, 0.0f, align | scroll);
    
    intraFontActivate(font, linear);

    lua_pushnumber(L, intraFontPrintColumn(font, x, y+intraFontTextHeight(font), width, text));

    return 1;
}

static int INTRAFONT_printUnderlined(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 8)
        return luaL_error(L, "intraFont.printUnderlined(x, y, text, [textColor], [lineColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 lineColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;
    
    lua_pushnumber(L, UnderLinedText(x, y+intraFontTextHeight(font), font, text, textSize, textColor, lineColor, linear));
    return 1;
}

static int INTRAFONT_printContoured(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 9)
        return luaL_error(L, "intraFont.printContoured(x, y, text, [textColor], [contourColor], [font], [textSize], [textAngle], [GU_LINEAR]) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 cColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    float textAngle = luaL_optnumber(L, 8, 0);
    bool linear = (lua_toboolean(L, 9)) ? true : false;
    
    lua_pushnumber(L, ContouredText(x, y+intraFontTextHeight(font), font, text, textSize, textAngle, textColor, cColor, INTRAFONT_ALIGN_LEFT, linear));
    
    return 1;
}

static int INTRAFONT_printBackground(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 8)
        return luaL_error(L, "intraFont.printBackground(x, y, text, [textColor], [bgColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : BLACK;
    u32 bgColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;
    
    lua_pushnumber(L, BackgroundColorText(x, y+intraFontTextHeight(font), font, text, textSize, textColor, bgColor, INTRAFONT_ALIGN_LEFT, linear));

    return 1;
}

static int INTRAFONT_printStriked(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 8)
        return luaL_error(L, "intraFont.printStriked(x, y, text, [textColor], [lineColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 lineColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;    
    
    lua_pushnumber(L, StrikedText(x, y+intraFontTextHeight(font), font, text, textSize, textColor, lineColor, linear));

    return 1;
}

static int INTRAFONT_printShadowed(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 11)
        return luaL_error(L, "intraFont.printShadowed(x, y, text, [textColor], [shadowColor], [font], [shadowAngle], [lightDistance], [textSize], [textAngle], [GU_LINEAR]) takes 3, 4, 5, 6, 7, 8, 9, 10 or 11 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 shadowColor = (args >= 5) ? *toColor(L, 5) : BLACK;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    int shadowAngle = luaL_optnumber(L, 7, 45);
    int dist = luaL_optnumber(L, 8, 2);
    float textSize = luaL_optnumber(L, 9, 1);
    int textAngle = luaL_optnumber(L, 10, 0);
    bool linear = (lua_toboolean(L, 11)) ? true : false;    
    
    lua_pushnumber(L, ShadowedText(x, y+intraFontTextHeight(font), font, text, textSize, textAngle, shadowAngle, dist, textColor, shadowColor, linear));

    return 1;
}

static int INTRAFONT_printReversed(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.reverseText(text) takes 1 argument");

    const char* text = luaL_checkstring(L, 1);
    
    char* reversedText = InversedText(text);
    
    if (reversedText == NULL)
        return luaL_error(L, "Error processing the input text.");

    lua_pushstring(L, reversedText);
    free(reversedText);
    return 1;
}

static int INTRAFONT_printGradient(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 8)
        return luaL_error(L, "intraFont.printGradient(x, y, text, [textColorStart], [textColorEnd], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char* text = luaL_checkstring(L, 3);
    u32 textColor1 = (args >= 4) ? *toColor(L, 4) : RED;
    u32 textColor2 = (args >= 5) ? *toColor(L, 5) : BLUE;
    intraFont *font = (args >= 6) ? getFont(L, 6): luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;    

    GradientText(x, y, font, text, textColor1, textColor2, textSize, linear);

    return 0;
}

static int INTRAFONT_unload(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.unload(font) takes 1 argument");

    intraFont* font = *getintraFont(L, 1);

    intraFontUnload(font);

    return 0;
}

static const luaL_Reg INTRAFONT_methods[] = {
    {"load",            INTRAFONT_load},
    {"print",           INTRAFONT_print},
    {"printColumn",     INTRAFONT_printColumn},
    {"printUnderlined", INTRAFONT_printUnderlined},
    {"printContoured",  INTRAFONT_printContoured},
    {"printBackground", INTRAFONT_printBackground},
    {"printStriked",    INTRAFONT_printStriked},
    {"printShadowed",   INTRAFONT_printShadowed},
    {"printGradient",   INTRAFONT_printGradient},
    {"reverseText",     INTRAFONT_printReversed},
    {"setStyle",        INTRAFONT_setStyle},
    {"size",            INTRAFONT_size},
    {"textW",           INTRAFONT_width},
    {"textH",           INTRAFONT_height},
    {"unload",          INTRAFONT_unload},
    {0, 0}
};

static const luaL_Reg INTRAFONT_metamethods[] = {
    //{"__tostring", INTRAFONT_tostring},
    // {"__gc", INTRAFONT_unload},
    {0, 0}
};

int INTRAFONT_init(lua_State *L)
{
    intraFontInit();

    UserdataRegister("intraFont", INTRAFONT_methods, INTRAFONT_metamethods);

    lua_pushstring(L, "intraFont");
    lua_gettable(L, LUA_GLOBALSINDEX);

    Const("ALIGN_LEFT",     INTRAFONT_ALIGN_LEFT)
    Const("ALIGN_CENTER",   INTRAFONT_ALIGN_CENTER)
    Const("ALIGN_RIGHT",    INTRAFONT_ALIGN_RIGHT)
    Const("SCROLL_LEFT",    INTRAFONT_SCROLL_LEFT)
    Const("SCROLL_RIGHT",   INTRAFONT_SCROLL_RIGHT)
    Const("SCROLL_SEESAW",  INTRAFONT_SCROLL_SEESAW)
    Const("SCROLL_THROUGH", INTRAFONT_SCROLL_THROUGH)

    return 1;
}

/******************************************************************
*****************************Image*********************************
******************************************************************/

UserdataStubs(G2D, g2dImage*)
//UserdataConvert(G2D, g2dImage*, "Image")

void G2D_RECT(int x, int y, int w, int h, int rotation, int alpha, g2dColor color, int AlMode)
{
    g2dBeginRects(NULL);
    g2dSetCoordMode(AlMode);
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(w, h);
    g2dSetRotation(rotation);
    g2dSetAlpha(setInterval(alpha, 0, 255));
    g2dSetColor(color);
    g2dAdd();
    g2dEnd();
}

void G2D_CIRCLE(int cx, int cy, float radius, u32 color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while(x <= y)
    {
        G2D_RECT(cx + x, cy + y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - x, cy + y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + x, cy - y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - x, cy - y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + y, cy + x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - y, cy + x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + y, cy - x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - y, cy - x, 1, 1, 0, 255, color, G2D_CENTER);

        if(d < 0)
            d = d + 4 * x + 6;
        else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void G2D_TRIANGLE(int x1, int y1, int x2, int y2, int x3, int y3, u32 color)
{
    g2dBeginLines(G2D_STRIP);
    g2dSetColor(color);
    g2dSetCoordXY(x1, y1);
    g2dAdd();
    g2dSetCoordXY(x2, y2);
    g2dAdd();
    g2dSetCoordXY(x3, y3);
    g2dAdd();
    g2dSetCoordXY(x1, y1);
    g2dAdd();
    g2dEnd();
}

void G2D_CUBE(int x, int y, int z, int size, u32 color)
{
    int halfSize = size / 2;

    // Определение вершин куба
    int points[8][3] = {
        {x - halfSize, y - halfSize, z - halfSize}, // 0
        {x + halfSize, y - halfSize, z - halfSize}, // 1
        {x + halfSize, y + halfSize, z - halfSize}, // 2
        {x - halfSize, y + halfSize, z - halfSize}, // 3
        {x - halfSize, y - halfSize, z + halfSize}, // 4
        {x + halfSize, y - halfSize, z + halfSize}, // 5
        {x + halfSize, y + halfSize, z + halfSize}, // 6
        {x - halfSize, y + halfSize, z + halfSize}  // 7
    };

    g2dBeginLines(G2D_STRIP);
    g2dSetColor(color);

    int i;
    // Рисуем переднюю грань
    for (i = 0; i < 4; i++) {
        g2dSetCoordXY(points[i][0], points[i][1]);
        g2dAdd();
    }
    g2dSetCoordXY(points[0][0], points[0][1]); // Замкнуть грань
    g2dAdd();

    // Рисуем заднюю грань
    for (i = 4; i < 8; i++) {
        g2dSetCoordXY(points[i][0], points[i][1]);
        g2dAdd();
    }
    g2dSetCoordXY(points[4][0], points[4][1]); // Замкнуть грань
    g2dAdd();

    // Рисуем соединяющие линии
    for (i = 0; i < 4; i++) {
        g2dSetCoordXY(points[i][0], points[i][1]);
        g2dAdd();
        g2dSetCoordXY(points[i + 4][0], points[i + 4][1]);
        g2dAdd();
    }

    g2dEnd();
}

static int G2D_clear(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 0 && args != 1)
        return luaL_error(L, "screen.clear([color]) takes 0 or 1 argument");

    u32 color = ((args == 1) && !lua_isnil(L, 1))? *toColor(L, 1) : BLACK;

    g2dClear(color);

    return 0;
}

static int G2D_flip(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "screen.flip() takes no arguments");

    g2dFlip(G2D_VSYNC);

    return 0;
}

static g2dImage **loadedImages = NULL;
static int imageCount = 0;
static int capacity = 0;

static int removeLoadedImage(g2dImage *tex)
{
    int i, j;
    for (i = 0; i < imageCount; i++)
    {
        if (loadedImages[i] == tex)
        {
            g2dTexFree(&tex); 
            
            if (i < imageCount - 1)
            {
                for (j = i; j < imageCount - 1; j++)
                    loadedImages[j] = loadedImages[j + 1];
            }
            
            loadedImages[--imageCount] = NULL; 

            if (capacity > 1)
            {
                capacity--; // Уменьшаем емкость
                loadedImages = realloc(loadedImages, capacity * sizeof(g2dImage *));
            if (!loadedImages && imageCount > 0)
                {
                    printf("Failed to reallocate memory");
                    sceKernelExitGame();
                }
            }
            else if (capacity == 1)
            {
                free(loadedImages);
                loadedImages = NULL;
                capacity = 0;
            }
            return 1;
        }
    }

    return 0;
}

static int G2D_texLoad(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "Image.load(path) takes 1 argument");

    char *path = (char*)luaL_checkstring(L, 1);
    g2dImage *img = g2dTexLoad(path, NULL, 0, G2D_VOID);
    if(!img)
        return luaL_error(L, "Image.load() error loading \"%s\"", path);

    checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
    loadedImages[imageCount++] = img;

    g2dImage** image = pushG2D(L);
	*image = img;

    return 1;
}

static int G2D_createPlaceholder(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "Image.createPlaceholder() takes no arguments");

    g2dImage *img = g2dTexCreatePlaceholder();
    if (!img)
        return luaL_error(L, "Image.createPlaceholder() error");

    checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
    loadedImages[imageCount++] = img;

    g2dImage** image = pushG2D(L);
	*image = img;

    return 1;
}

static int G2D_texUnload(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "Image.unload(texture) takes 1 arguments");

    g2dImage *img = *toG2D(L, 1);
    if(!img)
        return luaL_error(L, "Image.unload() can't get the texture");

    if (removeLoadedImage(img))
    {
        printLoadedItems((void**)loadedImages, imageCount, capacity, "Images");
        return 0;
    }

    return luaL_error(L, "Image not found in loaded images");
}

void G2D_texUnloadAll()
{
    int i;
    for (i = imageCount - 1; i >= 0; i--) {
        if (loadedImages[i] != NULL) {
            g2dTexFree(&loadedImages[i]);
            loadedImages[i] = NULL;
        }
    }

    imageCount = 0;

    if (capacity > 1)
    {
        loadedImages = realloc(loadedImages, sizeof(g2dImage *));
        if (loadedImages == NULL)
        {
            printf("Failed to reallocate memory");
            sceKernelExitGame();
        }
        capacity = 1;
    }

    printLoadedItems((void**)loadedImages, imageCount, capacity, "Images");
}

static int isImageLoaded(g2dImage *img) {
    int i;
    for (i = 0; i < imageCount; i++)
    {
        if (loadedImages[i] == img)
            return 1;
    }
    return 0;
}

static int G2D_getW(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "Image.W(texture) takes 1 argument");

    g2dImage *img = *toG2D(L, 1);
    if(!img || !isImageLoaded(img))
        return luaL_error(L, "Image.W() can't get the texture");

    lua_pushinteger(L, img->w);

    return 1;
}

static int G2D_getH(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "Image.H(texture) takes 1 argument");

    g2dImage *img = *toG2D(L, 1);
    if(!img || !isImageLoaded(img))
        return luaL_error(L, "Image.H() can't get the texture");

    lua_pushinteger(L, img->h);

    return 1;
}

static int G2D_draw(lua_State *L)
{
    int args = lua_gettop(L);
    if (args < 3 || args > 16 || (args == 4 && luaL_checknumber(L, 4) <= 0))
        return luaL_error(L, "Image.draw(texture, x, y, [width], [height], [color], [srcx], [srcy], [srcw], [srch], [rotation], [alpha], [origin_x], [origin_y], [GU_LINEAR], [GU_REPEAT]) takes 3, 5, 6, 10, 11, 12, 13, 14, 15 or 16 arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img || !isImageLoaded(img))
        return luaL_error(L, "Image.draw() can't get the texture");

    int x = luaL_checknumber(L, 2), y = luaL_checknumber(L, 3);
    int w = luaL_optnumber(L, 4, img->w), h = luaL_optnumber(L, 5, img->h);
    u32 color = (args >= 6 && !lua_isnil(L, 6)) ? *toColor(L, 6) : 0;
    int srcx = luaL_optnumber(L, 7, 0), srcy = luaL_optnumber(L, 8, 0);
    int srcw = luaL_optnumber(L, 9, img->w), srch = luaL_optnumber(L, 10, img->h);
    float Angle = luaL_optnumber(L, 11, 0.0f);
    int a = luaL_optnumber(L, 12, 255);
    int origin_x = luaL_optnumber(L, 13, 0);
    int origin_y = luaL_optnumber(L, 14, 0);
    bool linear = (lua_toboolean(L, 15)) ? true : false;
    bool repeat = (lua_toboolean(L, 16)) ? true : false;

    g2dBeginRects(img);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(repeat);
    g2dSetCoordXY(x, y);
    g2dSetCropXY(srcx, srcy); 
    g2dSetCropWH(srcw, srch);
    g2dSetScaleWH(w, h);
    g2dSetRotation(Angle);
    if(color != 0)
        g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int G2D_draweasy(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 3 || args > 10)
        return luaL_error(L, "Image.draweasy(texture, x, y, [color], [rotation], [alpha], [origin_x], [origin_y], [GU_LINEAR], [GU_REPEAT]) takes 3, 4, 5, 6, 7, 8, 9 or 10 arguments");

    g2dImage *img = *toG2D(L, 1);
    if(!img || !isImageLoaded(img))
        return luaL_error(L, "Image.draweasy() can't get the texture");
    
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    u32 color = (args >= 4 && !lua_isnil(L, 4)) ? *toColor(L, 4) : 0;
    float Angle = luaL_optnumber(L, 5, 0.0f);
    int a = luaL_optnumber(L, 6, 255);
    int origin_x = luaL_optnumber(L, 7, 0);
    int origin_y = luaL_optnumber(L, 8, 0);
    bool linear = (lua_toboolean(L, 9)) ? true : false;
    bool repeat = (lua_toboolean(L, 10)) ? true : false;

    g2dBeginRects(img);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(repeat);
    g2dSetCoordXY(x, y);
    g2dSetRotation(Angle);
    if(color != 0)
        g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int G2D_filledRect(lua_State *L)
{
    int args = lua_gettop(L);
    if(args < 5 || args > 8)
        return luaL_error(L, "screen.filledRect(x, y, width, height, color, [rotation] [alpha], [mode]) takes 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    int w = luaL_checknumber(L, 3);
    int h = luaL_checknumber(L, 4);
    u32 color = *toColor(L, 5);
    int Angle = luaL_optnumber(L, 6, 0);
    int a = luaL_optnumber(L, 7, 255);
    int AlMode = luaL_optnumber(L, 8, G2D_UP_LEFT);

    G2D_RECT(x, y, w, h, Angle, a, color, AlMode);

    return 0;
}

static int G2D_drawCircle(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 4)
        return luaL_error(L, "screen.drawCircle(x, y, radius, color) takes 4 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    float r = luaL_checknumber(L, 3);
    u32 color = *toColor(L, 4);

    G2D_CIRCLE(x, y, r, color);

    return 0;
}

static int G2D_drawTriangle(lua_State *L)
{
    int args = lua_gettop(L);
    if(args != 7)
        return luaL_error(L, "screen.drawTriangle(x1, y1, x2, y2, x3, y3, color) takes 7 arguments");

    int x1 = luaL_checknumber(L, 1);
    int y1 = luaL_checknumber(L, 2);
    int x2 = luaL_checknumber(L, 3);
    int y2 = luaL_checknumber(L, 4);
    int x3 = luaL_checknumber(L, 5);
    int y3 = luaL_checknumber(L, 6);
    u32 color = *toColor(L, 7);

    G2D_TRIANGLE(x1, y1, x2, y2, x3, y3, color);

    return 0;
}

static int G2D_drawLine(lua_State *L)
{
    if (lua_gettop(L) != 5) 
        return luaL_error(L, "screen.drawLine(x1, y1, x2, y2, color) takes 5 arguments");

    int x1 = luaL_checknumber(L, 1);
    int y1 = luaL_checknumber(L, 2);
    int x2 = luaL_checknumber(L, 3);
    int y2 = luaL_checknumber(L, 4);
    u32 color = *toColor(L, 5);

    g2dBeginLines(G2D_STRIP);
    g2dSetColor(color);
    g2dSetCoordXY(x1, y1);
    g2dAdd();
    g2dSetCoordXY(x2, y2);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int G2D_drawCube(lua_State *L)
{
    G2D_CUBE(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), *toColor(L, 5));

    return 0;
}

static int G2D_drawCircleOnTex(lua_State *L)
{

    g2dImage *tex = *toG2D(L, 1);
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int r = luaL_checknumber(L, 4);
    u32 color = *toColor(L, 5);

    draw_circle(tex, x, y, r, r, color);
    return 0;
}

static const luaL_Reg GFX_methods[] = {
    {"clear",        G2D_clear},
    {"flip",         G2D_flip},
    {"filledRect",   G2D_filledRect},
    {"drawLine",     G2D_drawLine},
    {"drawCircle",   G2D_drawCircle},
    {"drawTriangle", G2D_drawTriangle},
    //{"drawCube",     G2D_drawCube},
    {0, 0}
};

static const luaL_Reg GFX_methods2[] = {
    {"load",         G2D_texLoad},
    {"W",            G2D_getW},
    {"H",            G2D_getH},
    {"draw",         G2D_draw},
    {"draweasy",     G2D_draweasy},
    {"unload",       G2D_texUnload},
    {"drawCircleOnTex", G2D_drawCircleOnTex},
    {"createPlaceholder", G2D_createPlaceholder},
    {0, 0}
};

static const luaL_Reg GFX_metamethods[] = {
    // {"__gc", G2D_texUnload},
    {0, 0}
};

int GRAPHICS_init(lua_State *L)
{
    g2dInit();

    luaL_register(L, "screen", GFX_methods);
    //luaL_register(L, "Image", GFX_methods2);
    UserdataRegister("Image", GFX_methods2, GFX_metamethods);
    
    lua_pushstring(L, "Image");
    lua_gettable(L, LUA_GLOBALSINDEX);

    Const("Center", G2D_CENTER)
    Const("lUp",    G2D_UP_LEFT)
    Const("lDn",    G2D_DOWN_LEFT)
    Const("rUp",    G2D_UP_RIGHT)
    Const("rDn",    G2D_DOWN_RIGHT)

    return 1;
}

/******************************************************************
*****************************Video*********************************
******************************************************************/

g2dImage *PMP_CUR_FRAME;
intraFont *SUBTITLE_FONT;

static void clear_subtitles()
{
    // Очистка всех субтитров
    int i;
    for (i = 0; i < MAX_SUBS; i++) {
        subtitles[i].startTime[0] = '\0'; // Очистить временную метку начала
        subtitles[i].endTime[0] = '\0';   // Очистить временную метку окончания
        subtitles[i].text[0] = '\0'; // Очистить текст субтитра
    }
    subtitle_count = 0; // Сбросить количество субтитров
}

static void parseSRT(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Cannot open SRT file");
        return;
    }

    char byte;
    int found = 0;

    while ((byte = fgetc(file)) != EOF)
    {
        if (byte == 0x31)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Byte 0x31 not found in the file.\n");
        fclose(file);
        return;
    }

    fseek(file, -1, SEEK_CUR);

    char line[MAX_TEXT_LENGTH];
    while (subtitle_count < MAX_SUBS)
    {
        Subtitle sub = {.text = ""}; // Инициализация текста, пустая строка

        if (fscanf(file, "%*d\n") != 0)
            break;

        if (fscanf(file, "%[^ ] --> %[^ \n]\n", sub.startTime, sub.endTime) != 2)
            break;
        
        // Обнуляем текст перед загрузкой новых данных
        sub.text[0] = '\0';

        while (fgets(line, sizeof(line), file))
        {
            line[strcspn(line, "\r\n")] = 0; // Убираем символы конца строки

            if (strlen(line) == 0)
                break; // Если пустая строка, выходим из цикла

            // Добавление текста в одно поле
            if (strlen(sub.text) + strlen(line) < MAX_TEXT_LENGTH) { 
                strcat(sub.text, line); // Добавляем в текст
                strcat(sub.text, "\n"); // Добавляем перевод строки
            }
        }

        if (strlen(sub.text) > 0) {
            subtitles[subtitle_count++] = sub; // Добавляем субтитр, если текст не пуст
        }
    }

    fclose(file);
}

static void parseSubs(char* path)
{
    size_t len = strlen(path);

    if (strcmp(path + len - 4, ".srt") == 0)
    	parseSRT(path);
		
}

static char* LPYTVideoPath = NULL;
static int LPYTVideoPlay = 0;
static int PMP_LOOP = 0;

static void getFrame(g2dImage *tex)
{
    int orig_width, orig_height;
    void* frame_data = pmp_get_frame(NULL, &orig_width, &orig_height, NULL);

    if (!frame_data)
        return;

    uint32_t* src = (uint32_t*)frame_data;
            
    int x, y;
    for (y = 0; y < 272; y++)
    {
        int orig_y = (y * orig_height) *INVERSE_272;
        uint32_t* row = src + orig_y * 512;

        for (x = 0; x < 480; x++)
        {
            int orig_x = (x * orig_width) * INVERSE_480;
            uint32_t original_color = row[orig_x];

            uint8_t r = G2D_GET_R(original_color);
            uint8_t g = G2D_GET_G(original_color);
            uint8_t b = G2D_GET_B(original_color);

            uint32_t rgba_color = (r) | (g << 8) | (b << 16) | 0xFF000000; 
            tex->data[x + y * tex->tw] = rgba_color; 
        }
    }

    // sceKernelDcacheWritebackInvalidateAll();
    sceKernelDcacheWritebackAll();
    //sceKernelDcacheWritebackRange(tex->data, tex->tw * tex->th * sizeof(uint32_t));
}

static float convertTimecodeToSeconds(const char *timecode)
{
    int hours, minutes, seconds, milliseconds;
    sscanf(timecode, "%d:%d:%d,%d", &hours, &minutes, &seconds, &milliseconds);
    return hours * 3600 + minutes * 60 + seconds + milliseconds * 0.001f;
}

static float getTimeCode()
{
    return ((float)PMP_CURRENT_FRAME * (1/PMP_FPS) - 0.35f);
}


char *getSubString()
{
    if (pmp_isplaying() && PMP_GOT_SUBS)
    {
        int i;

        for (i = 0; i < subtitle_count; i++)
        {
            float timeCode = getTimeCode();
            if (timeCode > convertTimecodeToSeconds(subtitles[i].startTime) && timeCode < convertTimecodeToSeconds(subtitles[i].endTime))
                return subtitles[i].text;
        }
        return "";
    }

    return "";
}

static int PMP_play(lua_State *L)
{
    if (LPYTVideoPlay || pmp_isplaying() || PMP_PAUSE)
    {
        pmp_stop();

        clear_subtitles();
        removeLoadedImage(PMP_CUR_FRAME);
        if (LPYTVideoPath)
        {
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
        }
        PMP_GOT_SUBS = 0;
        PMP_CURRENT_FRAME = 0;
        PMP_PAUSE = FALSE;
    }

    int args = lua_gettop(L);
    if(args < 1 || args > 6)
        return luaL_error(L, "PMP.play(path, getPointer, [loop], [subtitlePath], [interruptButton], [FPS]) takes 1, 2, 3, 4, 5 or 6 arguments"); 

    char* path = (char*)luaL_checkstring(L, 1);
    bool getPointer = lua_toboolean(L, 2);
    PMP_LOOP = lua_toboolean(L, 3);
    char* subtitlepath = (args >= 4 && !lua_isnil(L, 4)) ? (char*)luaL_checkstring(L, 4) : NULL;
    PMP_INTERRUPT_BUTTON = (args >= 5 && !lua_isnil(L, 5)) ? luaL_checkint(L, 5) : 69;
    PMP_FPS = (args == 6) ? luaL_checknumber(L, 6) : 29.97;
    
    PMP_CUR_FRAME = _g2dTexCreate(480, 272, true);
    
    if (!PMP_CUR_FRAME)
        return luaL_error(L, "PMP.play() internal error");

    if (subtitlepath)
    {
        size_t len = strlen(subtitlepath);

        if (strcmp(subtitlepath + len - 4, ".srt") != 0 || !strcmp(subtitlepath + len - 4, ".ass") != 0)
            return luaL_error(L, "PMP.play() error: you're trying to load non-srt/ass subtitle file");

        parseSubs(subtitlepath);
        PMP_GOT_SUBS = 1;
    }

    if (getPointer)
    {
        if (pmp_play(path, 0, GU_PSM_8888) != 0)
            return luaL_error(L, "PMP.play() error: file \"%s\" doesn't exist or some internal error", path);

        if (LPYTVideoPath)
        {
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
        }

        LPYTVideoPath = (char*)malloc(512);

        if (!LPYTVideoPath)   
            return luaL_error(L, "PMP.play() internal error");

        memcpy(LPYTVideoPath, path, 512);

        LPYTVideoPlay = 1;

        checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
        loadedImages[imageCount++] = PMP_CUR_FRAME;

        printLoadedItems((void**)loadedImages, imageCount, capacity, "Images");

        g2dImage **image = pushG2D(L);
        *image = PMP_CUR_FRAME;

        return 1;
    }

    if (pmp_play(path, 1, GU_PSM_8888) != 0)
        return luaL_error(L, "PMP.play() error: file \"%s\" doesn't exist or some internal error", path);

    save_to_file("temp_sub_font.pgf", subs_font, size_subs_font);
    SUBTITLE_FONT = intraFontLoad("temp_sub_font.pgf", INTRAFONT_CACHE_LARGE | INTRAFONT_STRING_UTF8);
    remove("temp_sub_font.pgf");

    while(pmp_isplaying())
    {
        g2dClear(BLACK);
        getFrame(PMP_CUR_FRAME);
        
        g2dBeginRects(PMP_CUR_FRAME);
        g2dAdd();
        g2dEnd();

        if (PMP_GOT_SUBS && strcmp(getSubString(), "") != 0)
            BackgroundColorText(240, 205 + intraFontTextHeight(SUBTITLE_FONT), SUBTITLE_FONT, getSubString(), 1.0, WHITE, 0xDE000000, INTRAFONT_ALIGN_CENTER, false);
        

        g2dFlip(G2D_VSYNC);
    }
    
    pmp_stop();

    clear_subtitles();
    g2dTexFree(&PMP_CUR_FRAME);
    intraFontUnload(SUBTITLE_FONT);
    PMP_GOT_SUBS = 0;

    return 0;
}

static int PMP_getFrame(lua_State *L)
{
    if (LPYTVideoPlay)
    {
        if (pmp_isplaying())
        {  
            
            if (lua_gettop(L) != 1)
            {
                pmp_stop();
                return luaL_error(L, "PMP.getFrame(pointer) takes 1 argument");
            }
            
            if (!PMP_PAUSE)
            {
                g2dImage *tex = *toG2D(L, 1);
                getFrame(tex);
            } 
            lua_pushboolean(L, TRUE);
        }
        else if (PMP_LOOP)
        {
            pmp_stop();
            pmp_play(LPYTVideoPath, 0, GU_PSM_8888);
            lua_pushboolean(L, TRUE);
        }
        else if (!PMP_LOOP)
            lua_pushnil(L);

        return 1;
    }
    
    lua_pushnil(L);
    return 1;

}

static int PMP_stop(lua_State *L)
{
    int args = lua_gettop(L);
    if (args != 1)
    {
        pmp_stop();
        return luaL_error(L, "PMP.stop(pointer) takes 1 argument");
    }

    if (pmp_isplaying() || LPYTVideoPlay)
    {
        pmp_stop();
        g2dImage *tex = *toG2D(L, 1);
        
        if (removeLoadedImage(tex))
        {
            clear_subtitles();
            LPYTVideoPlay = 0;
            PMP_GOT_SUBS = 0;
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
            printLoadedItems((void**)loadedImages, imageCount, capacity, "Images");
            return 0;
        } 

        return luaL_error(L, "Image not found in loaded images");
    }
    
    return 0;
}

static int PMP_setVolume(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "PMP.setVolume(volume) takes 1 argument");

    int vol = setInterval(luaL_checknumber(L, 1), 0, 100);
    
    PMP_AUDIO_VOLUME = ceil((32768/100)*vol);

    return 1;
}

static int PMP_getTimeCode(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getTimeCode() takes no arguments");

    if (pmp_isplaying())
    {
        char rez[20];
        sprintf(rez, "%.2f", getTimeCode());
        lua_pushstring(L, rez);
    }
    else
        lua_pushnil(L);

    return 1;
}

static int PMP_getSubs(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getSubs() takes no arguments");

    lua_pushstring(L, getSubString());

    return 1;
}

#include "../libs/Mp4/mp4info.h"

static int MP4_Info(lua_State *L)
{
    // Открываем MP4 файл
    mp4info_t* info = mp4info_open(luaL_checkstring(L, 1));
    if (!info) {
        //printf("Error: Could not open MP4 file %s\n", filename);
        return -1;
    }

    printf("\nMP4 File Information: %s\n", luaL_checkstring(L, 1));
    printf("---------------------------------\n");
    printf("Time Scale: %ld\n", info->time_scale);
    printf("Duration: %ld\n", info->duration);
    printf("Total Tracks: %ld\n", info->total_tracks);
    printf("\n");

    for (int i = 0; i < info->total_tracks; i++) {
        mp4info_track_t* track = info->tracks[i];
        
        printf("Track %d:\n", i+1);
        printf("  Type: ");
        switch(track->type) {
            case TRACK_AUDIO: printf("Audio\n"); break;
            case TRACK_VIDEO: printf("Video\n"); break;
            case TRACK_SYSTEM: printf("System\n"); break;
            default: printf("Unknown\n");
        }
        
        printf("  Time Scale: %ld\n", track->time_scale);
        printf("  Duration: %ld\n", track->duration);
        
        if (track->type == TRACK_VIDEO) {
            printf("  Video Type: 0x%08lX\n", track->video_type);
            printf("  Width: %ld\n", track->width);
            printf("  Height: %ld\n", track->height);
            
            if (track->avc_sps_size > 0) {
                printf("  AVC Profile: 0x%02lX\n", track->avc_profile);
                printf("  AVC SPS Size: %ld\n", track->avc_sps_size);
                printf("  AVC PPS Size: %ld\n", track->avc_pps_size);
                printf("  AVC NAL Prefix Size: %ld\n", track->avc_nal_prefix_size);
            }
        }
        else if (track->type == TRACK_AUDIO) {
            printf("  Audio Type: 0x%08lX\n", track->audio_type);
            printf("  Channels: %ld\n", track->channels);
            printf("  Sample Rate: %ld\n", track->samplerate);
            printf("  Sample Bits: %ld\n", track->samplebits);
        }
        
        printf("  STTS Entries: %ld\n", track->stts_entry_count);
        printf("  CTTS Entries: %ld\n", track->ctts_entry_count);
        printf("  STSS Entries: %ld\n", track->stss_entry_count);
        printf("  STSC Entries: %ld\n", track->stsc_entry_count);
        printf("  STSZ Entries: %ld\n", track->stsz_entry_count);
        printf("  STCO Entries: %ld\n", track->stco_entry_count);
        printf("\n");
    }
    
    // Закрываем файл и освобождаем ресурсы
    mp4info_close(info);

    return 0;
}

static int PMP_pause(lua_State *L)
{
    PMP_PAUSE = !PMP_PAUSE;
    
    return 0;
}

static int PMP_seek(lua_State *L)
{
    pmp_seek(luaL_checknumber(L, 1));

    return 0;
}

static const luaL_Reg PMP_methods[] = {
    {"play",        PMP_play},
    {"setVolume",   PMP_setVolume},
    {"getFrame",    PMP_getFrame},
    {"stop",        PMP_stop},
    {"getTimeCode", PMP_getTimeCode},
    {"getSubs",     PMP_getSubs},
    {"pause",       PMP_pause},
    {"Mp4_Info",    MP4_Info},
    //{"seek",        PMP_seek},
    {0, 0}
};

int PMP_init(lua_State *L)
{
    pmp_init();
    PMP_AUDIO_VOLUME = 32768;
    PMP_INTERRUPT_BUTTON = PSP_CTRL_START;
    PMP_GAME_EXIT = 0;
    PMP_GOT_SUBS = 0;
    PMP_PAUSE = 0;
    PMP_LAST_FRAME = 0;
    PMP_CURRENT_FRAME = 0;

    //watermarkk = g2dTexLoad(NULL, watermark, size_watermark, G2D_VOID);

    luaL_register(L, "PMP", PMP_methods);

    return 1;
}