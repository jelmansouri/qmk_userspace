# converting to RP2040ce/liatris
CONVERT_TO=liatris

CAPS_WORD_ENABLE = yes

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c

# extra sources
SRC += oled.c
SRC += rgb_matrix.c
SRC += trackpad.c
