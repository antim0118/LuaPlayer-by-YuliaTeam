/* SCE CONFIDENTIAL
 PSP(TM) Programmer Tool Runtime Library Release 6.6.0
 *
 *      Copyright (C) 2008 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 */

#ifndef _SCE_VAUDIO_H
#define _SCE_VAUDIO_H

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif	/* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

/*J 音量変化なし (0dB) */
/*E No volume change (0dB)  */
#define SCE_VAUDIO_VOLUME_0dB 32768

/*J 16bit ステレオ形式 */
/*E 16-bit stereo format  */
#define SCE_VAUDIO_FMT_S16_STEREO 2

/*J 最大サンプル長 */
/*E Maximum sample length  */
#define SCE_VAUDIO_MAX_SAMPLE_LEN 2048u

/*J 最小サンプル長 */
/*E Minimum sample length  */
#define SCE_VAUDIO_MIN_SAMPLE_LEN 256u

/*J 以下の関数3つの機能は基本的に sceAudioXXX と同じです */
/*E The features of the following three functions are basically the same as those of sceAudioXXX */
int sceVaudioOutputBlocking(int vol, void *datapointer);
int sceVaudioChReserve(int len, int frequency, int bytesPerSample);
int sceVaudioChRelease(void);

/*J エフェクトタイプを設定 */
/*E Set the effect type  */
int sceVaudioSetEffectType(int type);

/*J プリセットエフェクトタイプの識別子 */
/*E Preset effect type identifier  */
#define SCE_VAUDIO_FX_TYPE_THRU		0
#define SCE_VAUDIO_FX_TYPE_HEAVY	1
#define SCE_VAUDIO_FX_TYPE_POPS		2
#define SCE_VAUDIO_FX_TYPE_JAZZ		3
#define SCE_VAUDIO_FX_TYPE_UNIQUE 	4
#define SCE_VAUDIO_FX_TYPE_MAX		5

/*J ALC モード設定 */
#define SCE_VAUDIO_ALC_OFF		0
#define SCE_VAUDIO_ALC_MODE1	1
#define SCE_VAUDIO_ALC_MODE_MAX	2

int sceVaudioSetAlcMode(int mode);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif	/* defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus) */

#endif	/* _SCE_VAUDIO_H */
