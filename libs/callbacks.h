#include <stdint.h>
#include <stdio.h>
#include <pspkernel.h>
#include <pspkerneltypes.h>
#include "pmp/pmp_play.h"

int exit_callback(int arg1, int arg2, void *common);

int CallbackThread(SceSize args, void *argp);

int SetupCallbacks(void);