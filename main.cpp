#include <windows.h>
#include "ToggleKeyboardLeds.h"

int main()
{
	ToggleKeyboardLeds leds;
	while (1)
	{
		leds.toggle(Leds::Num_Lock, true);
		Sleep(500);
		leds.toggle(Leds::Caps_Lock, true);
		Sleep(500);
		leds.toggle(Leds::Scroll_Lock, true);
		Sleep(500);
		leds.toggle(Leds::Num_Lock, false);
		Sleep(500);
		leds.toggle(Leds::Caps_Lock, false);
		Sleep(500);
		leds.toggle(Leds::Scroll_Lock, false);
		Sleep(500);
	}
	return 0;
}