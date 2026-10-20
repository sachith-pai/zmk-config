#include <zephyr/kernel.h>
#include <lvgl.h>
#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/layer_status.h>

static struct zmk_widget_output_status output_widget;
static struct zmk_widget_battery_status battery_widget;
static struct zmk_widget_layer_status layer_widget;

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, 128, 64);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    /* -------------------------------------------------------------
     * 1. TOP STATUS BAR (Inverted: Solid Background, Dark Text)
     * ------------------------------------------------------------- */
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 128, 14);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 1, 0);

    // Connection status (USB / BLE profile) inside header
    zmk_widget_output_status_init(&output_widget, header);
    lv_obj_t *out_obj = zmk_widget_output_status_obj(&output_widget);
    lv_obj_align(out_obj, LV_ALIGN_LEFT_MID, 2, 0);

    // Battery widget inside header
    zmk_widget_battery_status_init(&battery_widget, header);
    lv_obj_t *batt_obj = zmk_widget_battery_status_obj(&battery_widget);
    lv_obj_align(batt_obj, LV_ALIGN_RIGHT_MID, -2, 0);

    /* -------------------------------------------------------------
     * 2. MAIN SECTION: ACTIVE LAYER CARD (Left Side)
     * ------------------------------------------------------------- */
    lv_obj_t *layer_box = lv_obj_create(screen);
    lv_obj_set_size(layer_box, 68, 46);
    lv_obj_align(layer_box, LV_ALIGN_BOTTOM_LEFT, 2, -2);
    lv_obj_set_style_border_color(layer_box, lv_color_white(), 0);
    lv_obj_set_style_border_width(layer_box, 1, 0);
    lv_obj_set_style_radius(layer_box, 3, 0);
    lv_obj_set_style_pad_all(layer_box, 2, 0);
    lv_obj_clear_flag(layer_box, LV_OBJ_FLAG_SCROLLABLE);

    // Mini sub-heading
    lv_obj_t *layer_title = lv_label_create(layer_box);
    lv_label_set_text(layer_title, "LAYER");
    lv_obj_align(layer_title, LV_ALIGN_TOP_LEFT, 1, 0);

    // Dynamic layer name
    zmk_widget_layer_status_init(&layer_widget, layer_box);
    lv_obj_t *layer_obj = zmk_widget_layer_status_obj(&layer_widget);
    lv_obj_align(layer_obj, LV_ALIGN_BOTTOM_MID, 0, -2);

    /* -------------------------------------------------------------
     * 3. AUXILIARY CARD (Right Side)
     * ------------------------------------------------------------- */
    lv_obj_t *info_box = lv_obj_create(screen);
    lv_obj_set_size(info_box, 52, 46);
    lv_obj_align(info_box, LV_ALIGN_BOTTOM_RIGHT, -2, -2);
    lv_obj_set_style_border_color(info_box, lv_color_white(), 0);
    lv_obj_set_style_border_width(info_box, 1, 0);
    lv_obj_set_style_radius(info_box, 3, 0);
    lv_obj_set_style_pad_all(info_box, 2, 0);
    lv_obj_clear_flag(info_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *info_title = lv_label_create(info_box);
    lv_label_set_text(info_title, "STATUS");
    lv_obj_align(info_title, LV_ALIGN_TOP_LEFT, 1, 0);

    lv_obj_t *info_sub = lv_label_create(info_box);
    lv_label_set_text(info_sub, "READY");
    lv_obj_align(info_sub, LV_ALIGN_CENTER, 0, 4);

    return screen;
}