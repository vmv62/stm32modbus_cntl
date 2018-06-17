# stm32modbus_cntl
mobdus controller

Библиотека для работы сдатчиками даллас. Библиотека использует аппаратный хуарт.
https://github.com/Mcublog/Control_bath/tree/master/Src

Сделать:
1 Вывести функцию отправки за библиотеку модбас
2 Улучшить функции обработки команд модбаса
 arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -g -nostartfiles -Wl,-Map=stm32_git_probe.map -O0 -Wl,--gc-sections -LC:\CooCox\CoIDE\configuration\ProgramData\stm32_git_probe -Wl,-TC:\CooCox\CoIDE\configuration\ProgramData\stm32_git_probe/arm-gcc-link.ld -g -o stm32_git_probe.elf ..\obj\hw_init.o ..\obj\main.o ..\obj\startup_stm32f0xx.o ..\obj\modbus.o

