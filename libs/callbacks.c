#include "callbacks.h"

extern void g2dTerm();
extern void intraFontShutdown();
extern void pmp_shutdown();

void LPYT_FastFinish() {
  g2dTerm();
  intraFontShutdown();
  pmp_shutdown();
  sceKernelExitGame();
}

int exit_callback(int arg1, int arg2, void *common) {
  LPYT_FastFinish();

  return 0;
}

int CallbackThread(SceSize args, void *argp) {
  int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);
  sceKernelSleepThreadCB();

  return 0;
}

int SetupCallbacks(void) {
  int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);

  if (thid >= 0)
    sceKernelStartThread(thid, 0, 0);

  return thid;
}