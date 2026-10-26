# Compiles the custom OLED status screen into the ZMK app target.
target_sources_ifdef(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM app PRIVATE src/custom_status_screen.c)
