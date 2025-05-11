.global _start
_start:
	
	lui	 a5, 0x4080d
	li   a4, 1
	sb	 a4, 0x4e8(a5)
	lui  ra, 0x4200b
	addi ra, ra, 0x788
	ret
	
