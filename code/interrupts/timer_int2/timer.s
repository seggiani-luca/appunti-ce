#include <libce.h>

.extern c_timer
.global a_timer
a_timer:
	salva_registri
	call c_timer 
	carica_registri
	iretq

.extern c_keyb
.global a_keyb
a_keyb:
	salva_registri
	call c_keyb 
	carica_registri
	iretq
