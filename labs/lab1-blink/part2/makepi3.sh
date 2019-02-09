    arm-none-eabi-as blink-pin20.s -o blink-pin20.o
    arm-none-eabi-ld -g -T linker.ld blink-pin20.o -o blink-pin20.elf -nostdlib
    arm-none-eabi-objcopy blink-pin20.elf -O binary blink-pin20.bin
    rm blink-pin20.o
