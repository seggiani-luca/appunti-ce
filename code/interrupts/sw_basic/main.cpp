#include <libce.h>

extern "C" void a_handler();
extern "C" void c_handler() {
	printf("sto gestendo l'interruzione\n");
}

void main() {
	// imposta il gestore
	// extern "C" void gate_init(natb num, void routine(), bool trap, int liv)
	gate_init(0x40, a_handler, false, 3);
		
	// chiama l'interruzione interna	
	asm("int $0x40");
	pause();

	return;
}
