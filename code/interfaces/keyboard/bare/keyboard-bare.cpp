#include <libce.h>

#define NUM_CODES 28

// funzione di input byte
extern "C" natb myinputb(ioaddr addr);

// indirizzi porte tastiera
const ioaddr rbr_addr = 0x60;
const ioaddr str_addr = 0x64;

// tabella make code
natb make_codes[] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
	0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
	0x2c, 0xd, 0x2e, 0x2f, 0x30, 0x31, 0x32,
	0x1c, 0x39
};

// tabella caratteri minuscoli
char l_table[] = {
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	'\n', ' '
};

// tabella caratteri maiuscoli
char u_table[] = {
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'\n', ' '
};

// make code ESC
natb esc_code = 0x01;

// make code shfit
natb shift_down = 0x2A;
natb shift_up = 0xAA;

// gestione case
enum cas { lower, upper };
cas cur_cas = lower;

natb get_key() {
	natb status;
	
	// ciclo di lettura per il flag FI in str_addr
	do {
		status = myinputb(str_addr);
	} while(!(status & 0x01));
	
	// FI alto, leggi da rbr_addr
	return myinputb(rbr_addr);
}

char get_char(natb make_code) {
	// cerca il carattere per scansione lineare
	for(int i = 0; i < NUM_CODES; i++) {
		if(make_code == make_codes[i]) {
			// trovato, controlla il case corrente
			if(cur_cas == upper) {	
				return u_table[i];
			} else {
				return l_table[i];
			}
		}
	}

	// carattere nullo come default
	return '\0';
}

void main() {
	while(true) {
		// otteni make code
		natb make_code = get_key();
		
		// se ESC, esci
		if(make_code == esc_code) {
			break;
		}

		// gestisci shift
		if(make_code == shift_down) {
			cur_cas = upper;
		}
		if(make_code == shift_up) {
			cur_cas = lower;
		}

		// ottieni carattere e stampa
		char c = get_char(make_code);
		vid::char_write(c);
	}
}
