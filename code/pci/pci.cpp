#include <libce.h>

bool check_dev(natb bus, natb dev, natb fun) {
	// entrate tabella configurazione
	natw vendorID, deviceID;
	natb class_code;

	// ottieni vendor ID
	vendorID = pci::read_confw(bus, dev, fun, 0);

	// 0xffff significa inesistente
	if(vendorID == 0xffff) return false;
	
	// ottieni device ID e codice classe
	deviceID = pci::read_confw(bus, dev, fun, 2);
	class_code = pci::read_confb(bus, dev, fun, 11);

	// stampa informazioni dispositivo
	printf("%02x:%02x.%1d   %04x:%04x [%s]\n",
		bus, dev, 0,
		vendorID, deviceID,
		pci::decode_class(class_code)); // decodifica il codice classe
	
	return true;
}

void main() {
	for(natb bus = 0; bus < 100; bus++) { // 100 bastano
		for(natb dev = 0; dev < 32; dev++) {
			// cerchiamo il dispostivo bus:dev:xxx
			if(!check_dev(bus, dev, 0)) continue;
		
			// controlla le altre funzioni
			for(natb fun = 1; fun < 8; fun++) {
				check_dev(bus, dev, fun);
			}
		}
	}

	pause();
}
