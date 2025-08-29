# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules

MOUSEKEY_ENABLE = yes
CAPS_WORD_ENABLE = yes

SRC += rgb_matrix.c

ifdef HLC_TFT_DISPLAY
	SRC += display.c
	SRC += graphics/modifiers/fallback.qgf.c
	SRC += graphics/modifiers/fallback.qgf.c
	SRC += graphics/modifiers/caps_word_enabled.qgf.c
	SRC += graphics/modifiers/caps_word_shift_pressed.qgf.c
	SRC += graphics/modifiers/command_pressed.qgf.c
	SRC += graphics/modifiers/command_unpressed.qgf.c
	SRC += graphics/modifiers/control_pressed.qgf.c
	SRC += graphics/modifiers/control_unpressed.qgf.c
	SRC += graphics/modifiers/option_pressed.qgf.c
	SRC += graphics/modifiers/option_unpressed.qgf.c
	SRC += graphics/modifiers/shift_pressed.qgf.c
	SRC += graphics/modifiers/shift_unpressed.qgf.c

	SRC += graphics/layers/base_layer_icon_colored.qgf.c
	SRC += graphics/layers/base_layer_text_colored.qgf.c
	SRC += graphics/layers/lower_layer_icon_colored.qgf.c
	SRC += graphics/layers/lower_layer_text_colored.qgf.c
	SRC += graphics/layers/nav3d_layer_icon_colored.qgf.c
	SRC += graphics/layers/nav3d_layer_text_colored.qgf.c
	SRC += graphics/layers/raise_layer_icon_colored.qgf.c
	SRC += graphics/layers/raise_layer_text_colored.qgf.c

	# SRC += graphics/layers/base_layer_icon.qgf.c
	# SRC += graphics/layers/base_layer_text.qgf.c
	# SRC += graphics/layers/lower_layer_icon.qgf.c
	# SRC += graphics/layers/lower_layer_text.qgf.c
	# SRC += graphics/layers/nav3d_layer_icon.qgf.c
	# SRC += graphics/layers/nav3d_layer_text.qgf.c
	# SRC += graphics/layers/raise_layer_icon.qgf.c
	# SRC += graphics/layers/raise_layer_text.qgf.c
	SRC += graphics/layers/undefined_layer_icon.qgf.c
	SRC += graphics/layers/undefined_layer_text.qgf.c
endif

ifdef HLC_CIRQUE_TRACKPAD
	SRC += trackpad.c
endif
