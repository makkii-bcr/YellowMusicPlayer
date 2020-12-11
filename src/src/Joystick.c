#include "Joystick.h"


BOOL Joystick_getOn(DWORD id) {
	JOYINFOEX joyInfo;
	if (joyGetPosEx(id, &joyInfo) == JOYERR_NOERROR) {
		return TRUE;
	} else {
		return FALSE;
	}
}
