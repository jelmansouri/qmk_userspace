# converting to RP2040ce/liatris
CONVERT_TO=liatris

CAPS_WORD_ENABLE = yes

ifdef CIRQUE_TRACKPAD
    OLED_ENABLE = no
	POINTING_DEVICE_ENABLE = yes
	POINTING_DEVICE_DRIVER = cirque_pinnacle_i2c
	SRC += trackpad.c
endif

ifdef OLED_DISPLAY
	# extra sources
	SRC +=  oled.c
endif
