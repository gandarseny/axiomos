# 🌌 AxiomOS Adrift (v1.0)

AxiomOS Adrift is a minimal experimental operating system written in C with a custom kernel, shell, RAM filesystem, and calculator.

It boots using Multiboot and runs in 32-bit protected mode.

---

## 🚀 Features

### System
- Multiboot kernel
- VGA text output
- Custom keyboard input
- Interactive shell (`>SPACE>`)

### Filesystem
- RAM-based filesystem
- `write=name=content`
- `see=name`
- `flst`

### Tools
- Built-in calculator (`calculate`)
- System commands (`pulse`, `about`, `glyphs`, `clearx`, `haltx`)

---

## ⚙️ Requirements

Install dependencies (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install gcc-multilib grub-pc-bin xorriso qemu-system-x86
````

---

## 🔨 Build Instructions

### 1. Compile kernel

```bash
gcc -m32 -ffreestanding -fno-pic -c src/kernel.c -o build/kernel.o
```

### 2. Link kernel

```bash
ld -m elf_i386 -Ttext 0x100000 -o iso/boot/kernel.elf build/kernel.o --oformat binary
```


### 3. Create ISO

```bash
grub-mkrescue -o axiomos.iso iso
```

### 4. Run

```bash
qemu-system-i386 -cdrom axiomos.iso
```

---

## 💻 Commands

### System

* `pulse` → system heartbeat
* `about` → system info
* `glyphs` → list commands
* `clearx` → clear screen
* `haltx` → stop CPU

---

### Filesystem

Format:

```
write=name=content
```

Examples:

```bash
write=test=hello
see=test
flst
```

---

### Calculator


Start:

```bash
calculate
```

Operators:

* `p` = +
* `m` = *
* `-` = -
* `/` = /

Example:

```
12p3
```

---

## ⚠️ Warning

This OS is experimental and should only be run in virtual machines.

Do not run on real hardware unless you know what you are doing.
