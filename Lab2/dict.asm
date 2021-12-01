global find_word
extern string_equals
extern print_string

section .text

find_word:
; rdi - pointer to null-terminated string
; rsi - pointer to the dictionary's first word
; return address of the word in dictionary, else 0
	.loop:
		test rsi, rsi ; if the pointer's null -> not found
		jz .not_found
		push rdi ; save rdi
		push rsi ; save rsi
		add rsi, 8 ; each 8 bytes contain a word
		call string_equals ; compare string values
		pop rsi ; restore rdi
		pop rdi ; restore rsi
		cmp rax, 1 ; if rax == 1 -> found
		je .found
		mov rsi, [rsi] ; move the beginning of the dictionary to the next word
		jmp .loop
	.found:
		mov rax, rsi ; put the address into rax
	.not_found:
		ret ; if not found -> rsi = 0

