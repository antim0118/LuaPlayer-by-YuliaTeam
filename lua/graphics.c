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

/******************************************************************
*****************************Color*********************************
******************************************************************/

UserdataStubs(Color, g2dColor)

static int COLOR_create(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 3 && args != 4)
        return luaL_error(L, "Color.new(r, g, b, [a]) takes 3 or 4 arguments");

    int r = luaL_checkint(L, 1);
    int g = luaL_checkint(L, 2);
    int b = luaL_checkint(L, 3);
    int a = (args == 4) ? luaL_checkint(L, 4) : 255;

    //lua_pushnumber(L, G2D_RGBA(r, g, b, a));
    *pushColor(L) = G2D_RGBA(r, g, b, a);

    return 1;
}

static int COLOR_getC(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1 && args != 2)
        return luaL_error(L, "Color.get(color, [R/G/B/A]) takes 1 or 2 arguments");

    u32 color = *toColor(L, 1);

    if (args == 1) {
        lua_newtable(L);
        lua_pushstring(L, "r"); lua_pushnumber(L, G2D_GET_R(color)); lua_settable(L, -3);
        lua_pushstring(L, "g"); lua_pushnumber(L, G2D_GET_G(color)); lua_settable(L, -3);
        lua_pushstring(L, "b"); lua_pushnumber(L, G2D_GET_B(color)); lua_settable(L, -3);
        lua_pushstring(L, "a"); lua_pushnumber(L, G2D_GET_A(color)); lua_settable(L, -3);
    } else {
        const char *str = luaL_checkstring(L, 2);
        if (strcmp(str, "R") == 0)
            lua_pushnumber(L, G2D_GET_R(color));

        else if (strcmp(str, "G") == 0)
            lua_pushnumber(L, G2D_GET_G(color));

        else if (strcmp(str, "B") == 0)
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

int COLOR_init(lua_State *L) {
    luaL_register(L, "Color", COLOR_methods);

    return 1;
}

/******************************************************************
***************************intraFont*******************************
******************************************************************/

UserdataConvert(intraFont, intraFont *, "Font")

//UserdataStubs(intraFont, intraFont*)

static intraFont **loadedFonts = NULL;
static int fontCount = 0;
static int fontCapacity = 0;

void checkCapacity(void ***array, int *count, int *capacity, int minCapacity) {
    if (minCapacity > *capacity) {
        int newCapacity = (*capacity == 0) ? minCapacity : *capacity + (minCapacity - *capacity);

        if (newCapacity < minCapacity)
            newCapacity = minCapacity;

        if (*array == NULL) {
            *array = malloc(newCapacity * sizeof(void *));
            if (!*array) {
                printf("Failed to allocate memory");
                sceKernelExitGame();
            }
        } else {
            *array = realloc(*array, newCapacity * sizeof(void *));
            if (!*array) {
                printf("Failed to allocate memory");
                sceKernelExitGame();
            }
        }

        *capacity = newCapacity;
    }
}

void printLoadedItems(void **array, int count, int capacity, const char *type) {
    printf("Loaded %s:\n", type);
    int i;
    for (i = 0; i < count; i++) {
        if (array[i] != NULL) {
            printf("Index %d: %p\n", i, (void *)array[i]);
        } else {
            printf("Index %d: NULL\n", i);
        }
    }

    for (i = count; i < capacity; i++) {
        printf("Index %d: NULL (unused)\n", i);
    }
}

static int INTRAFONT_load(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1 && args != 2)
        return luaL_error(L, "intraFont.load(path, [size]) takes 1 or 2 argument");

    const char *path = luaL_checkstring(L, 1);
    float size = luaL_optnumber(L, 2, 11.f);
    intraFont *font;

    size_t len = strlen(path);

    if (strcasecmp(path + len - 4, ".ttf") == 0 || strcasecmp(path + len - 4, ".otf") == 0) {
        font = intraFontLoadTTF(path, INTRAFONT_STRING_UTF8, size);
    } else if (strcasecmp(path + len - 4, ".pgf") == 0) {
        font = intraFontLoad(path, INTRAFONT_CACHE_ALL | INTRAFONT_STRING_UTF8);
    }

    if (!font || font == NULL)
        return luaL_error(L, "intraFont.load() error loading \"%s\"", path);

    checkCapacity((void ***)&loadedFonts, &fontCount, &fontCapacity, fontCount + 1);
    loadedFonts[fontCount++] = font;

    intraFont **fnt = pushintraFont(L);
    *fnt = font;

    printLoadedItems((void **)loadedFonts, fontCount, fontCapacity, "Fonts");

    return 1;
}

void INTRA_fontUnloadAll() {
    int i;
    for (i = fontCount - 1; i >= 0; i--) {
        if (loadedFonts[i] != NULL) {
            intraFontUnload(loadedFonts[i]);
            loadedFonts[i] = NULL;
        }
    }

    free(loadedFonts);
    loadedFonts = NULL;
    fontCapacity = 0;

    printLoadedItems((void **)loadedFonts, fontCount, fontCapacity, "Fonts");
}

intraFont *getFont(lua_State *L, int index) {
    if (lua_isnil(L, index))
        return luaFont;

    intraFont *font = *getintraFont(L, index);
    if (font == NULL)
        return luaFont;

    return font;
}

static int isFontLoaded(intraFont *font) {
    int i;
    for (i = 0; i < fontCount; i++) {
        if (loadedFonts[i] == font)
            return 1;
    }

    return 0;
}

static int INTRAFONT_width(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 2 && args != 3)
        return luaL_error(L, "intraFont.textW(font, text, size) takes 2 or 3 arguments");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1) : luaFont;
    if (!font || (font != luaFont && !isFontLoaded(font)))
        return luaL_error(L, "intraFont.textW() font not found");

    const char *text = luaL_checkstring(L, 2);
    float size = luaL_optnumber(L, 3, font->size);

    intraFontSetStyle(font, size, font->color, 0, 0.0f, 0);

    lua_pushnumber(L, intraFontMeasureText(font, text));

    return 1;
}

static int INTRAFONT_height(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.textH(font)");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1) : luaFont;
    if (!font || (font != luaFont && !isFontLoaded(font)))
        return luaL_error(L, "intraFont.textH() font not found");

    float rez = intraFontTextHeight(font);

    lua_pushnumber(L, rez);

    return 1;
}

static int INTRAFONT_setStyle(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 4 && args != 5)
        return luaL_error(L, "intraFont.setStyle(font, size, color, angle, [align])");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1) : luaFont;
    if (!font || !isFontLoaded(font))
        return luaL_error(L, "intraFont.setStyle() font not found");

    float size = luaL_checknumber(L, 2);
    u32 color = *toColor(L, 3);
    float angle = luaL_checknumber(L, 4);
    int mode = luaL_optnumber(L, 5, INTRAFONT_ALIGN_LEFT);

    intraFontSetStyle(font, size, color, 0, angle, mode);

    return 0;
}

static int INTRAFONT_size(lua_State *L) {
    if (lua_gettop(L) != 2)
        return luaL_error(L, "intraFont.size(font, size) takes 2 arguments");

    intraFont *font = (lua_gettop(L) >= 1) ? getFont(L, 1) : luaFont;
    if (!font || !isFontLoaded(font))
        return luaL_error(L, "intraFont.size() font not found");

    font->size = luaL_checknumber(L, 2);

    lua_pushnumber(L, font->size);

    return 1;
}

static int INTRAFONT_print(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 9)
        return luaL_error(L, "intraFont.print(x, y, text, [textColor], [font], [size], [textAngle], [GU_LINEAR], [AlMode]) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 color = (args >= 4) ? *toColor(L, 4) : WHITE;
    intraFont *font = (args >= 5) ? getFont(L, 5) : luaFont;
    float size = luaL_optnumber(L, 6, 1);
    float angle = luaL_optnumber(L, 7, 0);
    bool linear = (lua_toboolean(L, 8)) ? true : false;
    float alMode = luaL_optnumber(L, 9, INTRAFONT_ALIGN_LEFT);

    intraFontSetStyle(font, size, color, 0, angle, alMode);

    intraFontActivate(font, linear);

    lua_pushnumber(L, intraFontPrint(font, x, y + intraFontTextHeight(font), text));

    return 1;
}

static int INTRAFONT_printColumn(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 4 || args > 10)
        return luaL_error(L, "intraFont.printColumn(x, y, text, width, [textColor], [font], [size], [align], [GU_LINEAR] [scroll]) takes 4, 5, 6, 7, 8, 9 or 10 arguments");

    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    float width = luaL_checknumber(L, 4);
    u32 color = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float size = luaL_optnumber(L, 7, 1.f);
    int align = luaL_optnumber(L, 8, INTRAFONT_ALIGN_LEFT);
    bool linear = (lua_toboolean(L, 9)) ? true : false;
    int scroll = luaL_optnumber(L, 10, INTRAFONT_ALIGN_LEFT);

    intraFontSetStyle(font, size, color, 0, 0.0f, align | scroll);

    intraFontActivate(font, linear);

    lua_pushnumber(L, intraFontPrintColumn(font, x, y + intraFontTextHeight(font), width, text));

    return 1;
}

static int INTRAFONT_printUnderlined(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 8)
        return luaL_error(L, "intraFont.printUnderlined(x, y, text, [textColor], [lineColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 lineColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;

    lua_pushnumber(L, UnderLinedText(x, y + intraFontTextHeight(font), font, text, textSize, textColor, lineColor, linear));
    return 1;
}

static int INTRAFONT_printContoured(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 9)
        return luaL_error(L, "intraFont.printContoured(x, y, text, [textColor], [contourColor], [font], [textSize], [textAngle], [GU_LINEAR]) takes 3, 4, 5, 6, 7, 8 or 9 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 cColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    float textAngle = luaL_optnumber(L, 8, 0);
    bool linear = (lua_toboolean(L, 9)) ? true : false;

    lua_pushnumber(L, ContouredText(x, y + intraFontTextHeight(font), font, text, textSize, textAngle, textColor, cColor, INTRAFONT_ALIGN_LEFT, linear));

    return 1;
}

static int INTRAFONT_printBackground(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 8)
        return luaL_error(L, "intraFont.printBackground(x, y, text, [textColor], [bgColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : BLACK;
    u32 bgColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;

    lua_pushnumber(L, BackgroundColorText(x, y + intraFontTextHeight(font), font, text, textSize, textColor, bgColor, INTRAFONT_ALIGN_LEFT, linear));

    return 1;
}

static int INTRAFONT_printStriked(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 8)
        return luaL_error(L, "intraFont.printStriked(x, y, text, [textColor], [lineColor], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 lineColor = (args >= 5) ? *toColor(L, 5) : WHITE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;

    lua_pushnumber(L, StrikedText(x, y + intraFontTextHeight(font), font, text, textSize, textColor, lineColor, linear));

    return 1;
}

static int INTRAFONT_printShadowed(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 11)
        return luaL_error(L, "intraFont.printShadowed(x, y, text, [textColor], [shadowColor], [font], [shadowAngle], [lightDistance], [textSize], [textAngle], [GU_LINEAR]) takes 3, 4, 5, 6, 7, 8, 9, 10 or 11 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor = (args >= 4) ? *toColor(L, 4) : WHITE;
    u32 shadowColor = (args >= 5) ? *toColor(L, 5) : BLACK;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    int shadowAngle = luaL_optnumber(L, 7, 45);
    int dist = luaL_optnumber(L, 8, 2);
    float textSize = luaL_optnumber(L, 9, 1);
    int textAngle = luaL_optnumber(L, 10, 0);
    bool linear = (lua_toboolean(L, 11)) ? true : false;

    lua_pushnumber(L, ShadowedText(x, y + intraFontTextHeight(font), font, text, textSize, textAngle, shadowAngle, dist, textColor, shadowColor, linear));

    return 1;
}

static int INTRAFONT_printReversed(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.reverseText(text) takes 1 argument");

    const char *text = luaL_checkstring(L, 1);

    char *reversedText = InversedText(text);

    if (reversedText == NULL)
        return luaL_error(L, "Error processing the input text.");

    lua_pushstring(L, reversedText);
    free(reversedText);
    return 1;
}

static int INTRAFONT_printGradient(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 8)
        return luaL_error(L, "intraFont.printGradient(x, y, text, [textColorStart], [textColorEnd], [font], [textSize], [GU_LINEAR]) takes 3, 4, 5, 6, 7 or 8 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    const char *text = luaL_checkstring(L, 3);
    u32 textColor1 = (args >= 4) ? *toColor(L, 4) : RED;
    u32 textColor2 = (args >= 5) ? *toColor(L, 5) : BLUE;
    intraFont *font = (args >= 6) ? getFont(L, 6) : luaFont;
    float textSize = luaL_optnumber(L, 7, 1);
    bool linear = (lua_toboolean(L, 8)) ? true : false;

    GradientText(x, y, font, text, textColor1, textColor2, textSize, linear);

    return 0;
}

static int INTRAFONT_unload(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "intraFont.unload(font) takes 1 argument");

    intraFont *font = *getintraFont(L, 1);

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

int INTRAFONT_init(lua_State *L) {
    intraFontInit();

    UserdataRegister("intraFont", INTRAFONT_methods, INTRAFONT_metamethods);

    lua_pushstring(L, "intraFont");
    lua_gettable(L, LUA_GLOBALSINDEX);

    Const("ALIGN_LEFT", INTRAFONT_ALIGN_LEFT)
        Const("ALIGN_CENTER", INTRAFONT_ALIGN_CENTER)
        Const("ALIGN_RIGHT", INTRAFONT_ALIGN_RIGHT)
        Const("SCROLL_LEFT", INTRAFONT_SCROLL_LEFT)
        Const("SCROLL_RIGHT", INTRAFONT_SCROLL_RIGHT)
        Const("SCROLL_SEESAW", INTRAFONT_SCROLL_SEESAW)
        Const("SCROLL_THROUGH", INTRAFONT_SCROLL_THROUGH)

        return 1;
}

/******************************************************************
*****************************Image*********************************
******************************************************************/

UserdataStubs(G2D, g2dImage *)
//UserdataConvert(G2D, g2dImage*, "Image")

void G2D_RECT(int x, int y, int w, int h, int rotation, int alpha, g2dColor color, int AlMode) {
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

void G2D_CIRCLE(int cx, int cy, float radius, u32 color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        G2D_RECT(cx + x, cy + y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - x, cy + y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + x, cy - y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - x, cy - y, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + y, cy + x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - y, cy + x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx + y, cy - x, 1, 1, 0, 255, color, G2D_CENTER);
        G2D_RECT(cx - y, cy - x, 1, 1, 0, 255, color, G2D_CENTER);

        if (d < 0)
            d = d + 4 * x + 6;
        else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void G2D_TRIANGLE(int x1, int y1, int x2, int y2, int x3, int y3, u32 color) {
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

void G2D_CUBE(int x, int y, int z, int size, u32 color) {
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

static int G2D_clear(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 0 && args != 1)
        return luaL_error(L, "screen.clear([color]) takes 0 or 1 argument");

    u32 color = ((args == 1) && !lua_isnil(L, 1)) ? *toColor(L, 1) : BLACK;

    g2dClear(color);

    return 0;
}

static int G2D_flip(lua_State *L) {
    if (lua_gettop(L) != 0)
        return luaL_error(L, "screen.flip() takes no arguments");

    g2dFlip(G2D_VSYNC);

    return 0;
}

g2dImage **loadedImages = NULL;
int imageCount = 0;
int capacity = 0;

int removeLoadedImage(g2dImage *tex) {
    int i, j;
    for (i = 0; i < imageCount; i++) {
        if (loadedImages[i] == tex) {
            g2dTexFree(&tex);

            if (i < imageCount - 1) {
                for (j = i; j < imageCount - 1; j++)
                    loadedImages[j] = loadedImages[j + 1];
            }

            loadedImages[--imageCount] = NULL;

            if (capacity > 1) {
                capacity--; // Уменьшаем емкость
                loadedImages = realloc(loadedImages, capacity * sizeof(g2dImage *));
                if (!loadedImages && imageCount > 0) {
                    printf("Failed to reallocate memory");
                    sceKernelExitGame();
                }
            } else if (capacity == 1) {
                free(loadedImages);
                loadedImages = NULL;
                capacity = 0;
            }
            return 1;
        }
    }

    return 0;
}

static int G2D_texLoad(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "Image.load(path) takes 1 argument");

    char *path = (char *)luaL_checkstring(L, 1);
    g2dImage *img = g2dTexLoad(path, NULL, 0, G2D_VOID);
    if (!img)
        return luaL_error(L, "Image.load() error loading \"%s\"", path);

    checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
    loadedImages[imageCount++] = img;

    g2dImage **image = pushG2D(L);
    *image = img;

    return 1;
}

static int G2D_createPlaceholder(lua_State *L) {
    if (lua_gettop(L) != 0)
        return luaL_error(L, "Image.createPlaceholder() takes no arguments");

    g2dImage *img = g2dTexCreatePlaceholder();
    if (!img)
        return luaL_error(L, "Image.createPlaceholder() error");

    checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
    loadedImages[imageCount++] = img;

    g2dImage **image = pushG2D(L);
    *image = img;

    return 1;
}

static int G2D_texUnload(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "Image.unload(texture) takes 1 arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, "Image.unload() can't get the texture");

    if (removeLoadedImage(img)) {
        printLoadedItems((void **)loadedImages, imageCount, capacity, "Images");
        return 0;
    }

    return luaL_error(L, "Image not found in loaded images");
}

static int G2D_texToString(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "Image.unload(texture) takes 1 arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img)
        return luaL_error(L, "Image.unload() can't get the texture");

    lua_Number size = (img->tw * img->th * 4.0f) / 1024.0f;
    lua_pushfstring(L, "Image(%dx%d, %f Kb)", img->w, img->h, size);

    return 1;
}

void G2D_texUnloadAll() {
    int i;
    for (i = imageCount - 1; i >= 0; i--) {
        if (loadedImages[i] != NULL) {
            g2dTexFree(&loadedImages[i]);
            loadedImages[i] = NULL;
        }
    }

    imageCount = 0;

    if (capacity > 1) {
        loadedImages = realloc(loadedImages, sizeof(g2dImage *));
        if (loadedImages == NULL) {
            printf("Failed to reallocate memory");
            sceKernelExitGame();
        }
        capacity = 1;
    }

    printLoadedItems((void **)loadedImages, imageCount, capacity, "Images");
}

static int isImageLoaded(g2dImage *img) {
    int i;
    for (i = 0; i < imageCount; i++) {
        if (loadedImages[i] == img)
            return 1;
    }
    return 0;
}

static int G2D_getW(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "Image.W(texture) takes 1 argument");

    g2dImage *img = *toG2D(L, 1);
    if (!img || !isImageLoaded(img))
        return luaL_error(L, "Image.W() can't get the texture");

    lua_pushinteger(L, img->w);

    return 1;
}

static int G2D_getH(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "Image.H(texture) takes 1 argument");

    g2dImage *img = *toG2D(L, 1);
    if (!img || !isImageLoaded(img))
        return luaL_error(L, "Image.H() can't get the texture");

    lua_pushinteger(L, img->h);

    return 1;
}

static int G2D_draw(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 17 || (args == 4 && luaL_checknumber(L, 4) <= 0))
        return luaL_error(L, "Image.draw(texture, x, y, [width], [height], [color], [srcx], [srcy], [srcw], [srch], [rotation], [alpha], [origin_x], [origin_y], [GU_LINEAR], [GU_REPEAT], [BLEND]) takes 3, 5, 6, 10, 11, 12, 13, 14, 15, 16 or 17 arguments");

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
    float origin_x = luaL_optnumber(L, 13, 0);
    float origin_y = luaL_optnumber(L, 14, 0);
    bool linear = (lua_toboolean(L, 15)) ? true : false;
    bool repeat = (lua_toboolean(L, 16)) ? true : false;
    int blend = luaL_optnumber(L, 17, -1);

    g2dBeginRects(img);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(repeat);
    g2dSetCoordXY(x, y);
    g2dSetCropXY(srcx, srcy);
    g2dSetCropWH(srcw, srch);
    g2dSetScaleWH(w, h);
    g2dSetRotation(Angle);
    if (color != 0)
        g2dSetColor(color);
    g2dSetAlpha(a);
    if (blend != -1)
        g2dSetTexBlendMode(blend);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int G2D_draweasy(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 3 || args > 10)
        return luaL_error(L, "Image.draweasy(texture, x, y, [color], [rotation], [alpha], [origin_x], [origin_y], [GU_LINEAR], [GU_REPEAT]) takes 3, 4, 5, 6, 7, 8, 9 or 10 arguments");

    g2dImage *img = *toG2D(L, 1);
    if (!img || !isImageLoaded(img))
        return luaL_error(L, "Image.draweasy() can't get the texture");

    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    u32 color = (args >= 4 && !lua_isnil(L, 4)) ? *toColor(L, 4) : 0;
    float Angle = luaL_optnumber(L, 5, 0.0f);
    int a = luaL_optnumber(L, 6, 255);
    float origin_x = luaL_optnumber(L, 7, 0);
    float origin_y = luaL_optnumber(L, 8, 0);
    bool linear = (lua_toboolean(L, 9)) ? true : false;
    bool repeat = (lua_toboolean(L, 10)) ? true : false;

    g2dBeginRects(img);
    g2dSetOriginXY(origin_x, origin_y);
    g2dSetTexLinear(linear);
    g2dSetTexRepeat(repeat);
    g2dSetCoordXY(x, y);
    g2dSetRotation(Angle);
    if (color != 0)
        g2dSetColor(color);
    g2dSetAlpha(a);
    g2dAdd();
    g2dEnd();

    return 0;
}

static int G2D_filledRect(lua_State *L) {
    int args = lua_gettop(L);
    if (args < 5 || args > 8)
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

static int G2D_drawCircle(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 4)
        return luaL_error(L, "screen.drawCircle(x, y, radius, color) takes 4 arguments");

    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    float r = luaL_checknumber(L, 3);
    u32 color = *toColor(L, 4);

    G2D_CIRCLE(x, y, r, color);

    return 0;
}

static int G2D_drawTriangle(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 7)
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

static int G2D_drawLine(lua_State *L) {
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

static int G2D_drawCube(lua_State *L) {
    G2D_CUBE(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), *toColor(L, 5));

    return 0;
}

static int G2D_drawCircleOnTex(lua_State *L) {
    g2dImage *tex = *toG2D(L, 1);
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int r = luaL_checknumber(L, 4);
    u32 color = *toColor(L, 5);

    draw_circle(tex, x, y, r, r, color);
    return 0;
}

static int G2D_drawRectangleColor(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 10)
        return luaL_error(L, "screen.drawRectangleColor(x1, y1, x2, y2, col1, col2, col3, col4, outline, BLEND) takes 10 arguments");

    int x1 = luaL_checknumber(L, 1);
    int y1 = luaL_checknumber(L, 2);
    int x2 = luaL_checknumber(L, 3);
    int y2 = luaL_checknumber(L, 4);
    u32 col1 = *toColor(L, 5);
    u32 col2 = *toColor(L, 6);
    u32 col3 = *toColor(L, 7);
    u32 col4 = *toColor(L, 8);
    bool outline = (lua_toboolean(L, 9)) ? true : false;
    int blend = luaL_optnumber(L, 10, -1);
    int rotation = 0;
    int alpha = 255;

    if (outline)
        return luaL_error(L, "screen.drawRectangleColor(...) - outline is not implemented xd");


    g2dBeginQuads(NULL);
    if (blend != -1)
        g2dSetTexBlendMode(blend);

    g2dSetCoordXY(x1, y1);    g2dSetColor(col1);    g2dAdd(); // TL
    g2dSetCoordXY(x2, y1);    g2dSetColor(col2);    g2dAdd(); // TR
    g2dSetCoordXY(x2, y2);    g2dSetColor(col3);    g2dAdd(); // BR
    g2dSetCoordXY(x1, y2);    g2dSetColor(col4);    g2dAdd(); // BL

    g2dEnd();

    return 0;
}

static int G2D_getCalls(lua_State *L) {
    lua_pushnumber(L, g2dDebugGetAdds());
    lua_pushnumber(L, g2dDebugGetEnds());

    return 2;
}

static int G2D_setCamera(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1 && args != 3)
        return luaL_error(L, "G2D_setCamera(use, [x], [y]) takes 1 or 3 arguments");

    g2dSetUseCamera(lua_toboolean(L, 1) != 0);

    if (args >= 3) {
        int x = luaL_checkint(L, 2);
        int y = luaL_checkint(L, 3);
        g2dSetCameraXY(luaL_checkint(L, 2), luaL_checkint(L, 3));
    }

    return 0;
}

static int G2D_getCamera(lua_State *L) {
    lua_pushnumber(L, g2dGetCameraX());
    lua_pushnumber(L, g2dGetCameraY());

    return 2;
}

static const luaL_Reg GFX_methods[] = {
    {"clear",               G2D_clear},
    {"flip",                G2D_flip},
    {"filledRect",          G2D_filledRect},
    {"drawLine",            G2D_drawLine},
    {"drawCircle",          G2D_drawCircle},
    {"drawTriangle",        G2D_drawTriangle},
    {"drawRectangleColor",  G2D_drawRectangleColor},
    {"getCalls",            G2D_getCalls},
    {"setCamera",           G2D_setCamera},
    {"getCamera",           G2D_getCamera},
    //{"drawCube",     G2D_drawCube},
    {0, 0}
};

static const luaL_Reg GFX_methods2[] = {
    {"load",                G2D_texLoad},
    {"W",                   G2D_getW},
    {"H",                   G2D_getH},
    {"draw",                G2D_draw},
    {"draweasy",            G2D_draweasy},
    {"unload",              G2D_texUnload},
    {"drawCircleOnTex",     G2D_drawCircleOnTex},
    {"createPlaceholder",   G2D_createPlaceholder},
    {0, 0}
};

static const luaL_Reg GFX_metamethods[] = {
    // {"__gc", G2D_texUnload},
    {0, 0}
};

int GRAPHICS_init(lua_State *L) {
    g2dInit();

    luaL_newmetatable(L, "G2D");
    lua_pushcfunction(L, G2D_texToString);
    lua_setfield(L, -2, "__tostring");
    lua_pop(L, 1);

    luaL_register(L, "screen", GFX_methods);
    //luaL_register(L, "Image", GFX_methods2);
    UserdataRegister("Image", GFX_methods2, GFX_metamethods);

    lua_pushstring(L, "Image");
    lua_gettable(L, LUA_GLOBALSINDEX);

    Const("Center", G2D_CENTER)
        Const("lUp", G2D_UP_LEFT)
        Const("lDn", G2D_DOWN_LEFT)
        Const("rUp", G2D_UP_RIGHT)
        Const("rDn", G2D_DOWN_RIGHT)

        return 1;
}