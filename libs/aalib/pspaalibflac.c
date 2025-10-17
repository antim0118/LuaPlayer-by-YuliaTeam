////////////////////////////////////////////////
//
//      pspaalibflac.c
//      Part of the PSP Advanced Audio Library
//      Based on flacplayer code
//
//      This file includes functions for playing FLAC files.
//
////////////////////////////////////////////////

#include "pspaalibflac.h"

typedef struct {
    FLAC__StreamDecoder *decoder;
    short *pcm_buffer;
    int buffer_size;
    int buffer_pos;
    int buffer_filled;
    int sample_rate;
    int channels;
    bool paused;
    bool initialized;
    bool autoloop;
    int stopReason;
} FlacFileInfo;

static FlacFileInfo streamsFlac[4] = {0};

static void log_flac_info(int channel) {
    printf("\n=== FLAC Channel %d Info ===\n", channel);
    printf("Initialized: %s\n", streamsFlac[channel].initialized ? "YES" : "NO");
    printf("Decoder: %p\n", streamsFlac[channel].decoder);
    printf("PCM Buffer: %p\n", streamsFlac[channel].pcm_buffer);
    printf("Buffer Size: %d samples\n", streamsFlac[channel].buffer_size);
    printf("Sample Rate: %d Hz (expected: 44100)\n", streamsFlac[channel].sample_rate);
    printf("Channels: %d\n", streamsFlac[channel].channels);
    printf("Paused: %s\n", streamsFlac[channel].paused ? "YES" : "NO");
    printf("Autoloop: %s\n", streamsFlac[channel].autoloop ? "YES" : "NO");
    printf("Stop Reason: %d\n", streamsFlac[channel].stopReason);
    printf("Buffer Pos/Filled: %d/%d\n", streamsFlac[channel].buffer_pos, streamsFlac[channel].buffer_filled);
    printf("==========================\n");
}

static short scale_sample(FLAC__int32 sample) {
    sample = sample >> 8;
    if (sample > 32767) return 32767;
    if (sample < -32768) return -32768;
    return (short)sample;
}

static FLAC__StreamDecoderWriteStatus write_callback(
    const FLAC__StreamDecoder *decoder,
    const FLAC__Frame *frame,
    const FLAC__int32 *const buffer[],
    void *client_data) 
{
    int channel = *(int*)client_data;
    printf("\n[WriteCallback] Channel %d - New frame\n", channel);
    printf("  Blocksize: %ld\n", frame->header.blocksize);
    printf("  Channels: %ld\n", frame->header.channels);
    printf("  Sample Rate: %ld\n", frame->header.sample_rate);
    printf("  Bits per Sample: %ld\n", frame->header.bits_per_sample);

    if (channel < 0 || channel > 3) {
        printf("[WriteCallback] ERROR: Invalid channel %d\n", channel);
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    int samples = frame->header.blocksize;
    streamsFlac[channel].channels = frame->header.channels;

    // Проверяем и выделяем буфер при необходимости
    if (!streamsFlac[channel].pcm_buffer || samples > streamsFlac[channel].buffer_size) {
        printf("[WriteCallback] (Re)allocating buffer for %d samples\n", samples);
        if (streamsFlac[channel].pcm_buffer) {
            free(streamsFlac[channel].pcm_buffer);
        }
        streamsFlac[channel].pcm_buffer = (short*)malloc(samples * 2 * sizeof(short));
        if (!streamsFlac[channel].pcm_buffer) {
            printf("[WriteCallback] ERROR: Failed to allocate PCM buffer\n");
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }
        streamsFlac[channel].buffer_size = samples;
    }

    // Преобразование и копирование данных
    printf("[WriteCallback] Converting samples...\n");
    for (int i = 0; i < samples; i++) {
        // Пример лога первых 5 сэмплов
        if (i < 5) {
            printf("  Sample %d: L=%ld, R=%ld\n", i, buffer[0][i], 
                   frame->header.channels > 1 ? buffer[1][i] : buffer[0][i]);
        }
        
        // Левый канал
        streamsFlac[channel].pcm_buffer[i*2] = (short)(buffer[0][i] >> 8);
        
        // Правый канал
        if (frame->header.channels > 1) {
            streamsFlac[channel].pcm_buffer[i*2+1] = (short)(buffer[1][i] >> 8);
        } else {
            streamsFlac[channel].pcm_buffer[i*2+1] = streamsFlac[channel].pcm_buffer[i*2];
        }
    }

    streamsFlac[channel].buffer_pos = 0;
    streamsFlac[channel].buffer_filled = samples;

    printf("[WriteCallback] Done. Buffer filled with %d samples\n", samples);
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void metadata_callback(
    const FLAC__StreamDecoder *decoder,
    const FLAC__StreamMetadata *metadata,
    void *client_data) 
{
    int channel = *(int*)client_data;
    printf("\n[MetadataCallback] Channel %d\n", channel);
    
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        printf("  STREAMINFO metadata:\n");
        printf("    Sample Rate: %ld Hz\n", metadata->data.stream_info.sample_rate);
        printf("    Channels: %ld\n", metadata->data.stream_info.channels);
        printf("    Bits per Sample: %ld\n", metadata->data.stream_info.bits_per_sample);
        printf("    Total Samples: %llu\n", metadata->data.stream_info.total_samples);
        printf("    Max Blocksize: %ld\n", metadata->data.stream_info.max_blocksize);
        
        streamsFlac[channel].sample_rate = metadata->data.stream_info.sample_rate;
        streamsFlac[channel].channels = metadata->data.stream_info.channels;
    }
    else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
        printf("  VORBISCOMMENT metadata (ignored)\n");
    }
    else {
        printf("  Other metadata type: %d (ignored)\n", metadata->type);
    }
}

static void error_callback(
    const FLAC__StreamDecoder *decoder,
    FLAC__StreamDecoderErrorStatus status,
    void *client_data) 
{
    int channel = *(int*)client_data;
    printf("\n[ErrorCallback] Channel %d - Error: %s\n", 
           channel, FLAC__StreamDecoderErrorStatusString[status]);
}

bool GetPausedFlac(int channel) {
    if (channel < 0 || channel > 3) {
        return true; // Возвращаем true для неверного канала как безопасный вариант
    }
    if (!streamsFlac[channel].initialized) {
        return true;
    }
    return streamsFlac[channel].paused;
}

int SetAutoloopFlac(int channel, bool autoloop) {
    if (channel < 0 || channel > 3) {
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }
    if (!streamsFlac[channel].initialized) {
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }
    streamsFlac[channel].autoloop = autoloop;
    return PSPAALIB_SUCCESS;
}

int GetStopReasonFlac(int channel) {
    if (channel < 0 || channel > 3) {
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }
    if (!streamsFlac[channel].initialized) {
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }
    return streamsFlac[channel].stopReason;
}

int PlayFlac(int channel) {
    printf("\n[PlayFlac] Starting playback on channel %d\n", channel);
    
    if (channel < 0 || channel > 3) {
        printf("[PlayFlac] ERROR: Invalid channel %d\n", channel);
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }

    if (!streamsFlac[channel].initialized) {
        printf("[PlayFlac] ERROR: Channel not initialized\n");
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }

    if (!FLAC__stream_decoder_seek_absolute(streamsFlac[channel].decoder, 0)) {
        printf("[PlayFlac] WARNING: Seek failed, resetting decoder\n");
        FLAC__stream_decoder_reset(streamsFlac[channel].decoder);
        FLAC__stream_decoder_process_until_end_of_metadata(streamsFlac[channel].decoder);
    }

    streamsFlac[channel].paused = false;
    streamsFlac[channel].stopReason = PSPAALIB_STOP_NOT_STOPPED;
    streamsFlac[channel].buffer_pos = 0;
    streamsFlac[channel].buffer_filled = 0;

    printf("[PlayFlac] Pre-filling buffer...\n");
    for (int i = 0; i < 3; i++) {
        if (!FLAC__stream_decoder_process_single(streamsFlac[channel].decoder)) {
            printf("[PlayFlac] ERROR: Failed to decode initial frames\n");
            streamsFlac[channel].paused = true;
            return PSPAALIB_ERROR_FLAC_INVALID_FILE;
        }
    }

    printf("[PlayFlac] Playback started successfully\n");
    log_flac_info(channel);
    return PSPAALIB_SUCCESS;
}

int StopFlac(int channel) {
    if (channel < 0 || channel > 3) {
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }
    if (!streamsFlac[channel].initialized) {
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }
    
    RewindFlac(channel);
    streamsFlac[channel].paused = true;
    streamsFlac[channel].stopReason = PSPAALIB_STOP_ON_REQUEST;
    
    return PSPAALIB_SUCCESS;
}

int PauseFlac(int channel) {
    if (channel < 0 || channel > 3) {
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }
    if (!streamsFlac[channel].initialized) {
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }
    
    streamsFlac[channel].paused = !streamsFlac[channel].paused;
    streamsFlac[channel].stopReason = PSPAALIB_STOP_NOT_STOPPED;
    
    return PSPAALIB_SUCCESS;
}

int RewindFlac(int channel) {
    if (channel < 0 || channel > 3) {
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }
    if (!streamsFlac[channel].initialized) {
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }
    
    if (!FLAC__stream_decoder_seek_absolute(streamsFlac[channel].decoder, 0)) {
        FLAC__stream_decoder_reset(streamsFlac[channel].decoder);
        FLAC__stream_decoder_process_until_end_of_metadata(streamsFlac[channel].decoder);
    }
    
    streamsFlac[channel].buffer_pos = 0;
    streamsFlac[channel].buffer_filled = 0;
    
    return PSPAALIB_SUCCESS;
}

int GetBufferFlac(short* buf, int length, float amp, int channel) {
    printf("\n[GetBuffer] Requested %d samples from channel %d (amp=%.2f)\n", length, channel, amp);
    
    if (channel < 0 || channel > 3) {
        printf("[GetBuffer] ERROR: Invalid channel %d\n", channel);
        memset(buf, 0, length * 2 * sizeof(short));
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }

    if (!streamsFlac[channel].initialized) {
        printf("[GetBuffer] ERROR: Channel not initialized\n");
        memset(buf, 0, length * 2 * sizeof(short));
        return PSPAALIB_ERROR_FLAC_UNINITIALIZED_CHANNEL;
    }

    if (streamsFlac[channel].paused) {
        printf("[GetBuffer] WARNING: Channel paused\n");
        memset(buf, 0, length * 2 * sizeof(short));
        return PSPAALIB_WARNING_PAUSED_BUFFER_REQUESTED;
    }

    int samples_copied = 0;
    while (samples_copied < length) {
        if (streamsFlac[channel].buffer_pos >= streamsFlac[channel].buffer_filled) {
            printf("[GetBuffer] Need more data, decoding...\n");
            if (!FLAC__stream_decoder_process_single(streamsFlac[channel].decoder)) {
                if (!streamsFlac[channel].autoloop) {
                    printf("[GetBuffer] End of stream reached\n");
                    streamsFlac[channel].paused = true;
                    streamsFlac[channel].stopReason = PSPAALIB_STOP_END_OF_STREAM;
                    memset(buf + samples_copied * 2, 0, (length - samples_copied) * 2 * sizeof(short));
                    return PSPAALIB_WARNING_END_OF_STREAM_REACHED;
                }
                printf("[GetBuffer] Autoloop enabled, rewinding...\n");
                RewindFlac(channel);
                continue;
            }
            continue;
        }

        int samples_available = streamsFlac[channel].buffer_filled - streamsFlac[channel].buffer_pos;
        int samples_to_copy = (samples_available < (length - samples_copied)) ? 
                            samples_available : (length - samples_copied);

        printf("[GetBuffer] Copying %d samples (pos=%d, filled=%d)\n", 
               samples_to_copy, streamsFlac[channel].buffer_pos, streamsFlac[channel].buffer_filled);

        for (int i = 0; i < samples_to_copy * 2; i++) {
            buf[samples_copied * 2 + i] = (short)(streamsFlac[channel].pcm_buffer[streamsFlac[channel].buffer_pos * 2 + i] * amp);
        }

        samples_copied += samples_to_copy;
        streamsFlac[channel].buffer_pos += samples_to_copy;
    }

    printf("[GetBuffer] Successfully filled buffer\n");
    return PSPAALIB_SUCCESS;
}

int LoadFlac(char *filename, int channel) {
    printf("\n[LoadFlac] Starting to load FLAC file '%s' on channel %d\n", filename, channel);
    
    if (channel < 0 || channel > 3) {
        printf("[LoadFlac] ERROR: Invalid channel %d\n", channel);
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }

    if (streamsFlac[channel].initialized) {
        printf("[LoadFlac] Channel %d already initialized, unloading first\n", channel);
        UnloadFlac(channel);
    }

    printf("[LoadFlac] Initializing FLAC decoder...\n");
    streamsFlac[channel].decoder = FLAC__stream_decoder_new();
    if (!streamsFlac[channel].decoder) {
        printf("[LoadFlac] ERROR: Failed to create FLAC decoder\n");
        return PSPAALIB_ERROR_FLAC_INVALID_FILE;
    }

    FLAC__stream_decoder_set_md5_checking(streamsFlac[channel].decoder, false);
    printf("[LoadFlac] MD5 checking disabled\n");

    printf("[LoadFlac] Initializing file decoder...\n");
    FLAC__StreamDecoderInitStatus init_status = FLAC__stream_decoder_init_file(
        streamsFlac[channel].decoder, filename, 
        write_callback, metadata_callback, error_callback, &channel);

    if (init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        printf("[LoadFlac] ERROR: Decoder init failed with status %d\n", init_status);
        FLAC__stream_decoder_delete(streamsFlac[channel].decoder);
        streamsFlac[channel].decoder = NULL;
        return PSPAALIB_ERROR_FLAC_INVALID_FILE;
    }

    printf("[LoadFlac] Processing metadata...\n");
    if (!FLAC__stream_decoder_process_until_end_of_metadata(streamsFlac[channel].decoder)) {
        printf("[LoadFlac] ERROR: Failed to process metadata\n");
        FLAC__stream_decoder_delete(streamsFlac[channel].decoder);
        streamsFlac[channel].decoder = NULL;
        return PSPAALIB_ERROR_FLAC_INVALID_FILE;
    }

    printf("[LoadFlac] Checking audio format...\n");
    if (streamsFlac[channel].sample_rate != 44100) {
        printf("[LoadFlac] ERROR: Invalid sample rate %d (expected 44100)\n", streamsFlac[channel].sample_rate);
        FLAC__stream_decoder_delete(streamsFlac[channel].decoder);
        streamsFlac[channel].decoder = NULL;
        return PSPAALIB_ERROR_FLAC_INVALID_SAMPLE_RATE;
    }

    if (streamsFlac[channel].channels < 1 || streamsFlac[channel].channels > 2) {
        printf("[LoadFlac] ERROR: Invalid channel count %d\n", streamsFlac[channel].channels);
        FLAC__stream_decoder_delete(streamsFlac[channel].decoder);
        streamsFlac[channel].decoder = NULL;
        return PSPAALIB_ERROR_FLAC_INVALID_FILE;
    }

    streamsFlac[channel].initialized = true;
    streamsFlac[channel].paused = true;
    streamsFlac[channel].stopReason = PSPAALIB_STOP_JUST_LOADED;
    streamsFlac[channel].autoloop = false;

    printf("[LoadFlac] Successfully loaded FLAC file\n");
    log_flac_info(channel);
    
    return PSPAALIB_SUCCESS;
}

int UnloadFlac(int channel) {
    printf("\n[UnloadFlac] Unloading channel %d\n", channel);
    
    if (channel < 0 || channel > 3) {
        printf("[UnloadFlac] ERROR: Invalid channel %d\n", channel);
        return PSPAALIB_ERROR_FLAC_INVALID_CHANNEL;
    }

    if (!streamsFlac[channel].initialized) {
        printf("[UnloadFlac] WARNING: Channel already uninitialized\n");
        return PSPAALIB_SUCCESS;
    }

    if (streamsFlac[channel].decoder) {
        printf("[UnloadFlac] Finishing decoder...\n");
        FLAC__stream_decoder_finish(streamsFlac[channel].decoder);
        FLAC__stream_decoder_delete(streamsFlac[channel].decoder);
        streamsFlac[channel].decoder = NULL;
    }

    if (streamsFlac[channel].pcm_buffer) {
        printf("[UnloadFlac] Freeing PCM buffer...\n");
        free(streamsFlac[channel].pcm_buffer);
        streamsFlac[channel].pcm_buffer = NULL;
    }

    printf("[UnloadFlac] Resetting channel state...\n");
    memset(&streamsFlac[channel], 0, sizeof(FlacFileInfo));
    
    printf("[UnloadFlac] Channel %d unloaded\n", channel);
    return PSPAALIB_SUCCESS;
}