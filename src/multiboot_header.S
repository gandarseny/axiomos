.section .multiboot
.align 4
.long 0x1BADB002        # multiboot magic
.long 0x00010003        # flags (memory info, video)
.long -(0x1BADB002 + 0x00010003)
