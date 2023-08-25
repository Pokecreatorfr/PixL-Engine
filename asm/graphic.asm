.section rodata

.global ddd
.align 4

ddd:
.incbin "graphics/ddd.png"

.global ddd_size
.align 4

ddd_size:
.int 0x00000000 + ddd_size - ddd


.section rodata

.global tileset1
.align 4

tileset1:
.incbin "graphics/tileset1.png"

.global tileset1_size
.align 4

tileset1_size:
.int 0x00000000 + tileset1_size - tileset1


