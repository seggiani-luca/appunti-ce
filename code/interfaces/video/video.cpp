#include <libce.h>
// dimensioni memoria video (2K)
#define SIZE 4000

namespace vid {
	// dichiara l'array video di libce
	extern volatile natw* video;
}

#define video vid::video

char mess[] = "- x86 rules ";
int cursor = 0;

// attributi carattere: 0x00001111-ASCII-
// significa bianco su sfondo nero
natl attr = 0x0F00;

// stampa una stringa
void prt_string(char* mess) {
	while(*mess != '\0') {
		video[cursor] = *mess | attr;
		cursor = (cursor + 1) % SIZE;
		mess++; 
	}
}

void main() {
	for(int i = 0; i < 167; i++) {
		prt_string(mess);
	}

	do {	
		// esci su ESC
		natb k = kbd::get_code();
		if(k == 0x01) break;
	} while(true);
}
