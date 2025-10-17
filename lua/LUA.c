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

#include "LUA.h"
#include "../libs/include_res/output_wav.c"
#include "../libs/include_res/output_pgf.c"

intraFont *luaFont;

void save_to_file(const char* filename, unsigned char* data, int size)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Не удалось открыть файл для записи");
        return;
    }

    fwrite(data, sizeof(unsigned char), size, file);

    fclose(file);
}

int checkFileSum(const char *path, uint32_t size)
{
  FILE *f = fopen(path, "rb");
  if(f == NULL) return 1;
  fseek(f, 0, SEEK_END);
  long file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  uint8_t *file_data = malloc(file_size);
  fread(file_data, 1, file_size, f);
  
  uint32_t crc = 0xFFFFFFFF;
  const uint8_t *bytes = (const uint8_t *) file_data;
  size_t i, j;
  for (i = 0; i < file_size; i++) {
    crc ^= bytes[i];
    for (j = 0; j < 8; j++) {
      crc = (crc >> 1) ^ (CRC32_POLYNOMIAL & (-(crc & 1)));
    }
  }

  fclose(f);
  free(file_data);
  
  if(size == ~crc) return 0;
  else return 1;
}

static int LUA_exit(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "LUA.exit() takes no arguments");

    pmp_shutdown();
    sceKernelExitGame();
    return 0;
}

static int LUA_delay(lua_State *L)
{
    if(lua_gettop(L) != 1)
        return luaL_error(L, "LUA.sleep(ms) takes 1 argument");

    sceKernelDelayThreadCB(luaL_checknumber(L, 1)*1000);

    return 0;
}

int get_freeRam()
{
    void* buf[128];
    int i, j;

    for(i = 0; i < 128; i++)
    {
        buf[i] = malloc(512 * 1024);
        if(!buf[i])
            break;
    }
    
    int result = i;

    for(j = result - 1; j >= 0; j--)
    {
        free(buf[j]);
    }

    return (result * 512 * 1024);
}

static int LUA_freeRAM(lua_State *L)
{
    if(lua_gettop(L) != 0)
        return luaL_error(L, "LUA.getRAM() takes no arguments");

    lua_pushnumber(L, get_freeRam());

    return 1;
}

static int LUA_getRand(lua_State *L) {
    if (lua_gettop(L) != 2) 
        return luaL_error(L, "LUA.getRandom(min, max) takes 2 arguments");
    
    srand(time(NULL)); 
    int min = luaL_checknumber(L, 1);
    int max = luaL_checknumber(L, 2);

    int random = rand() % (max - min + 1) + min;
    lua_pushnumber(L, random);
    
    return 1;
}

/*static int LUA_screenshot(lua_State *L)
{
    if(lua_gettop(L) != 3)
        return luaL_error(L, "LUA.screenshot(path, width, height) takes 3 arguments");

    const char* path = luaL_checkstring(L, 1);
    int new_width = luaL_checkint(L, 2);
    int new_height = luaL_checkint(L, 3);

    png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_write_ptr)
        return luaL_error(L, "LUA.screenshot(path, width, height) error creating output file's structure");

    png_infop info_write_ptr = png_create_info_struct(png_write_ptr);
    if (!info_write_ptr) {
        png_destroy_write_struct(&png_write_ptr, NULL);
        return luaL_error(L, "LUA.screenshot(path, width, height) error creating output file's structure");
    }

    FILE *output_file = fopen(path, "wb");
    if (!output_file) {
        png_destroy_write_struct(&png_write_ptr, &info_write_ptr);
        return luaL_error(L, "LUA.screenshot(path, width, height) can't create/open output file");
    }

    png_init_io(png_write_ptr, output_file);
    png_set_IHDR(png_write_ptr, info_write_ptr, new_width, new_height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write_ptr, info_write_ptr);

    uint8_t *line = (uint8_t*)malloc(new_width * 3);

    int x, y;

    for (y = 0; y < new_height; y++) {
        for (x = 0; x < new_width; x++) {
            int x_original = x * g2d_disp_buffer.w / new_width;
            int y_original = y * g2d_disp_buffer.h / new_height;
            
            int x1 = x_original;
            int y1 = y_original;
            int x2 = x_original + 1 >= g2d_disp_buffer.w ? g2d_disp_buffer.w - 1 : x_original + 1;
            int y2 = y_original + 1 >= g2d_disp_buffer.h ? g2d_disp_buffer.h - 1 : y_original + 1;

            double x_ratio = (double)(x_original - x1) / (x2 - x1);
            double y_ratio = (double)(y_original - y1) / (y2 - y1);

            g2dColor color1 = g2d_disp_buffer.data[x1 + y1 * 512];
            g2dColor color2 = g2d_disp_buffer.data[x2 + y1 * 512];
            g2dColor color3 = g2d_disp_buffer.data[x1 + y2 * 512];
            g2dColor color4 = g2d_disp_buffer.data[x2 + y2 * 512];

            uint8_t r = (1 - x_ratio) * (1 - y_ratio) * G2D_GET_R(color1) + x_ratio * (1 - y_ratio) * G2D_GET_R(color2) +
                        (1 - x_ratio) * y_ratio * G2D_GET_R(color3) + x_ratio * y_ratio * G2D_GET_R(color4);
            uint8_t g = (1 - x_ratio) * (1 - y_ratio) * G2D_GET_G(color1) + x_ratio * (1 - y_ratio) * G2D_GET_G(color2) +
                        (1 - x_ratio) * y_ratio * G2D_GET_G(color3) + x_ratio * y_ratio * G2D_GET_G(color4);
            uint8_t b = (1 - x_ratio) * (1 - y_ratio) * G2D_GET_B(color1) + x_ratio * (1 - y_ratio) * G2D_GET_B(color2) +
                        (1 - x_ratio) * y_ratio * G2D_GET_B(color3) + x_ratio * y_ratio * G2D_GET_B(color4);

            line[x * 3] = r;
            line[x * 3 + 1] = g;
            line[x * 3 + 2] = b;
        }

        png_write_row(png_write_ptr, line);
    }

    free(line);
    png_write_end(png_write_ptr, info_write_ptr);
    png_destroy_write_struct(&png_write_ptr, &info_write_ptr);
    fclose(output_file);

    return 0;
}*/

static int LUA_screenshot(lua_State *L)
{
    if (lua_gettop(L) != 3)
        return luaL_error(L, "LUA.screenshot(path, width, height) takes 3 arguments");

    const char* path = luaL_checkstring(L, 1);
    int new_width = luaL_checkint(L, 2);
    int new_height = luaL_checkint(L, 3);
    
    FILE *file = fopen(path, "wb");
    if (!file)
        return luaL_error(L, "LUA.screenshot(path): Cannot create/open path file.");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) 
        return luaL_error(L, "LUA.screenshot error creating file's write structure.");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return luaL_error(L, "LUA.screenshot error creating file's info structure.");
    }
    
    png_init_io(png_ptr, file);
    png_set_IHDR(png_ptr, info_ptr, new_width, new_height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, 
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    uint8_t* line = (uint8_t*)malloc(new_width * 3);
    if (!line) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(file);
        return luaL_error(L, "Memory allocation failed");
    }

    int orig_width = g2d_disp_buffer.w;
    int orig_height = g2d_disp_buffer.h;

    int x, y;
    for (y = 0; y < new_height; y++) {
        int orig_y = (y * orig_height) / new_height;
        g2dColor *row = &g2d_disp_buffer.data[orig_y * 512];

        for (x = 0; x < new_width; x++) {
            int orig_x = (x * orig_width) / new_width;

            uint8_t r = G2D_GET_R(row[orig_x]);
            uint8_t g = G2D_GET_G(row[orig_x]);
            uint8_t b = G2D_GET_B(row[orig_x]);

            line[x * 3] = r;
            line[x * 3 + 1] = g;
            line[x * 3 + 2] = b;
        }
        png_write_row(png_ptr, line);
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    free(line);
    fclose(file);

    return 0;
}

static int LUA_print(lua_State *L)
{
    if(lua_gettop(L) != 3)
        return luaL_error(L, "LUA.print(x, y, text) takes 3 arguments");

    intraFontSetStyle(luaFont, 0.9, WHITE, 0, 0, 0);
    
    int x = luaL_checkint(L, 1);
    int y = luaL_checkint(L, 2) + intraFontTextHeight(luaFont);
    const char *text = luaL_checkstring(L, 3);
    
    lua_pushnumber(L, intraFontPrint(luaFont, x, y, text));
    
    return 1;
}

static const luaL_Reg LUA_methods[] = {
    {"exit",       LUA_exit},
    {"quit",       LUA_exit},
    {"sleep",      LUA_delay},
    {"getRAM",     LUA_freeRAM},
    {"getRandom",  LUA_getRand},
    {"screenshot", LUA_screenshot},
    {"print",      LUA_print},
    {0, 0}
};

int LUA_init(lua_State *L)
{
    save_to_file("templuaFont.pgf", LUAFont, size_LUAFont);
    save_to_file("temp_system_sound.wav", system_sound, size_system_sound);

    if(checkFileSum("templuaFont.pgf", 0x48437F2D) == 0 && checkFileSum("temp_system_sound.wav", 0xD78A8334) == 0)
    {
        luaFont = intraFontLoad("templuaFont.pgf", INTRAFONT_CACHE_LARGE | INTRAFONT_STRING_UTF8);
        if(AalibLoad("temp_system_sound.wav", PSPAALIB_CHANNEL_WAV_32, TRUE) != 0) sceKernelExitGame();
    
        remove("templuaFont.pgf");
        remove("temp_system_sound.wav");
    }
    else
    {
        remove("templuaFont.pgf");
        remove("temp_system_sound.wav");
        sceKernelExitGame();
    }
    
    luaL_register(L, "LUA", LUA_methods);
    
    return 0;
}