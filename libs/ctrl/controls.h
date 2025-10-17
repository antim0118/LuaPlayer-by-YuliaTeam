#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <pspctrl.h>

void controls_init();
void controls_read();
int controls_pressed(const unsigned int button);
int controls_held(const unsigned int button);
int controls_released(const unsigned int button);
int controls_AnalogX();
int controls_AnalogY();

#ifdef __cplusplus
}
#endif