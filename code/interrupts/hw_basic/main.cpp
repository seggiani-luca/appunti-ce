#include <libce.h>
#define KBD_VECT 0x20

bool fine = false;

extern "C" void a_keyboard();
extern "C" void c_keyboard() {
	// leggiamo da tastiera
	natb code = kbd::get_code();
	
	if(code == 0x01) fine = true;

	char c = kbd::conv(code);
	vid::char_write(c);

	apic::send_EOI();
}

void main() {
	// attiva le interruzioni tastiera
	kbd::enable_intr();
	
	// imposta l'APIC
	apic::set_VECT(1, KBD_VECT);
	apic::set_TRGM(1, false); // false: fronte, true: livello
	apic::set_MIRQ(1, false);
	
	// imposta il gate nella IDT
	gate_init(KBD_VECT, a_keyboard);

	while(!fine);

	return;	
}
