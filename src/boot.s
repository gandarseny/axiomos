section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x3)

section .text
global _start
extern kernel_main

_start:
    cli
    call kernel_main
    hlt