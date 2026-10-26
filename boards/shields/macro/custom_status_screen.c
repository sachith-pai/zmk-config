/*
 * Custom status screen for "Macro" (SSD1306 128x64, mono, ZMK v0.3 / LVGL 8).
 *
 *  +------------------------------------+
 *  | [BT 1 ok]                    85%   |   built-in output + battery widgets
 *  |------------------------------------|   divider
 *  |    |<<          mute          >>|   |   icons   (change with active layer)
 *  |    PREV         MUTE         NEXT  |   captions (change with active layer)
 *  +------------------------------------+
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>

#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

/* ---------- layout (pixels) ---------- */
#define SCREEN_W 128
#define DIVIDER_Y 17
#define ICON_Y 22
#define CAPTION_Y 50
#define NUM_KEYS 3
#define COL_W (SCREEN_W / NUM_KEYS) /* 42 */

/* ---------- per-layer key labels ----------
 * Index == layer index in macro.keymap:
 *   0 = YT Music, 1 = Media, 2 = Misc
 * Only the 3 plain keys are shown; the dial click is not.
 * If you reorder / add layers in the keymap, edit this table to match.
 */
struct key_label {
    const char *icon;
    const char *caption;
};

struct layer_keys {
    struct key_label key[NUM_KEYS];
};

static const struct layer_keys layer_table[] = {
    [0] = {{{LV_SYMBOL_PREV, "PREV"}, {LV_SYMBOL_MUTE, "MUTE"}, {LV_SYMBOL_NEXT, "NEXT"}}},
    [1] = {{{"F1", "F1"}, {"F2", "F2"}, {LV_SYMBOL_SETTINGS, "UNLCK"}}},
    [2] = {{{LV_SYMBOL_TRASH, "BTCLR"}, {LV_SYMBOL_BLUETOOTH, "BTNXT"}, {LV_SYMBOL_SETTINGS, "UNLCK"}}},
};

static const struct layer_keys layer_fallback = {{{"-", "-"}, {"-", "-"}, {"-", "-"}}};

/* ---------- widgets ---------- */
static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_battery_status battery_status_widget;

static lv_obj_t *icon_lbl[NUM_KEYS];
static lv_obj_t *caption_lbl[NUM_KEYS];

struct keys_state {
    zmk_keymap_layer_index_t index;
};

static void keys_update_cb(struct keys_state state) {
    const struct layer_keys *l =
        (state.index < ARRAY_SIZE(layer_table)) ? &layer_table[state.index] : &layer_fallback;

    for (int i = 0; i < NUM_KEYS; i++) {
        lv_label_set_text(icon_lbl[i], l->key[i].icon);
        lv_label_set_text(caption_lbl[i], l->key[i].caption);
    }
}

static struct keys_state keys_get_state(const zmk_event_t *eh) {
    return (struct keys_state){.index = zmk_keymap_highest_layer_active()};
}

/* Same pattern as ZMK's own layer_status.c: state is fetched on the event thread,
 * LVGL calls run on the display work queue. */
ZMK_DISPLAY_WIDGET_LISTENER(widget_keys, struct keys_state, keys_update_cb, keys_get_state)
ZMK_SUBSCRIPTION(widget_keys, zmk_layer_state_changed);

static lv_obj_t *make_column_label(lv_obj_t *parent, int col, int y, const lv_font_t *font) {
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(l, COL_W);
    lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(l, font, LV_PART_MAIN);
    lv_obj_set_pos(l, col * COL_W, y);
    return l;
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    const lv_font_t *small = lv_theme_get_font_small(screen);

    /* top bar: BT/USB output (left), battery (right) */
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_output_status_obj(&output_status_widget), small,
                               LV_PART_MAIN);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_battery_status_obj(&battery_status_widget), small,
                               LV_PART_MAIN);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);

    /* divider: 1px bar in the theme's foreground colour (so it survives DISPLAY_INVERT) */
    lv_obj_t *line = lv_obj_create(screen);
    lv_obj_remove_style_all(line);
    lv_obj_set_size(line, SCREEN_W, 1);
    lv_obj_set_pos(line, 0, DIVIDER_Y);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(line, lv_obj_get_style_text_color(line, LV_PART_MAIN), LV_PART_MAIN);

    /* 3 columns: big icon on top, small caption below */
    for (int i = 0; i < NUM_KEYS; i++) {
        icon_lbl[i] = make_column_label(screen, i, ICON_Y, &lv_font_montserrat_20);
        caption_lbl[i] = make_column_label(screen, i, CAPTION_Y, small);
    }

    widget_keys_init(); /* must come after the labels exist */

    return screen;
}
