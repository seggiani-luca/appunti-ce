#include <libce.h>
#include "keyboard.h"
#include "video.h" // definisce il buffer video
#define BUF_SIZE 512

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

// aspetta il disco
void wait_for_disk() {
	natb s;
	do {
		s = inputb(disk_status);
	} while ((s & 0x88) != 0x08);
}

// scrivi un settore sul disco
void write_sector(natb* sector) {
	wait_for_disk();

	// reinterpret_cast per mandare 2 byte per volta (ripetuti a 256 * 2 = 512)
	outputbw(reinterpret_cast<natw*>(sector), 256, disk_buffer);
}

// leggi un settore dal disco
void read_sector(natb* sector) {
	wait_for_disk();

	// come sopra
	inputbw(disk_buffer, reinterpret_cast<natw*>(sector), 256);
}

// make code salva (1) e carica (2)
const natb save_code = 0x02;
const natb load_code = 0x03;

// indirizzo lba disco
natl lba = 1;

// buffer testo
natb buffer[BUF_SIZE];

// svuota il buffer
void init_buffer() {
	for(int i = 0; i < BUF_SIZE; i++) {
		buffer[i] = 0x00;
	}
}

// cursore buffer testo
natl cursor = 0;

// sposta il cursore senza uscire dal buffer
inline void mov_cursor(int d) {
	if(cursor == 0 && d < 0) return; 
	
	cursor += d;
	if(cursor >= BUF_SIZE) cursor = BUF_SIZE - 1;
}

// salva buffer testo
void save() {
	give_command(lba, 1, hd::WRITE_SECT);
	write_sector(buffer);
}

// carica buffer testo
void load() {
	give_command(lba, 1, hd::READ_SECT);
	read_sector(buffer);
}

void main() {
	// inizia svuotando il buffer
	init_buffer();

	// vai in un ciclo di lettura
	while(true) {
		natb make_code = get_key();

		if(make_code == esc_code) break;
		if(make_code == back_code) mov_cursor(-1);

		if(make_code == save_code) save();
		if(make_code == load_code) load();
		
		char c = get_char(make_code);
		if(c != '\0') {
			buffer[cursor] = c;
			mov_cursor(1);
		}

		// aggiorna schermo
		prt_screen(buffer, BUF_SIZE);
		set_cursor(cursor);
	}
}
