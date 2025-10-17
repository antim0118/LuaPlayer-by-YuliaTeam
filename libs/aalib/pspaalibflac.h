////////////////////////////////////////////////
//
//      pspaalibflac.h
//      Part of the PSP Advanced Audio Library
//
//      This file includes function declarations for
//      pspaalibflac.c.
//
////////////////////////////////////////////////

#ifndef _PSPAALIBFLAC_H_
#define _PSPAALIBFLAC_H_

#include "pspaalibcommon.h"
#include <FLAC/stream_decoder.h>

bool GetPausedFlac(int channel);
int SetAutoloopFlac(int channel, bool autoloop);
int GetStopReasonFlac(int channel);
int PlayFlac(int channel);
int StopFlac(int channel);
int PauseFlac(int channel);
int RewindFlac(int channel);
int GetBufferFlac(short* buf, int length, float amp, int channel);
int LoadFlac(char* filename, int channel);
int UnloadFlac(int channel);

#endif