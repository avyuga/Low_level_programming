%define buffer_size 255
%include "words.inc"

global _start
extern print_string, read_word, exit
extern find_word

section .data

input: db '>> Enter a word >> ', 0
key: db '>> Key >> ', 0
error: db '>> Error occured, word not found.',10, 0
buffer: times buffer_size db 0x0

section .text

_start:
	mov rdi, input ; printing a starting line
	call print_string
	
	mov rdi, buffer ; pointer to the buffer
	mov rsi, buffer_size ; amount of bytes to read (buffer size)
	call read_word
	
	mov rdi, rax  ; the word we are looking for in the dictionary
	mov rsi, last ; pointer to the first word in dictionary
	call find_word 

	test rax, rax ; if zero -> not found
	jz .not_found

	.found:
		push rax ; save the pointer
		mov rdi, key ; print '>> Key >>'
		call print_string
		pop rax ; restore pointer
		mov rdi, rax ; print the key
		call print_string
		call exit ; exit with code 0

	.not_found:
		mov rdi, error ; print error message
		call print_string
	
		mov rax, 60 ; exit with code 1
		mov rdi, 1
		syscall
		ret 
	
	
