/*
PMP Mod
Copyright (C) 2006 jonny
Copyright (C) 2007 Raphael <raphael@fx-world.org>

Homepage: http://jonny.leffe.dnsalias.com
          http://wordpress.fx-world.org
E-mail:   jonny@leffe.dnsalias.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
this play the file (av output and basic functions - pause, seek ... )
*/


#ifndef pmp_play_h
#define pmp_play_h

#include <pspthreadman.h>
#include <pspaudio.h>
#include <pspaudio_kernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>
#include "pmp_decode.h"

#ifndef GLOBALS_PMP
#define GLOBALS_PMP

extern int PMP_AUDIO_VOLUME;
extern int PMP_INTERRUPT_BUTTON;
extern int PMP_GAME_EXIT;
extern int PMP_CURRENT_FRAME;
extern int PMP_NUMBER_OF_FRAMES;
extern int PMP_GOT_SUBS;
extern int PMP_PAUSE;
extern float PMP_FPS;
extern int PMP_LAST_FRAME;

#endif //GLOBALS_PMP

struct pmp_play_struct
	{
	struct pmp_decode_struct decoder;


    int audio_reserved;
	
	SceUID semaphore_can_get;
	SceUID semaphore_can_put;
	SceUID semaphore_can_show;
	SceUID semaphore_show_done;

	SceUID output_thread;
	SceUID show_thread;
	SceUID decode_thread;


	int   playing;
	int   finished;
	int   isLoop;
	int   return_request;
	char *return_result;


	int show;

	unsigned int audio_stream;
	int audio_channel;
	unsigned int volume_boost;
	
	unsigned int subtitle_count;
	unsigned int subtitle;
	unsigned int subtitle_format;
	unsigned int subtitle_fontcolor;
	unsigned int subtitle_bordercolor;

	int seek_request;
	int seek_frame;
	};



void pmp_play_safe_constructor(struct pmp_play_struct *p);
char *pmp_play_open(struct pmp_play_struct *p, char *s, int show, int format);
void pmp_play_close(struct pmp_play_struct *p);
char *pmp_play_start(volatile struct pmp_play_struct *p);
char *pmp_play_waitend(struct pmp_play_struct *p);
int pmp_decode_thread(SceSize input_length, void *input);
int pmp_output_thread(SceSize input_length, void *input);

#endif
