#

CC=wnfc
ASM=atasm

OS_NAME = $(OSNAME)
ifeq ("${OS_NAME}", "Linux")
  ALTIRRA=wine /home/lars/.wine/drive_c/atarixl.software/AtariXL_400/Altirra-410b5.exe
else
  ALTIRRA=Altirra.exe
endif

WNFC_OPTIONS='-O 2'

FIRMWARE=../../firmware
COMPILER=..

.PHONY: copy_atari_files prepare_atari clean all prepare basic_tests

# You need to give the "PROGRAM name" name out of wnf file here, add .65o

MULTI_SPRITE_OBJ_FILES=\
DEMO16SP.65o \
COUNT.65o

INCLUDES=\
SPRITES.INC \
SPRITE-DATA.INC

ADDITIONALS=\
autoload-test-runner.lst \
test-runner-d.lst \
no-color-switch.com \
Turbo-Basic_XL_2.0.com \
.os.txt \
AUTORUN.TUR

all: $(INCLUDES) $(MULTI_SPRITE_OBJ_FILES) multi_sprite_tests

clean::
	rm -f .wnffiles.txt
	rm -f $(ADDITIONALS) $(MULTI_SPRITE_OBJ_FILES) $(INCLUDES)
	rm -f *.COM *.ASM *.65o .insert.txt
	rm -f *.lab *.out
	rm -f DEMO16SP.ASM.inc DEMO16SP.lst *.log
	rm -f SPRTDATA.INC *.atr
	rm -f COUNT.ASM.inc COUNT.lst

clean::
	rm -f start-multi-sprite.atr

DEMO16SP.65o: demo-16-sprites.wnf displaylist.INC dli.INC $(INCLUDES) header.wnf
	$(CC) $< $(WNFC_OPTIONS) -I $(COMPILER)
	$(ASM) -ha -s $(@:.65o=.ASM) -g$(@:.65o=.lst) -l$(@:.65o=.lab) >$(@:.65o=.log)
	cp $@ $(@:.65o=.COM)

COUNT.65o: count.wnf header.wnf
	$(CC) $< $(WNFC_OPTIONS) -I $(COMPILER)
	$(ASM) -ha -s $(@:.65o=.ASM) -g$(@:.65o=.lst) -l$(@:.65o=.lab) >$(@:.65o=.log)
	cp $@ $(@:.65o=.COM)

SPRITES.INC: all-sprites.wnf header.wnf
	$(CC) $< $(WNFC_OPTIONS) -I $(COMPILER)

SPRITE-DATA.INC: sprite-data.wnf header.wnf
	$(CC) $< $(WNFC_OPTIONS) -I $(COMPILER)

prepare_atari ::
#	echo "COUNT.COM -> AUTORUN.SYS" >>.insert.txt
	echo "DEMO16SP.COM" >>.insert.txt
	echo "COUNT.COM" >>.insert.txt
	echo "DEMO16SP.COM -> AUTORUN.SYS" >>.insert.txt

multi_sprite_disk: prepare_atari
	xldir $(COMPILER)/starter17.atr insert .insert.txt start-multi-sprite.atr

atari_multi_sprite: multi_sprite_disk
	atari800 \
    -hreadwrite \
    -H2 /tmp/atari \
    -xlxe_rom $(FIRMWARE)/ATARIXL.ROM \
    -xl -xl-rev 2 \
    -pal -showspeed -nobasic \
    -windowed -win-width 682 -win-height 482 \
    -vsync -video-accel \
    ${ATARI800_OPTIONS} \
   start-multi-sprite.atr

# Start Altirra with debug and stop at start address
# /portable searchs for an Altirra.ini
debug: $(INCLUDES) $(MULTI_SPRITE_OBJ_FILES) multi_sprite_disk
	$(ALTIRRA) \
	/portable \
	/debug \
	/debugcmd: ".sourcemode on" \
	/debugcmd: ".loadsym DEMO16SP.lst" \
	/debugcmd: ".loadsym DEMO16SP.lab" \
	/debugcmd: "bp 4000" \
	/disk "start-multi-sprite.atr"

prepare:
	rm -f .insert.txt

multi_sprite_tests: prepare $(MULTI_SPRITE_OBJ_FILES) atari_multi_sprite

start: all
