#include "song.h" // include libce
#define TIMESTEP 3

// indirizzi timer
const ioaddr timer0_addr = 0x40;
const ioaddr timer2_addr = 0x42;
const ioaddr cwr_addr = 0x43;
const ioaddr spr_addr = 0x61;

// frame corrente di esecuzione
natl frame = 0;

// imposta il divisore di un timer
void set_divisor(natw divisor, ioaddr addr) {
	// metti divisor in addr in 2 passate
	outputb(divisor, addr);
	outputb(divisor >> 8, addr);
}

// leggi il conteggio di un timer
natl read_timer(ioaddr addr) {
	// comando di latch
	outputb(0x00, cwr_addr);

	natb low = inputb(addr);
	natb high = inputb(addr);
	return (high << 8) | low;
}

// abilita lo speaker
void note_on() {
	outputb(3, spr_addr);
}

// disattiva lo speaker
void note_off() {
	outputb(0, spr_addr);
}

bool update_song() {
	song_frame cur_frame = song[frame];

	switch(cur_frame.mode) {
		case 0:
			// off
			note_off();
			break;
		case 1: {
			// on
			note_on();

			// divisore della nota
			natl divisor = cur_frame.get_divisor();
			set_divisor(divisor, timer2_addr);
			break;
		}
		case 2: {
			// legato
			natl divisor = cur_frame.get_divisor();
			set_divisor(divisor, timer2_addr);
			
			break;
		}
	}

	frame++;

	if(frame == length + 1) return false;
	return true;
}

void main() {
	// imposta il timer 0
	// outputb(0x36, cwr_addr); // modo 3
	natl div = 0; // 0 significa 65536
	set_divisor(div, timer0_addr);

	// imposta il timer 2
	outputb(0xB6, cwr_addr); // modo 3

	// i tick svolti
	natl tick = 0;
	natl next_song_update = 0;

	natl last_value = read_timer(timer0_addr);

	while (true) {
		natl current_value = read_timer(timer0_addr);

		// se il valore corrente è maggiore del valore precedente 
		// si è fatto un salto
		if (current_value > last_value) {
			tick++;
		}
	
		// aggiorna se necessario
		if(tick > next_song_update) {
			bool res = update_song();
			if(!res) break;

			// imposta il prossimo aggiornamento
			next_song_update += TIMESTEP;
		}

		last_value = current_value;
	}
}
