	.set noreorder

#include "pspstub.s"

	STUB_START "Support",0x40090000,0x00130005
	STUB_FUNC  0x97B05960,getBaryon
	STUB_FUNC  0xE21C181C,getPommel
	STUB_FUNC  0xE2A5D1EE,sceSysregGetTachyonVersion
	STUB_END
