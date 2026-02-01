#include "pmp.h"
#include "../libs/pmp/pmp.h"
#include "../libs/pmp/pmp_play.h"
#include "../libs/include_res/subs_font.c"

extern g2dImage **toG2D(lua_State *L, int index);
extern g2dImage **pushG2D(lua_State *L);
// UserdataStubs(G2D, g2dImage *)

extern g2dImage **loadedImages;
extern int imageCount;
extern int capacity;

#define MAX_SUBS 256
#define MAX_TEXT_LENGTH 512

typedef struct
{
    char startTime[15];
    char endTime[15];
    char text[MAX_TEXT_LENGTH];
} Subtitle;

Subtitle subtitles[MAX_SUBS];
int subtitle_count = 0;

g2dImage *PMP_CUR_FRAME;
intraFont *SUBTITLE_FONT;

static void clear_subtitles() {
    // Очистка всех субтитров
    int i;
    for (i = 0; i < MAX_SUBS; i++) {
        subtitles[i].startTime[0] = '\0'; // Очистить временную метку начала
        subtitles[i].endTime[0] = '\0';   // Очистить временную метку окончания
        subtitles[i].text[0] = '\0'; // Очистить текст субтитра
    }
    subtitle_count = 0; // Сбросить количество субтитров
}

static void parseSRT(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open SRT file");
        return;
    }

    char byte;
    int found = 0;

    while ((byte = fgetc(file)) != EOF) {
        if (byte == 0x31) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Byte 0x31 not found in the file.\n");
        fclose(file);
        return;
    }

    fseek(file, -1, SEEK_CUR);

    char line[MAX_TEXT_LENGTH];
    while (subtitle_count < MAX_SUBS) {
        Subtitle sub = { .text = "" }; // Инициализация текста, пустая строка

        if (fscanf(file, "%*d\n") != 0)
            break;

        if (fscanf(file, "%[^ ] --> %[^ \n]\n", sub.startTime, sub.endTime) != 2)
            break;

        // Обнуляем текст перед загрузкой новых данных
        sub.text[0] = '\0';

        while (fgets(line, sizeof(line), file)) {
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

static void parseSubs(char *path) {
    size_t len = strlen(path);

    if (strcmp(path + len - 4, ".srt") == 0)
        parseSRT(path);

}

static char *LPYTVideoPath = NULL;
static int LPYTVideoPlay = 0;
static int PMP_LOOP = 0;

static void getFrame(g2dImage *tex) {
    int orig_width, orig_height;
    void *frame_data = pmp_get_frame(NULL, &orig_width, &orig_height, NULL);

    if (!frame_data)
        return;

    uint32_t *src = (uint32_t *)frame_data;

    int x, y;
    for (y = 0; y < 272; y++) {
        int orig_y = (y * orig_height) * INVERSE_272;
        uint32_t *row = src + orig_y * 512;

        for (x = 0; x < 480; x++) {
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

static float convertTimecodeToSeconds(const char *timecode) {
    int hours, minutes, seconds, milliseconds;
    sscanf(timecode, "%d:%d:%d,%d", &hours, &minutes, &seconds, &milliseconds);
    return hours * 3600 + minutes * 60 + seconds + milliseconds * 0.001f;
}

static float getTimeCode() {
    return ((float)PMP_CURRENT_FRAME * (1 / PMP_FPS) - 0.35f);
}


char *getSubString() {
    if (pmp_isplaying() && PMP_GOT_SUBS) {
        int i;

        for (i = 0; i < subtitle_count; i++) {
            float timeCode = getTimeCode();
            if (timeCode > convertTimecodeToSeconds(subtitles[i].startTime) && timeCode < convertTimecodeToSeconds(subtitles[i].endTime))
                return subtitles[i].text;
        }
        return "";
    }

    return "";
}

static int PMP_play(lua_State *L) {
    if (LPYTVideoPlay || pmp_isplaying() || PMP_PAUSE) {
        pmp_stop();

        clear_subtitles();
        removeLoadedImage(PMP_CUR_FRAME);
        if (LPYTVideoPath) {
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
        }
        PMP_GOT_SUBS = 0;
        PMP_CURRENT_FRAME = 0;
        PMP_PAUSE = FALSE;
    }

    int args = lua_gettop(L);
    if (args < 1 || args > 6)
        return luaL_error(L, "PMP.play(path, getPointer, [loop], [subtitlePath], [interruptButton], [FPS]) takes 1, 2, 3, 4, 5 or 6 arguments");

    char *path = (char *)luaL_checkstring(L, 1);
    bool getPointer = lua_toboolean(L, 2);
    PMP_LOOP = lua_toboolean(L, 3);
    char *subtitlepath = (args >= 4 && !lua_isnil(L, 4)) ? (char *)luaL_checkstring(L, 4) : NULL;
    PMP_INTERRUPT_BUTTON = (args >= 5 && !lua_isnil(L, 5)) ? luaL_checkint(L, 5) : 69;
    PMP_FPS = (args == 6) ? luaL_checknumber(L, 6) : 29.97;

    PMP_CUR_FRAME = _g2dTexCreate(480, 272, true);

    if (!PMP_CUR_FRAME)
        return luaL_error(L, "PMP.play() internal error");

    if (subtitlepath) {
        size_t len = strlen(subtitlepath);

        if (strcmp(subtitlepath + len - 4, ".srt") != 0 || !strcmp(subtitlepath + len - 4, ".ass") != 0)
            return luaL_error(L, "PMP.play() error: you're trying to load non-srt/ass subtitle file");

        parseSubs(subtitlepath);
        PMP_GOT_SUBS = 1;
    }

    if (getPointer) {
        if (pmp_play(path, 0, GU_PSM_8888) != 0)
            return luaL_error(L, "PMP.play() error: file \"%s\" doesn't exist or some internal error", path);

        if (LPYTVideoPath) {
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
        }

        LPYTVideoPath = (char *)malloc(512);

        if (!LPYTVideoPath)
            return luaL_error(L, "PMP.play() internal error");

        memcpy(LPYTVideoPath, path, 512);

        LPYTVideoPlay = 1;

        checkCapacity((void ***)&loadedImages, &imageCount, &capacity, imageCount + 1);
        loadedImages[imageCount++] = PMP_CUR_FRAME;

        printLoadedItems((void **)loadedImages, imageCount, capacity, "Images");

        g2dImage **image = pushG2D(L);
        *image = PMP_CUR_FRAME;

        return 1;
    }

    if (pmp_play(path, 1, GU_PSM_8888) != 0)
        return luaL_error(L, "PMP.play() error: file \"%s\" doesn't exist or some internal error", path);

    save_to_file("temp_sub_font.pgf", subs_font, size_subs_font);
    SUBTITLE_FONT = intraFontLoad("temp_sub_font.pgf", INTRAFONT_CACHE_LARGE | INTRAFONT_STRING_UTF8);
    remove("temp_sub_font.pgf");

    while (pmp_isplaying()) {
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

static int PMP_getFrame(lua_State *L) {
    if (LPYTVideoPlay) {
        if (pmp_isplaying()) {

            if (lua_gettop(L) != 1) {
                pmp_stop();
                return luaL_error(L, "PMP.getFrame(pointer) takes 1 argument");
            }

            if (!PMP_PAUSE) {
                g2dImage *tex = *toG2D(L, 1);
                getFrame(tex);
            }
            lua_pushboolean(L, TRUE);
        } else if (PMP_LOOP) {
            pmp_stop();
            pmp_play(LPYTVideoPath, 0, GU_PSM_8888);
            lua_pushboolean(L, TRUE);
        } else if (!PMP_LOOP)
            lua_pushnil(L);

        return 1;
    }

    lua_pushnil(L);
    return 1;

}

static int PMP_stop(lua_State *L) {
    int args = lua_gettop(L);
    if (args != 1) {
        pmp_stop();
        return luaL_error(L, "PMP.stop(pointer) takes 1 argument");
    }

    if (pmp_isplaying() || LPYTVideoPlay) {
        pmp_stop();
        g2dImage *tex = *toG2D(L, 1);

        if (removeLoadedImage(tex)) {
            clear_subtitles();
            LPYTVideoPlay = 0;
            PMP_GOT_SUBS = 0;
            free(LPYTVideoPath);
            LPYTVideoPath = NULL;
            printLoadedItems((void **)loadedImages, imageCount, capacity, "Images");
            return 0;
        }

        return luaL_error(L, "Image not found in loaded images");
    }

    return 0;
}

static int PMP_setVolume(lua_State *L) {
    if (lua_gettop(L) != 1)
        return luaL_error(L, "PMP.setVolume(volume) takes 1 argument");

    int vol = setInterval(luaL_checknumber(L, 1), 0, 100);

    PMP_AUDIO_VOLUME = ceil((32768 / 100) * vol);

    return 1;
}

static int PMP_getTimeCode(lua_State *L) {
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getTimeCode() takes no arguments");

    if (pmp_isplaying()) {
        char rez[20];
        sprintf(rez, "%.2f", getTimeCode());
        lua_pushstring(L, rez);
    } else
        lua_pushnil(L);

    return 1;
}

static int PMP_getSubs(lua_State *L) {
    if (lua_gettop(L) != 0)
        return luaL_error(L, "PMP.getSubs() takes no arguments");

    lua_pushstring(L, getSubString());

    return 1;
}

#include "../libs/Mp4/mp4info.h"

static int MP4_Info(lua_State *L) {
    // Открываем MP4 файл
    mp4info_t *info = mp4info_open(luaL_checkstring(L, 1));
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
        mp4info_track_t *track = info->tracks[i];

        printf("Track %d:\n", i + 1);
        printf("  Type: ");
        switch (track->type) {
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
        } else if (track->type == TRACK_AUDIO) {
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

static int PMP_pause(lua_State *L) {
    PMP_PAUSE = !PMP_PAUSE;

    return 0;
}

static int PMP_seek(lua_State *L) {
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

int PMP_init(lua_State *L) {
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