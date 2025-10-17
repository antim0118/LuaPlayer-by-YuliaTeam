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

#include "particles.h"
#include "timer.h"

typedef struct Coord {
    float pos[2];
    float pos0[2];
    float size[2];
    int angle;
} Coord;

typedef struct Particle {
    const char* type;
    const char* name;
    int alpha;
    int visible;
    C_timer* time;
    g2dImage *image;
    Coord *coords;
    int coords_length;
} Particle;

enum ParticleType {
    SNOWFALL,
    LEAFS,
    RAIN,
    UNKNOWN
};

#define IMAGE_WIDTH 480
#define IMAGE_HEIGHT 272
#define MAX_PARTICLES 100
#define RAIN_ANGLE 30

char OVER_PARTICLING[512];

Particle Particles[MAX_PARTICLES];

int num_particles = 0;

static int getParticleType(const char* type) {
    if (strcmp(type, "snowfall") == 0) return SNOWFALL;
    if (strcmp(type, "leafs") == 0) return LEAFS;
    if (strcmp(type, "rain") == 0) return RAIN;
    return UNKNOWN;
}

void PARTICLE_DRAW(Particle* particle)
{
    int c;
    
    for (c = 0; c < particle->coords_length; c++)
    {
        if (TIMER_TIME(particle->time) > 1.0/100 && strcmp(particle->type, "rain") == 0)
        {
            particle->coords[c].pos[0] = particle->coords[c].pos[0] - 40;
            particle->coords[c].pos[1] = particle->coords[c].pos[1] + 40;
        }
        else if (TIMER_TIME(particle->time) > 30 && strcmp(particle->type, "rain") != 0)
        {
            particle->coords[c].pos[1] = particle->coords[c].pos[1] + 1;
            particle->coords[c].angle = particle->coords[c].angle - (rand() % 6);
        }

        if (TIMER_TIME(particle->time) > 60)
        {
            if (strcmp(particle->type, "snowfall") == 0)
            {
                particle->coords[c].pos[0] = particle->coords[c].pos[0] + (rand() % 7 - 3) * 0.1;
            }
            else if (strcmp(particle->type, "leafs") == 0)
            {
                particle->coords[c].pos[0] = particle->coords[c].pos[0] + (rand() % 4 + 3) * 0.6;
            }

            if (particle->coords[c].pos[1] > 272 + particle->image->h)
            {
                if (strcmp(particle->type, "snowfall") == 0)
                {
                    particle->coords[c].pos[0] = rand() % 480;
                    particle->coords[c].pos[1] = particle->coords[c].pos0[1];
                    particle->coords[c].angle = 0;
                }
                else if (strcmp(particle->type, "leafs") == 0)
                {
                    particle->coords[c].pos[0] = rand() % 720 - 240;
                    particle->coords[c].pos[1] = particle->coords[c].pos0[1];
                    particle->coords[c].angle = 0;
                }
                else if (strcmp(particle->type, "rain") == 0)
                {
                    particle->coords[c].pos[0] = rand() % 600 + 180;
                    particle->coords[c].pos[1] = particle->coords[c].pos0[1];
                }
            }

            if (c == particle->coords_length - 1)
            {
                TIMER_RESET(particle->time);
                TIMER_START(particle->time);
            }
        }

        g2dBeginRects(particle->image);
        g2dSetCoordMode(G2D_CENTER);
        g2dSetCoordXY(particle->coords[c].pos[0], particle->coords[c].pos[1]);
        g2dSetScaleWH(particle->coords[c].size[0] * particle->image->w, particle->coords[c].size[1] * particle->image->h);
        g2dSetRotation(particle->coords[c].angle);
        g2dAdd();
        g2dEnd();
    }
}

void PARTICLE_UPDATE(void)
{
    int i;

    for (i = 0; i < num_particles; i++)
    {
        if (Particles[i].visible == 1)
            PARTICLE_DRAW(&Particles[i]);
    }
}

char* PARTICLE_NEW(Particle* new_particle, const char *path, const char *name, const char* type, int rain_angle, int multiplier)
{
    int i;
    
    if (num_particles < MAX_PARTICLES)
    {
        for (i = 0; i < MAX_PARTICLES; i++)
        {
            if ((Particles[i].name != NULL) && (strcmp(name, Particles[i].name) == 0))
            {
                sprintf(OVER_PARTICLING, "Particle named \"%s\" already added", name);
                return OVER_PARTICLING;
            }
        }

        new_particle->type = type;
        new_particle->name = name;
        new_particle->alpha = 255;
        new_particle->visible = 1;
        new_particle->time = TIMER_CREATE();
        new_particle->image = g2dTexLoad((char*)path, G2D_VOID);
        new_particle->coords = (Coord *)malloc(multiplier * sizeof(Coord));
        new_particle->coords_length = multiplier;
        
        int prtclType = getParticleType(type);

        for (i = 0; i < multiplier; i++)
        {
            switch (prtclType)
            {
                case SNOWFALL:
                    new_particle->coords[i].pos[0] = (float)(rand() % IMAGE_WIDTH);
                    new_particle->coords[i].pos[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].pos0[0] = (float)(rand() % IMAGE_WIDTH);
                    new_particle->coords[i].pos0[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].size[0] = (float)(rand() % 20 + 1) * 0.1;
                    new_particle->coords[i].size[1] = new_particle->coords[i].size[0];
                    new_particle->coords[i].angle = (float)(rand() % 360);
                    break;
                case LEAFS:
                    new_particle->coords[i].pos[0] = (float)(rand() % (IMAGE_WIDTH + 240) - 240);
                    new_particle->coords[i].pos[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].pos0[0] = (float)(rand() % (IMAGE_WIDTH + 240) - 240);
                    new_particle->coords[i].pos0[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].size[0] = (float)(rand() % 10 + 4) * 0.1;
                    new_particle->coords[i].size[1] = new_particle->coords[i].size[0];
                    new_particle->coords[i].angle = (float)(rand() % 360);
                    break;
                case RAIN:
                    new_particle->coords[i].pos[0] = (float)(rand() % (IMAGE_WIDTH + 600) - 600);
                    new_particle->coords[i].pos[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].pos0[0] = (float)(rand() % (IMAGE_WIDTH + 600) - 600);
                    new_particle->coords[i].pos0[1] = -(new_particle->image->h * (float)(rand() % 30));
                    new_particle->coords[i].size[0] = (float)(rand() % 12 + 4) * 0.1;
                    new_particle->coords[i].size[1] = (float)(rand() % 16 + 4) * 0.1;
                    new_particle->coords[i].angle = rain_angle;
                    break;
                default:
                    break;
            }
        }

        num_particles++;

        return (char*)"Success";
    }
    else
        return (char*)"Not enough space to add a new particle";
}

void PARTICLE_DELETE(const char *name)
{
    int i, j;

    for (i = 0; i < MAX_PARTICLES; i++)
    {
        if ((Particles[i].name != NULL) && (strcmp(name, Particles[i].name) == 0))
        {
            Particles[i].visible = 0;
            Particles[i].name = NULL;

            if (Particles[i].image)
                g2dTexFree(&Particles[i].image);

            if (Particles[i].time)
                TIMER_REMOVE(&Particles[i].time);

            if (Particles[i].coords)
            {    
                free(Particles[i].coords);
                Particles[i].coords = NULL;
            }

            if (num_particles > 1)
            {
                for (j = i; j < MAX_PARTICLES - 1; j++)
                {
                    Particles[j].visible = 0;
                    Particles[j].name = NULL;

                    if (Particles[j].image)
                        g2dTexFree(&Particles[j].image);

                    if (Particles[j].time)
                        TIMER_REMOVE(&Particles[j].time);

                    if (Particles[j].coords)
                    {
                        free(Particles[j].coords);
                        Particles[j].coords = NULL;
                    }

                    Particles[j] = Particles[j + 1];
                }
            }

            num_particles--;

            break;
        }
    }
}

static int luaPARTICLE_UPDATE(lua_State *L)
{
    if (lua_gettop(L) != 0)
        return luaL_error(L, "particles.update() takes no arguments");

    PARTICLE_UPDATE();

    return 0;
}

static int luaPARTICLE_NEW(lua_State *L)
{
    if (lua_gettop(L) != 4 && lua_gettop(L) != 5)
        return luaL_error(L, "particles.new(path, name, type, multiplier, [rain_angle]) takes 4 or 5 arguments");

    const char* path = luaL_checkstring(L, 1);
    const char* name = luaL_checkstring(L, 2);
    const char* type = luaL_checkstring(L, 3);
    int multiplier = luaL_checknumber(L, 4);
    int rain_angle = luaL_optnumber(L, 5, RAIN_ANGLE);

    const char* rez = PARTICLE_NEW(&Particles[num_particles], path, name, type, rain_angle, multiplier);

    if (strcmp(rez, "Success") != 0)
        return luaL_error(L, rez);

    return 0;
}

static int luaPARTICLE_DELETE(lua_State *L)
{
    if (lua_gettop(L) != 1)
        return luaL_error(L, "particles.delete(name) takes 1 argument");

    const char* name = luaL_checkstring(L, 1);

    PARTICLE_DELETE(name);

    return 0;
}

static int luaPARTICLE_GET(lua_State *L)
{
    if (Particles[0].time)
    {
        lua_newtable(L);
        lua_pushnumber(L, TIMER_TIME(Particles[0].time));
        lua_setfield(L, -2, "[1]");

        lua_pushnumber(L, Particles[0].coords->angle);
        lua_setfield(L, -2, "[2]");

        lua_pushstring(L, Particles[0].type);
        lua_setfield(L, -2, "[3]");
    }

    return 1;
}

static const luaL_Reg PARTICLE_methods[] = {
    {"update", luaPARTICLE_UPDATE},
    {"new",    luaPARTICLE_NEW},
    {"delete", luaPARTICLE_DELETE},
    {"get",    luaPARTICLE_GET},
    {0, 0}
};

int PARTICLE_init(lua_State *L)
{
    luaL_register(L, "particles", PARTICLE_methods);

    return 0;
}