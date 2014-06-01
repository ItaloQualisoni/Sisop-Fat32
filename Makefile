all:clean compile run hexdump

compile:
	gcc Fat.c -o fat32
run:
	./fat32
hexdump:
	hexdump -C fat.part

clean:
	rm -f *.part fat32