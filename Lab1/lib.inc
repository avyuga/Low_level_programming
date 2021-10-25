section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
	mov rax, 60 ; EXIT syscall number
	mov rdi, 0 ; exit code 0
	syscall
	ret 

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
	xor rax, rax 	; set rcx = length = 0
	.loop:
		cmp byte[rdi+rax], 0 ; comparing bit #rdi+rcx with zero
		jz .stop ; if zero leave
		inc rax ; else increment rcx
		jmp .loop
	.stop:
		ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    	call string_length ; the number is in rcx
	mov rdx, rax ; put string_length from rax here
	mov rax, 1 ; WRITE syscall number
	mov rsi, rdi ; put the output string aka argument in rsi from rdi
	push rdi ; save the output string in stack
	mov rdi, 1 ; STDOUT descriptor
	syscall
	pop rdi ; get the string back
	ret	


; Принимает код символа и выводит его в stdout
print_char:
	mov rax, 1 ; WRITE syscall number
	push rdi ; put an argument into stack
	mov rdi, 1 ; STDOUT descriptor
	mov rsi, rsp ; take it into rsi from stack
	mov rdx, 1 ; length for one symbol
	syscall
	pop rdi ; get the char back
	ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
	mov rdi, 0xA ; \n symbol number
	call print_char
	ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov r8, 10 ; put the base of numeral system
	mov rax, rdi ; put the argument in register appropriate for division
	mov rcx, rsp ; save the stack in rcx
	dec rsp
	mov byte[rsp], 0 ; put smallest byte of \0 into the stack
	.loop:
		xor rdx, rdx ; rdx=0
		div r8 ; the last number is in rdx
		add rdx, 0x30 ; transfer into ascii
		dec rsp
		mov byte[rsp], dl ; put the number(the smallest bit) in stack
		test rax, rax
		jnz .loop
	mov rdi, rsp
	push rcx
	call print_string
	pop rcx
	mov rsp, rcx
	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
	cmp rdi, 0 ; compare the number with 0
	jge .positive ; jump to .positive if signed comparison says >=0
	push rdi ; save rdi
	mov rdi, 0x2d ; put the '-' into rdi
	call print_char	; print it
	pop rdi ; get rdi back
	neg rdi ; rdi=-rdi
	.positive: 
		push rdi
		call print_uint ; print abs(rdi)
		pop rdi
		ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
	xor rax, rax
	push r12
	.counting_and_comparing_lengths:
		push rax ;save rcx
		mov r8, rdi ; store rdi
		call string_length ; calling for rdi, its length in rcx
		mov r10, rax ; rdi's length
		mov r9, rsi ; store rsi
		mov rdi, rsi ; swap it so string_length would work
		call string_length ; calling for rsi, its length in rcx
		mov r11, rax ; rsi's length
		pop rax ; get rcx back
		cmp r10, r11 ; comparing lengths
		jne .stop ; if not equal -> stop
	.compare_by_symbol:
		xor rdx, rdx ; rdx=0
		.loop:
			cmp byte[r8+rdx], 0 ; find out if this is the end of line
			je .reached_the_end ; if reached without corrupting, then return  1
			mov r12b, byte[r9+rdx] 
			cmp byte[r8+rdx], r12b ; compare bytes						
			jne .stop ; if not equal, stop
			inc rdx ; rdx++
			jmp .loop 
	.reached_the_end:
		mov rax, 1
	.stop:
		pop r12
		ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char: 
	mov rax, 0 ; READ syscall number
	mov rdi, 0 ; STDIN descriptor	
	mov rdx, 1 ; reading 1 symbol
	dec rsp
	mov rsi, rsp ; read to stack
	syscall

	cmp rax, 0
	jne .stop
	mov byte[rsi], 0
	.stop:
		xor rax, rax
		mov al, byte[rsp]
		inc rsp
		ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор


read_word:
	; rdi - pointer to the memory
	; rsi - amount of bytes to read
	xor r8,r8  ; counter of the bytes already read
	xor rax, rax
	.loop:
		push rdi ; save anything that can be crashed in read_char
		push rdx
		push rsi
		call read_char ; read a character
		pop rsi ; get everything saved back
		pop rdx
		pop rdi
		cmp al, 0x20 ; is it a whitespace?
		je .whitespace
		cmp al, 0xA ; is it a newline?
		je .whitespace
		cmp al, 0x9 ; is it a tabulation?
		je .whitespace
		cmp al, 0 ; maybe end of line?
		je .stop
		inc r8 ; counter++ if ok
	
		cmp r8, rsi ; compare the bytes read and amount of bytes possible to read
		jg .overflow ; if greater -> leave and return rax=0

		mov byte[rdi+r8-1], al ; write the byte read to memory
		jmp .loop
	.whitespace:
		cmp r8, 0
		je .loop
		cmp r8, 0
		jg .stop
	.overflow:
		mov rax, 0
		ret
	.stop:	
		mov byte[rdi+r8], 0	
		mov rax, rdi
		mov rdx, r8
		ret
 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	; rdi - pointer to the string
	xor r8, r8 ; counter
	xor rax, rax ; result will be here
	mov r9, 10 ; base of numeral system
	.loop:
		xor rcx, rcx
		mov cl, byte[rdi+r8] ; reading the next byte of a string		

		cmp cl, '0' ; if it is not a number char, then error
		jl .stop
		cmp cl, '9'
		jg .stop

		sub cl, '0' ; make it a number
		mul r9 ; multiply result of the previous step on the base of numeral system
		add rax, rcx ; add the number read to it
		inc r8 ; counter++
		jmp .loop

	.stop:
		; the number is already in rax
		mov rdx, r8 ; number of chars
		ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
	; rdi - pointer to the string
	xor r8, r8 ; counter
	xor rax, rax ; result will be here
	mov cl, byte[rdi] ; the first char of the string
	cmp cl, '-' ; if the first symbol is '-' -> negative
	je .negative 
	cmp cl, '0' ; if not a number -> error
	jl .error
	cmp cl, '9'
	jg .error
	call parse_uint ; if reached -> parse unsigned int
	ret
	.negative:
		inc rdi ; move the pointer to the 2nd symbol
		call parse_uint ; parse unsigned
		neg rax ; inverse the result
		inc rdx ; increase char counter
		ret
	.error:
		xor rdx, rdx
		ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
	; rdi - pointer to the string
	; rsi - pointer to the buffer
	; rdx - buffer length
	call string_length ; length of string is in rax
	inc rax ; need space for \0
	cmp rax, rdx ; compare string length and size of buffer
	jg .stop 
	xor r8, r8
	xor r9, r9
	dec rax
	.loop:
		cmp r8, rax
		je .success
		mov r9b, byte[rdi+r8]
		mov byte[rsi+r8], r9b
		inc r8
		jmp .loop
	.success:
		mov byte[rsi+rax], 0
		ret
	.stop:
		xor rax, rax
		ret
