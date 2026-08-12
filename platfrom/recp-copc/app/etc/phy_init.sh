sw wr 0x10 1 0xc17e
sw wr 0x11 1 0xc17e
sw wr 0x12 1 0xc17e
sw wr 0x13 1 0xc17e
sw wr 0x14 1 0xc17e
sw wr 0x15 1 0xc17e
sw wr 0x16 1 0xc17e
sw wr 0x17 1 0xc17e
sw wr 0x18 1 0xc17e
sw wr 0x19 1 0xc17e


phyw 0 7 1e a000
phyw 0 7 1f 2
phyw 0 7 0 8140
phyw 0 7 1e a000
phyw 0 7 1f 0

phyw 1 3 1e a001
phyw 1 3 1f 8045
phyw 1 3 1e a000
phyw 1 3 1f 2
phyw 1 3 0 8140

phyw 0 3 1e a001
phyw 0 3 1f 8043
phyw 0 3 1e 51
phyw 0 3 1f 879
phyw 0 3 1e a000
phyw 0 3 1f 2
phyw 0 3 0 8140


cpld wr 0x20 0x1f
