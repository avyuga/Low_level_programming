all: lib.o dict.o main.o
	ld -o lab2 lib.o dict.o main.o

lib.o: lib.asm
	nasm -g lib.asm -felf64 -o lib.o
dict.o: dict.asm
	nasm -g dict.asm -felf64 -o dict.o
main.o: main.asm
	nasm -g main.asm -felf64 -o main.o
clean:
	rm -f main.o lib.o dict.o lab2	
