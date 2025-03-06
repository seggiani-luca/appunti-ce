.global myinputb

myinputb:
	push %rdx

	# l'indirizzo e' su %rdi
	mov %rdi, %rdx
	# il ritorno va in %al
	inb %rdx, %al
	
	pop %rdx
	ret
