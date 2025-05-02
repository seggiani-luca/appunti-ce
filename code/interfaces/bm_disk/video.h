// dimensioni memoria video (2K)
#define SIZE 2000

// registri cursore (adattatore CRT)
const natl cur_index = 0x03D4;
const natl cur_data = 0x03D5;

namespace vid {
	// dichiara l'array video di libce
	extern volatile natw* video;
}

#define video vid::video

// attributi carattere: 0x00001111-ASCII-
// significa bianco su sfondo nero
natl attr = 0x0F00;

// imposta cursore
void set_cursor(int pos) {
	// aggiorna posizione cursore adattatore grafico
	outputb(0x0F, cur_index);
	outputb(pos, cur_data);
	outputb(0x0E, cur_index);
	outputb(pos >> 8, cur_data);
}

// stampa una schermata video
void prt_screen(natb* screen, int size) {
	for(int i = 0; i < size; i++) {
		video[i] = screen[i] | attr;
	}
}
