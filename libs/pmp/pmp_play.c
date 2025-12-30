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


#include <pspgu.h>
#include "pmp_play.h"
#include "../ctrl/controls.h"
#include "../../lua/common.h"
#include "../aalib/pspaalib.h"

int PMP_AUDIO_VOLUME;
int PMP_INTERRUPT_BUTTON;
int PMP_GAME_EXIT;
int PMP_CURRENT_FRAME;
int PMP_NUMBER_OF_FRAMES;
int PMP_GOT_SUBS;
int PMP_PAUSE;
float PMP_FPS;
int PMP_LAST_FRAME;

void pmp_play_safe_constructor(struct pmp_play_struct *p)
{
	p->audio_reserved = -1;
	
	p->semaphore_can_get   = -1;
	p->semaphore_can_put   = -1;
	p->semaphore_can_show  = -1;
	p->semaphore_show_done = -1;

	//p->output_thread = -1;
	//p->decode_thread = -1;
	if (!PMP_PAUSE)
		PMP_CURRENT_FRAME = 0;
	PMP_NUMBER_OF_FRAMES = -1;

	p->show_thread   = -1;

	p->playing = 0;
	p->finished = 0;
	p->return_request = 0;
	p->return_result  = 0;
	p->seek_request = 0;
	p->seek_frame = 0;
	pmp_decode_safe_constructor(&p->decoder);
}

void pmp_play_close(struct pmp_play_struct *p)
{
	//sceAudioSetFrequency(44100);
	if (!(p->audio_reserved < 0)) sceVaudioChRelease(); //FreeHardwareChannel(69);// sceAudioChRelease(p->audio_reserved);

	if (!(p->semaphore_can_get   < 0)) sceKernelDeleteSema(p->semaphore_can_get);
	if (!(p->semaphore_can_put   < 0)) sceKernelDeleteSema(p->semaphore_can_put);
	if (!(p->semaphore_can_show  < 0)) sceKernelDeleteSema(p->semaphore_can_show);
	if (!(p->semaphore_show_done < 0)) sceKernelDeleteSema(p->semaphore_show_done);

	//if (!(p->output_thread < 0)) sceKernelDeleteThread(p->output_thread);
	//if (!(p->decode_thread < 0)) sceKernelDeleteThread(p->decode_thread);
	//if (!(p->show_thread   < 0)) sceKernelTerminateDeleteThread(p->show_thread);

	pmp_decode_close(&p->decoder);

	pmp_play_safe_constructor(p);
}


static int pmp_wait(volatile struct pmp_play_struct *p, SceUID s, char *e)
{
	SceUInt t = 1000000;
	

	while (1)
	{
		int result = sceKernelWaitSema(s, 1, &t);

		if (result == SCE_KERNEL_ERROR_OK)
		{
			break;
		}
		else if (result == SCE_KERNEL_ERROR_WAIT_TIMEOUT)
		{
			sceKernelDelayThread(1);

			if (p->return_request == 1)
				return(0);
		}
		else
		{
			p->return_result  = e;
			p->return_request = 1;
			return(0);
		}
	}


	return(1);
}



int pmp_decode_thread(SceSize input_length, void *input)
{
	volatile struct pmp_play_struct *p = *((void **) input);
	
	//int current_video_frame = 0;
	//PMP_CURRENT_FRAME = 29;
	PMP_NUMBER_OF_FRAMES = p->decoder.reader.file.header.video.number_of_frames;
	printf("starting decode thread.\n");
	printf("number of frames: %i\n", p->decoder.reader.file.header.video.number_of_frames);
	
	while (p->return_request == 0 && PMP_CURRENT_FRAME != p->decoder.reader.file.header.video.number_of_frames)
	{

		if (p->seek_request) {
            PMP_CURRENT_FRAME = p->seek_frame;
            p->seek_request = 0;
            
            // Сбросить буферы декодера
            p->decoder.current_video_frame = p->decoder.video_frame_buffers[0];
            
            p->seek_request = 0;
        }

		if (pmp_wait(p, p->semaphore_can_put, "pmp_play_start: sceKernelWaitSema failed on semaphore_can_put") == 0)
		{
			break;
		}

		if (!PMP_PAUSE)
		{
			p->decoder.last_buffer_number = p->decoder.current_buffer_number;
	
			char *result = pmp_decode_get((struct pmp_decode_struct *) &p->decoder, PMP_CURRENT_FRAME, p->audio_stream, 1, p->volume_boost/*, p->subtitle, p->subtitle_format*/);
			if (result != 0)
			{
				p->return_result  = result;
				p->return_request = 1;
				break;
			}
		}
		/* // Uncomment and run decode_thread only for a speed benchmark
		p->decoder.current_video_frame = p->decoder.output_frame_buffers[p->decoder.last_buffer_number].video_frame;
		if (p->decoder.video_buffer_width==512)
		{
			sceDisplaySetFrameBuf(p->decoder.current_video_frame, 512, p->decoder.video_decode_format, PSP_DISPLAY_SETBUF_NEXTFRAME);
		}*/
		
		if (sceKernelSignalSema(p->semaphore_can_get, 1) < 0)
		{
			p->return_result  = "pmp_play_start: sceKernelSignalSema failed on semaphore_can_get";
			p->return_request = 1;
			break;
		}
		
		if (!PMP_PAUSE)
			PMP_CURRENT_FRAME++;
		
	}
	
	printf("current frame: %i\n", PMP_CURRENT_FRAME);
	if (p->return_request == 0)
	{
		p->finished = 1;
		p->return_result = "finished decoding";
	}
	
	printf("exiting decode thread: %s\n", p->return_result);
	return(0);
}


static void pmp_input(volatile struct pmp_play_struct *p)
{
	if (p->show)
		controls_read();

	if (p->show && controls_pressed(PSP_CTRL_SELECT))
		PMP_PAUSE = !PMP_PAUSE;

	if ((p->show && PMP_INTERRUPT_BUTTON != 69 && controls_pressed(PMP_INTERRUPT_BUTTON)) || (PMP_GAME_EXIT == 1))
	{
		p->return_request = 1;
		p->return_result = "exit: manual";
	}

}

int dlist[512] __attribute__ ((aligned (16)));

int pmp_output_thread(SceSize input_length, void *input)
{
	struct pmp_play_struct *p = *((void **) input);
	printf("starting output thread.\n");
	
	unsigned int first_video_frame     = 1;
	unsigned int current_buffer_number = 0;
	
	while (p->return_request == 0 && (p->finished == 0 || current_buffer_number != p->decoder.last_buffer_number) )
	{
		pmp_input(p);

		volatile struct pmp_decode_buffer_struct *current_buffer;

		if (p->seek_request) {
            // Сбросить текущий номер буфера при перемотке
            current_buffer_number = 0;
        }

		if (!PMP_PAUSE)
		{
			current_buffer = &p->decoder.output_frame_buffers[current_buffer_number];
		}

		if (pmp_wait(p, p->semaphore_can_get, "pmp_output_thread: sceKernelWaitSema failed on semaphore_can_get") == 0)
		{
			break;
		}
		
		//current_buffer->first_delay -= 500;
		//sceKernelDelayThread(current_buffer->first_delay < 1 ? 1 : current_buffer->first_delay);
		sceKernelDelayThread(1);
		
		p->decoder.current_video_frame = current_buffer->video_frame;

		if (!PMP_PAUSE)
		{
			
			//sceAudioOutputBlocking(p->audio_reserved, PMP_AUDIO_VOLUME, current_buffer->audio_frame);
			sceVaudioOutputBlocking(PMP_AUDIO_VOLUME, current_buffer->audio_frame);

			int i = 1;
			for (; i < current_buffer->number_of_audio_frames; i++)
			{
				//sceAudioOutputBlocking(p->audio_reserved, PMP_AUDIO_VOLUME, current_buffer->audio_frame + p->decoder.audio_frame_size * i);
				sceVaudioOutputBlocking(PMP_AUDIO_VOLUME, current_buffer->audio_frame + p->decoder.audio_frame_size * i);
			}
		
			//current_buffer->last_delay -= 500;
		
			current_buffer_number = (current_buffer_number + 1) % p->decoder.number_of_frame_buffers;
		}

		//sceKernelDelayThread(current_buffer->last_delay < 1 ? 1 : current_buffer->last_delay);
		sceKernelDelayThread(1);

		if (first_video_frame == 1)
		{
			first_video_frame = 0;
		}
		else
		{
			if (sceKernelSignalSema(p->semaphore_can_put, 1) < 0)
			{
				p->return_result  = "pmp_output_thread: sceKernelSignalSema failed on semaphore_can_put";
				p->return_request = 1;
				break;
			}
		}
	}
	
	p->return_request = 1;
	printf("exiting output thread: %s\n", p->return_result);
	
	sceKernelWaitThreadEnd(p->decode_thread, 0);
	//pmp_play_waitend(p);
	pmp_play_close(p);
	return(0);
}



char *pmp_play_start(volatile struct pmp_play_struct *p)
{
	p->playing = 1;
	sceKernelStartThread(p->decode_thread, 4, &p);	
	sceKernelStartThread(p->output_thread, 4, &p);
	return(0);
}



char *pmp_play_waitend(struct pmp_play_struct *p)
{
	if (p->decode_thread>=0) sceKernelWaitThreadEnd(p->decode_thread, 0);
	if (p->output_thread>=0) sceKernelWaitThreadEnd(p->output_thread, 0);
	return(0);
}


char *pmp_play_open(struct pmp_play_struct *p, char *s, int show, int format)
{
	pmp_play_safe_constructor(p);
	p->subtitle = 0;
	p->subtitle_count = 0;
	p->show = show;

	printf(">pmp_play_open( %p, \"%s\", %i, %i)\n", p, s, show, format);
	char *result = pmp_decode_open(&p->decoder, s, format);
	if (result != 0)
	{
		pmp_play_close(p);
		return(result);
	}

	/*
	//modify by cooleyes 2006/12/11
	//#define video_directory "ms0:/PSP/VIDEO/"
	char video_directory[512];
	char video_filename[512];
	memset(video_directory, 0, 512);
	memset(video_filename, 0, 512);
	char* divchar = strrchr(s, '/');
	if ( divchar == NULL) {
		strncpy(video_directory, "ms0:/PSP/VIDEO/", 512);
		strncpy(video_filename, s, 512);
	}
	else {
		strncpy(video_directory, s, divchar-s+1);
		strncpy(video_filename, divchar+1, 512); 
	}
	if (pmp_sub_parse_search( video_directory, video_filename, p->decoder.reader.file.header.video.rate, p->decoder.reader.file.header.video.scale, &p->subtitle_count)==0) p->subtitle = 1;
	//if (pmp_sub_parse_search( video_directory, s, p->decoder.reader.file.header.video.rate, p->decoder.reader.file.header.video.scale, &p->subtitle_count)==0) p->subtitle = 1;
	//modify end 
	*/
	

	/*if (sceAudioSetFrequency(p->decoder.reader.file.header.audio.rate) < 0)
	{
		pmp_play_close(p);
		return("pmp_play_open: sceAudioSetFrequency failed");
	}*/
	
	//int free_audio_channel = GetFreeHardwareChannel(69);
	//if (free_audio_channel < 0)
	//{
//		pmp_play_close(p);
//		return("pmp_play_open: no free audio channels");
//	}

	//p->audio_reserved = sceAudioChReserve(free_audio_channel, p->decoder.reader.file.header.audio.scale, PSP_AUDIO_FORMAT_STEREO);
	p->audio_reserved = sceVaudioChReserve(p->decoder.reader.file.header.audio.scale, 44100, PSP_VAUDIO_FORMAT_STEREO);
	if (p->audio_reserved < 0)
	{
		pmp_play_close(p);
		return("pmp_play_open: sceAudioChReserve failed");
	}
	
	p->semaphore_can_get = sceKernelCreateSema("can_get", 0, 0, p->decoder.number_of_frame_buffers, 0);
	if (p->semaphore_can_get < 0)
	{
		pmp_play_close(p);
		return("pmp_play_open: sceKernelCreateSema failed on semaphore_can_get");
	}


	p->semaphore_can_put = sceKernelCreateSema("can_put", 0, p->decoder.number_of_frame_buffers, p->decoder.number_of_frame_buffers, 0);
	if (p->semaphore_can_put < 0)
	{
		pmp_play_close(p);
		return("pmp_play_open: sceKernelCreateSema failed on semaphore_can_put");
	}



	/*if (p->output_thread == -1)
	{
		p->output_thread = sceKernelCreateThread("output", pmp_output_thread, 0x8, 32768, 0, 0);
		if (p->output_thread < 0)
		{
			pmp_play_close(p);
			return("pmp_play_open: sceKernelCreateThread failed on output_thread");
		}
	}

	if (p->decode_thread == -1)
	{
		p->decode_thread = sceKernelCreateThread("decode", pmp_decode_thread, 0x8, 32768, 0, 0);
		if (p->decode_thread < 0)
		{
			pmp_play_close(p);
			return("pmp_play_open: sceKernelCreateThread failed on decode_thread");
		}
	}*/

	p->return_request = 0;
	p->return_result  = 0;

	p->audio_stream     = 0;
	p->volume_boost     = 0;


	printf(">pmp_play_open done.\n");
	return(0);
}
