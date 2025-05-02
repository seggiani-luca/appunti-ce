#include <libce.h>
#include "video.h"

// definiti in buffer.s
extern natl prd[]; // la tabella PRD
extern natb buffer[]; // il buffer dati
#define BUF_SIZE 512

/*
 * Interrupt
 */
// codice interruzione hard disk
#define HD_VECT 0x60

// handler
extern "C" void a_int();
extern "C" void c_int() {
	// fai l'acknowledge (passo 7)
	bm::ack(); // ack bus mastering
	hd::ack(); // ack disco
	apic::send_EOI(); 
}

void init_int() {
	// imposta l'IREQ 14 al codice HD_VECT
	apic::set_VECT(14, HD_VECT);
	// carica l'handler
	gate_init(HD_VECT, a_int);
	// smaschera l'IREQ 14
	apic::set_MIRQ(14, false);
}

/*
 * Disco rigido
 */

// registri disco
const ioaddr disk_buffer = 0x01F0;
const ioaddr disk_status = 0x01F7;
const ioaddr disk_sectors = 0x01F2;
const ioaddr disk_command = 0x01F7;

// registri indirizzo LBA (sarebbero SNR CNL CNH HND)
const ioaddr disk_lba0 = 0x01F3;
const ioaddr disk_lba1 = 0x01F4;
const ioaddr disk_lba2 = 0x01F5;
const ioaddr disk_lba3 = 0x01F6;

// indirizzo lba disco
natl lba = 1;

// dai indirizzo LBA al controllore disco
void give_lba(natl lba) {
	// dividi in 4 byte
	natb lba0 = lba;
	natb lba1 = lba << 8;
	natb lba2 = lba << 16;
	natb lba3 = lba << 24;

	// il byte piu' significativo deve attivare l'LBA,
	// lba stava comunque su 28 bit
	lba3 = (lba3 & 0x0F) | 0xE0; // 1110-LBA-

	outputb(lba0, disk_lba0);
	outputb(lba1, disk_lba1);
	outputb(lba2, disk_lba2);
	outputb(lba3, disk_lba3);
}

// dai comando al controllore disco
void give_command(natl lba, natb sectors, natb cmd) {
	give_lba(lba);
	outputb(sectors, disk_sectors);
	outputb(cmd, disk_command);
}

/*
 * Controller DMA 
 */

// indirizzo dispositivo bus mastering
natb bus = 0, dev = 0, fun = 0;

// inizializza dispositivo bus masetering
void init_bm() {
	bm::find(bus, dev, fun);
	bm::init(bus, dev, fun);
}

// prepara tabella PRD
void prepare_prd() {
	prd[0] = reinterpret_cast<natq>(buffer);
	         // byte EOT | dim. buffer
	prd[1] = 0x80000000 | (512UL & 0xFFFF);
}

// effettua un operazione disco in bus mastering 
void bm_op(bool write) {
	// il PRD e' gia pronto (passo 1)

	// carica il PRD (passo 2)
	bm::prepare(reinterpret_cast<paddr>(prd), write);
	
	// dai il comando (passo 3)
	give_command(lba, 1, write ? hd::WRITE_DMA : hd::READ_DMA);
	
	// inizia il bus mastering (passo 4)
	bm::start();

	// adesso il controllore DMA effettuera' i passi 5 e 6
}

/*
 * Console (video/tastiera) 
 */

// svuota il buffer
void init_buffer() {
	for(int i = 0; i < BUF_SIZE; i++) {
		buffer[i] = 0x00;
	}
}

// make code salva (1) e carica (2)
const natb save_code = 0x02; // sarebbe 1
const natb load_code = 0x03; // sarebbe 2

// make code esc 
natb esc_code = 0x01;

// make code backspace
natb back_code = 0x0E;

// cursore buffer testo
natl cursor = 0;

// sposta il cursore senza uscire dal buffer
inline void mov_cursor(int d) {
	if(cursor == 0 && d < 0) return; 
	
	cursor += d;
	if(cursor >= BUF_SIZE) cursor = BUF_SIZE - 1;
}

void main() {
	// inizializza il gestore di interrupt
	init_int();
	
	// attiva gli interrupt disco
	hd::enable_intr();
	
	// inizializza il controllore in bus mastering
	init_bm();

	// prepara il prd
	prepare_prd();

	// svuota il buffer
	init_buffer();

	// vai in un ciclo di lettura
	while(true) {
		// aggiorna schermo
		prt_screen(buffer, BUF_SIZE);
		set_cursor(cursor);

		// ottieni stato tastiera
		natb make_code = kbd::get_code();

		if(make_code == esc_code) break;
		if(make_code == back_code) {
			mov_cursor(-1); 
			buffer[cursor] = 0x00;
			continue;
		}

		if(make_code == save_code) {
			bm_op(true); // scrivi
			continue;
		}
		if(make_code == load_code) {
			bm_op(false); // leggi
			continue;
		}
		
		char c = kbd::conv(make_code);
		if(c != '\0') {
			buffer[cursor] = c;
			mov_cursor(1);
		}
	}
}
