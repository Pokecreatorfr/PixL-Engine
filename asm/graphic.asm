.section rodata

.global snow0
.align 4

snow0:
.incbin "graphics/snow0.png"

.global snow0_size
.align 4

snow0_size:
.int 0x00000000 + snow0_size - snow0


.section rodata

.global tileset1
.align 4

tileset1:
.incbin "graphics/tileset1.png"

.global tileset1_size
.align 4

tileset1_size:
.int 0x00000000 + tileset1_size - tileset1


