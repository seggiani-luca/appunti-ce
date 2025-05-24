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
	// teoricamente vogliamo leggere il timer 0,
	// quindi dovremmo inviare la Read Back Word
	// 0100_0111 (ottieni stato t. 0)
	// questa pero' non sembra essere supportata,
	// quindi usiamo il comando di latch 0x00
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

	vid::str_write("frame: ");
	print_natl(frame);

	switch(cur_frame.mode) {
		case 0:
			// off
			vid::str_write(" - off");

			note_off();
			break;
		case 1: {
			// on
			// divisore della nota
			natl divisor = cur_frame.get_divisor();
			
			vid::str_write(" - on - note: ");
			vid::str_write(cur_frame.note);
			vid::str_write(" divisor: ");
			print_natl(divisor);
			
			note_on();

			set_divisor(divisor, timer2_addr);
			break;
		}
		case 2: {
			// legato
			vid::str_write(" - legato");
			
			natl divisor = cur_frame.get_divisor();
			set_divisor(divisor, timer2_addr);
			
			break;
		}
	}

	vid::char_write('\n');

	frame++;

	if(frame == length + 1) return false;
	return true;
}

// temporizzazione
natl tick = 0;
natl next_song_update = 0;

bool fine = false;

// interruzioni
#define TIM_VECT 0x20
#define KBD_VECT 0x21

extern "C" void a_timer();
extern "C" void c_timer() {
	tick++;
	
	// aggiorna se necessario
	if(tick > next_song_update) {
		bool res = update_song();
		if(!res) fine = true; 

		// imposta il prossimo aggiornamento
		next_song_update += TIMESTEP;
	}

	apic::send_EOI();
}

extern "C" void a_keyb();
extern "C" void c_keyb() {
	natb code = kbd::get_code();
	
	if(code == 0x01) fine = true;

	apic::send_EOI();
}

void main() {
	// imposta il timer 0
	// outputb(0x36, cwr_addr); // modo 3
	natl div = 0; // 0 significa 65536
	set_divisor(div, timer0_addr);

	// imposta il timer 2
	outputb(0xB6, cwr_addr); // modo 3

	// imposta l'APIC
	// timer
	apic::set_VECT(2, TIM_VECT);
	apic::set_TRGM(2, false);
	apic::set_MIRQ(2, false);
	
	// tastiera
	apic::set_VECT(1, KBD_VECT);
	apic::set_TRGM(1, false);
	apic::set_MIRQ(1, false);

	// imposta IDT
	gate_init(TIM_VECT, a_timer);

	kbd::enable_intr();
	gate_init(KBD_VECT, a_keyb);

	while(!fine);
}
