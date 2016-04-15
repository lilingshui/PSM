#include "Bio_Func.h"

void main() {
	_sysinit();
	_setscreenmode(1);
	_setdispcolor(1);
	_gotoxy(2,2);
	_putstr((U8*)"Hello world pengsm");
	_bioskey(0);
}