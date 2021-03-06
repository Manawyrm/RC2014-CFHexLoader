SRCS=$(wildcard */*.c) $(wildcard *.c)
CC = zcc
CFLAGS = +rc2014-manawyrm -subtype=none -vn -SO3 -O3  --opt-code-size -clib=sdcc_iy -pragma-define:__CRTCFG=-1
#  --max-allocs-per-node200000

all:
	$(CC) $(CFLAGS) --list $(SRCS) -o main -create-app

.PHONY clean:
	rm -f *.bin *.lst *.ihx *.hex *.obj *.rom *.lis zcc_opt.def $(APP_NAME) *.reloc *.sym *.map disasm.txt

run: 
	cat main.ihx | python slowprint.py > /dev/ttyUSB0

build: 
	losetup -o 1024 /dev/loop0 /home/tobias/Entwicklung/Code/RC2014/ROMs/cfbootloader/my.cf
	sudo mount /dev/loop0 /mnt/root/
	sudo cp main.bin /mnt/root/MANDEL.BIN
	sudo umount /dev/loop0
	losetup -d /dev/loop0
