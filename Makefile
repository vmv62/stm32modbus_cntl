compile:
	arm-none-eabi-gcc -std=gnu99 -g -O2 -specs=nosys.specs -Wall -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m0 -fsingle-precision-constant -Wdouble-promotion main.c hw_init.c modbus.c -o main.elf
