stm32F
103C8T6
9910X RR93


ruedi@ruedi-ThinkPad-T450:~/.platformio/packages/tool-openocd$ bin/openocd -c "set CPUTAPID 0x2ba01477" -f scripts/interface/stlink.cfg -f scripts/board/stm32f103c8_blue_pill.cfg -c "program /home/ruedi/git/stm32test/.pio/build/stm32F103C8/firmware.elf exit"

https://github.com/rogerclarkmelbourne/STM32duino-bootloader/blob/master/binaries/generic_boot20_pc13.bin

ruedi@ruedi-ThinkPad-T450:~/.platformio/packages/tool-openocd$ bin/openocd -c "set CPUTAPID 0x2ba01477" -f scripts/interface/stlink.cfg -f scripts/board/stm32f103c8_blue_pill.cfg -c "program /home/ruedi/Downloads/generic_boot20_pc13.bin 0x08000000 verify exit"
