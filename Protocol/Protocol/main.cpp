#include "left.h"


int main() {
	PC pc;
	pc.print();

	while (1) {
		switch (_getch()) {
		case 'r': pc.read(_getch(), _getch()); break;
		case 'w': pc.write(_getch(), _getch()); break;
		case 27: return 0;
		}
	}
}