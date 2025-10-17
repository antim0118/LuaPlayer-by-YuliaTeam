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

#include "vfpu_math.h"
#include "../libs/vfpu_math.h"

static int MATH_ABSF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.abs(number) takes 1 argument");

    lua_pushnumber(L, vfpu_absf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_ACOSF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.acos(number) takes 1 argument");

    lua_pushnumber(L, vfpu_acosf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_ASINF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.asin(number) takes 1 argument");

    lua_pushnumber(L, vfpu_asinf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_ATAN2F(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "math.atan2(x, y) takes 2 arguments");

    lua_pushnumber(L, vfpu_atan2f((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2)));

    return 1;
}

static int MATH_ATANF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.atan(number) takes 1 argument");

    lua_pushnumber(L, vfpu_atanf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_CEILF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.ceil(number) takes 1 argument");

    lua_pushnumber(L, vfpu_ceilf((float)lua_tonumber(L, 1)));

    return 1;
} 

static int MATH_COSHF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.cosh(number) takes 1 argument");

    lua_pushnumber(L, vfpu_coshf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_COSF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.cos(number) takes 1 argument");

    lua_pushnumber(L, vfpu_cosf((float)lua_tonumber(L, 1)));

    return 1;
} 

static int MATH_DEGF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.deg(radians) takes 1 argument");

    lua_pushnumber(L, vfpu_degf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_EXPF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.exp(number) takes 1 argument");

    lua_pushnumber(L, vfpu_expf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_FLOORF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.floor(number) takes 1 argument");

    lua_pushnumber(L, vfpu_floorf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_FMODF(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "math.fmod(x, y) takes 2 arguments");

    lua_pushnumber(L, vfpu_fmodf((float)lua_tonumber(L, 1), (float)lua_tonumber(L,2)));

    return 1;
}

static int MATH_FREXPF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.frexp(x) takes 1 argument");

    int exp;
    float mantissa = vfpu_frexpf((float)lua_tonumber(L, 1), &exp);

    lua_pushnumber(L, mantissa);
    lua_pushinteger(L, exp);

    return 2;
}

static int MATH_LDEXPF(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "math.ldexp(x, exp) takes 2 arguments");

    lua_pushnumber(L, vfpu_ldexpf((float)lua_tonumber(L, 1), (int)lua_tonumber(L, 2)));

    return 1;
}

static int MATH_LOG10F(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.log10(number) takes 1 argument");

    lua_pushnumber(L, vfpu_log10f((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_LOGF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.log(number) takes 1 argument");

    lua_pushnumber(L, vfpu_logf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_MAXF(lua_State *L)
{
    int n = lua_gettop(L);  /* number of arguments */
    lua_Number dmax = luaL_checknumber(L, 1);

    int i;
    for (i=2; i<=n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d > dmax)
        dmax = d;
    }

    lua_pushnumber(L, dmax);
    return 1;
}

static int MATH_MINF(lua_State *L)
{
    int n = lua_gettop(L);  /* number of arguments */
    lua_Number dmin = luaL_checknumber(L, 1);

    int i;
    for (i=2; i<=n; i++) {
        lua_Number d = luaL_checknumber(L, i);
        if (d < dmin)
        dmin = d;
    }

    lua_pushnumber(L, dmin);
    return 1;
}

static int MATH_MODFF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.modf(x) takes 1 argument");

    float intpart;
    float fractpart = vfpu_modff(lua_tonumber(L, 1), &intpart);

    lua_pushnumber(L, intpart);
    lua_pushnumber(L, fractpart);
    
    return 2;
}

static int MATH_POWF(lua_State *L)
{
    if (lua_gettop(L) != 2)
        return luaL_error(L, "math.pow(x, y) takes 2 arguments");

    lua_pushnumber(L, vfpu_powf((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2)));

    return 1;
}

static int MATH_RADF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.rad(degrees) takes 1 argument");

    lua_pushnumber(L, vfpu_radf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_RANDOM(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "math.random() takes no arguments");

    lua_pushnumber(L, vfpu_random());

    return 1;
}

static int MATH_RANDOMSEED(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.randomseed(seed) takes 1 argument");

    vfpu_randomseed((unsigned int)lua_tonumber(L, 1));

    return 0;
}

static int MATH_SINHF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.sinh(x) takes 1 argument");

    lua_pushnumber(L, vfpu_sinhf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_SINF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.sin(x) takes 1 argument");

    lua_pushnumber(L, vfpu_sinf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_SQRTF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.sqrt(x) takes 1 argument");

    lua_pushnumber(L, vfpu_sqrtf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_TANHF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.tanh(x) takes 1 argument");

    lua_pushnumber(L, vfpu_tanhf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_TANF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.tan(x) takes 1 argument");

    lua_pushnumber(L, vfpu_tanf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_COTF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.cot(x) takes 1 argument");

    lua_pushnumber(L, vfpu_cotf((float)lua_tonumber(L, 1)));

    return 1;
}

static int MATH_COTHF(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "math.coth(x) takes 1 argument");

    lua_pushnumber(L, vfpu_cothf((float)lua_tonumber(L, 1)));

    return 1;
}

static const luaL_Reg VFPU_methods[] = {
    {"abs", MATH_ABSF},
    {"acos", MATH_ACOSF},
    {"asin", MATH_ASINF},
    {"atan2", MATH_ATAN2F},
    {"atan", MATH_ATANF},
    {"ceil", MATH_CEILF},
    {"cosh", MATH_COSHF},
    {"cos", MATH_COSF},
    {"deg", MATH_DEGF},
    {"exp", MATH_EXPF},
    {"floor", MATH_FLOORF},
    {"fmod", MATH_FMODF},
    {"frexp", MATH_FREXPF},
    {"ldexp", MATH_LDEXPF},
    {"log10", MATH_LOG10F},
    {"log", MATH_LOGF},
    {"max", MATH_MAXF},
    {"min", MATH_MINF},
    {"modf", MATH_MODFF},
    {"pow", MATH_POWF},
    {"rad", MATH_RADF},
    {"random", MATH_RANDOM},
    {"randomseed", MATH_RANDOMSEED},
    {"sinh", MATH_SINHF},
    {"sin", MATH_SINF},
    {"sqrt", MATH_SQRTF},
    {"tanh", MATH_TANHF},
    {"tan", MATH_TANF},
    {"cot", MATH_COTF},
    {"coth", MATH_COTHF},
    {0,0}
};

int VFPU_init(lua_State *L)
{
    luaL_register(L, "math", VFPU_methods);

    return 1;
}