#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: Pip-Empty
//

void init_style_pip_empty_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_width(style, 7);
    lv_style_set_outline_color(style, lv_color_hex(0xff000000));
    lv_style_set_outline_width(style, 6);
    lv_style_set_outline_pad(style, 2);
};

lv_style_t *get_style_pip_empty_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pip_empty_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pip_empty(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pip_empty_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pip_empty(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pip_empty_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Pip-Filled
//

void init_style_pip_filled_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_outline_width(style, 6);
    lv_style_set_outline_color(style, lv_color_hex(0xff000000));
    lv_style_set_outline_pad(style, 2);
};

lv_style_t *get_style_pip_filled_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pip_filled_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pip_filled(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pip_filled_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pip_filled(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pip_filled_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Pip-Hidden
//

void init_style_pip_hidden_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_outline_width(style, 6);
    lv_style_set_outline_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_pip_hidden_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pip_hidden_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pip_hidden(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pip_hidden_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pip_hidden(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pip_hidden_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Grey-Accent
//

void init_style_button_grey_accent_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff52586e));
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_button_grey_accent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_accent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_grey_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_grey_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_grey_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_grey_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-White-Accent
//

void init_style_button_white_accent_MAIN_DEFAULT(lv_style_t *style) {
    init_style_button_grey_accent_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_button_white_accent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_white_accent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_white_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_white_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_white_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_white_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Grey-Default
//

void init_style_button_grey_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff9ea4ba));
    lv_style_set_radius(style, 0);
    lv_style_set_shadow_width(style, 0);
};

lv_style_t *get_style_button_grey_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_grey_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_grey_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_grey_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_grey_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Grey-Selected
//

void init_style_button_grey_selected_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff6c748c));
};

lv_style_t *get_style_button_grey_selected_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_selected_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_grey_selected_MAIN_DEFAULT(lv_style_t *style) {
    init_style_button_grey_default_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_shadow_width(style, 0);
};

lv_style_t *get_style_button_grey_selected_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_selected_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_grey_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_grey_selected_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_grey_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_grey_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_grey_selected_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_grey_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Red-Default
//

void init_style_button_red_default_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffe74429));
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_width(style, 3);
    lv_style_set_radius(style, 0);
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_button_red_default_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_red_default_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_red_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_shadow_width(style, 0);
    lv_style_set_border_color(style, lv_color_hex(0xffe74429));
    lv_style_set_border_width(style, 3);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
    lv_style_set_pad_left(style, 13);
    lv_style_set_pad_right(style, 13);
};

lv_style_t *get_style_button_red_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_red_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_red_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_red_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_red_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_red_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_red_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_red_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Red-Selected
//

void init_style_button_red_selected_MAIN_DEFAULT(lv_style_t *style) {
    init_style_button_red_default_MAIN_DEFAULT(style);
    
    lv_style_set_bg_color(style, lv_color_hex(0xffe74429));
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_button_red_selected_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_red_selected_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_red_selected_MAIN_PRESSED(lv_style_t *style) {
    init_style_button_red_default_MAIN_PRESSED(style);
    
};

lv_style_t *get_style_button_red_selected_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_red_selected_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_red_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_red_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_red_selected_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_red_selected(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_red_selected_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_red_selected_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Text-Button-Grey
//

void init_style_text_button_grey_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_font(style, &ui_font_lcars_20);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_right(style, 6);
};

lv_style_t *get_style_text_button_grey_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_button_grey_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_button_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_button_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_button_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_button_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Button-Red
//

void init_style_text_button_red_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_CENTER);
    lv_style_set_text_color(style, lv_color_hex(0xffe74429));
    lv_style_set_text_font(style, &ui_font_lcars_20);
    lv_style_set_pad_right(style, 6);
    lv_style_set_pad_left(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_top(style, 2);
};

lv_style_t *get_style_text_button_red_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_button_red_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_text_button_red_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_text_button_red_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_button_red_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_text_button_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_button_red_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_text_button_red_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_text_button_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_button_red_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_text_button_red_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Text-Flavor-Black
//

void init_style_text_flavor_black_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_font(style, &ui_font_lcars_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_text_flavor_black_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_flavor_black_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_flavor_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_flavor_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_flavor_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_flavor_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Button-Grey-Rounded
//

void init_style_button_grey_rounded_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff9ea4ba));
    lv_style_set_radius(style, 50);
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_font(style, &ui_font_lcars_20);
    lv_style_set_outline_width(style, 1);
    lv_style_set_outline_color(style, lv_color_hex(0xff000000));
    lv_style_set_margin_top(style, 4);
    lv_style_set_margin_bottom(style, 4);
    lv_style_set_margin_left(style, 4);
    lv_style_set_margin_right(style, 4);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
};

lv_style_t *get_style_button_grey_rounded_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_rounded_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_grey_rounded_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_button_grey_rounded_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_rounded_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_grey_rounded(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_grey_rounded_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_grey_rounded_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_grey_rounded(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_grey_rounded_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_grey_rounded_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Button-Grey-Bullet
//

void init_style_button_grey_bullet_MAIN_DEFAULT(lv_style_t *style) {
    init_style_button_grey_rounded_MAIN_DEFAULT(style);
    
    lv_style_set_shadow_width(style, 0);
    lv_style_set_outline_width(style, 0);
};

lv_style_t *get_style_button_grey_bullet_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_bullet_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_grey_bullet_MAIN_PRESSED(lv_style_t *style) {
    init_style_button_grey_rounded_MAIN_PRESSED(style);
    
};

lv_style_t *get_style_button_grey_bullet_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_grey_bullet_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_grey_bullet(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_grey_bullet_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_grey_bullet_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_grey_bullet(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_grey_bullet_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_grey_bullet_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: Slider_Blue
//

void init_style_slider_blue_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff2196f3));
};

lv_style_t *get_style_slider_blue_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_blue_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slider_blue_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff2196f3));
};

lv_style_t *get_style_slider_blue_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_blue_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_blue_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff2196f3));
};

lv_style_t *get_style_slider_blue_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_blue_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_blue_KNOB_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff114f81));
};

lv_style_t *get_style_slider_blue_KNOB_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_blue_KNOB_DISABLED(style);
    }
    return style;
};

void add_style_slider_blue(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_blue_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_blue_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_blue_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_blue_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
};

void remove_style_slider_blue(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_blue_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_blue_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_blue_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_blue_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
};

//
// Style: Slider_Green
//

void init_style_slider_green_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff3d));
};

lv_style_t *get_style_slider_green_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_green_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_green_KNOB_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff005500));
};

lv_style_t *get_style_slider_green_KNOB_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_green_KNOB_DISABLED(style);
    }
    return style;
};

void init_style_slider_green_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff3d));
};

lv_style_t *get_style_slider_green_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_green_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_green_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff00ff00));
};

lv_style_t *get_style_slider_green_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_green_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slider_green(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_green_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_green_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_slider_green_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_green_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slider_green(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_green_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_green_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_slider_green_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_green_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Slider_Red
//

void init_style_slider_red_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffff0000));
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_slider_red_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_red_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slider_red_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffff0000));
};

lv_style_t *get_style_slider_red_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_red_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_red_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffff0000));
};

lv_style_t *get_style_slider_red_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_red_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_red_KNOB_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff550000));
};

lv_style_t *get_style_slider_red_KNOB_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_red_KNOB_DISABLED(style);
    }
    return style;
};

void add_style_slider_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_red_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_red_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_red_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_red_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
};

void remove_style_slider_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_red_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_red_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_red_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_red_KNOB_DISABLED(), LV_PART_KNOB | LV_STATE_DISABLED);
};

//
// Style: Slider_White
//

void init_style_slider_white_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_radius(style, 20);
    lv_style_set_opa(style, 0);
};

lv_style_t *get_style_slider_white_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_white_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_slider_white_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_slider_white_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_white_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_white_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_slider_white_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_white_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slider_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_white_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_white_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slider_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_white_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_white_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Flavor-Gray-Dark
//

void init_style_text_flavor_gray_dark_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff2e312e));
    lv_style_set_text_font(style, &ui_font_lcars_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_text_flavor_gray_dark_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_flavor_gray_dark_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_flavor_gray_dark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_flavor_gray_dark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_flavor_gray_dark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_flavor_gray_dark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Flavor-Gray-Light
//

void init_style_text_flavor_gray_light_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff959189));
    lv_style_set_text_font(style, &ui_font_lcars_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_text_flavor_gray_light_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_flavor_gray_light_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_flavor_gray_light(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_flavor_gray_light_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_flavor_gray_light(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_flavor_gray_light_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Flavor-Orange
//

void init_style_text_flavor_orange_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffff8901));
    lv_style_set_text_font(style, &ui_font_lcars_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_text_flavor_orange_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_flavor_orange_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_flavor_orange(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_flavor_orange_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_flavor_orange(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_flavor_orange_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Flavor-White
//

void init_style_text_flavor_white_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_font(style, &ui_font_lcars_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
};

lv_style_t *get_style_text_flavor_white_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_flavor_white_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_flavor_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_flavor_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_flavor_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_flavor_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Text-Label-White
//

void init_style_text_label_white_MAIN_DEFAULT(lv_style_t *style) {
    init_style_text_flavor_white_MAIN_DEFAULT(style);
    
    lv_style_set_text_font(style, &ui_font_lcars_20);
};

lv_style_t *get_style_text_label_white_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_label_white_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_text_label_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_label_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_text_label_white(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_label_white_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: check-default
//

void init_style_check_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_font(style, &ui_font_lcars_16);
};

lv_style_t *get_style_check_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_check_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_check_default_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffa0a4b8));
};

lv_style_t *get_style_check_default_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_check_default_MAIN_DISABLED(style);
    }
    return style;
};

void init_style_check_default_INDICATOR_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff6f748a));
};

lv_style_t *get_style_check_default_INDICATOR_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_check_default_INDICATOR_DISABLED(style);
    }
    return style;
};

void add_style_check_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_check_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_check_default_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_check_default_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
};

void remove_style_check_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_check_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_check_default_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_check_default_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
};

//
// Style: Screen_Black
//

void init_style_screen_black_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_screen_black_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_black_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_black(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_black_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: roller_lcars_16
//

void init_style_roller_lcars_16_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_lcars_16);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_radius(style, 10);
    lv_style_set_text_color(style, lv_color_hex(0xff9ea4ba));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_color(style, lv_color_hex(0xffe0e0e0));
};

lv_style_t *get_style_roller_lcars_16_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_lcars_16_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_roller_lcars_16_SELECTED_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 20);
};

lv_style_t *get_style_roller_lcars_16_SELECTED_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_lcars_16_SELECTED_DEFAULT(style);
    }
    return style;
};

void add_style_roller_lcars_16(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_roller_lcars_16_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_roller_lcars_16_SELECTED_DEFAULT(), LV_PART_SELECTED | LV_STATE_DEFAULT);
};

void remove_style_roller_lcars_16(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_roller_lcars_16_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_roller_lcars_16_SELECTED_DEFAULT(), LV_PART_SELECTED | LV_STATE_DEFAULT);
};

//
// Style: container_bullet_button
//

void init_style_container_bullet_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff9ea4ba));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_container_bullet_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_container_bullet_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_container_bullet_button_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffbfc1cb));
};

lv_style_t *get_style_container_bullet_button_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_container_bullet_button_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_container_bullet_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_container_bullet_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_container_bullet_button_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_container_bullet_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_container_bullet_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_container_bullet_button_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_pip_empty,
        add_style_pip_filled,
        add_style_pip_hidden,
        add_style_button_grey_accent,
        add_style_button_white_accent,
        add_style_button_grey_default,
        add_style_button_grey_selected,
        add_style_button_red_default,
        add_style_button_red_selected,
        add_style_text_button_grey,
        add_style_text_button_red,
        add_style_text_flavor_black,
        add_style_button_grey_rounded,
        add_style_button_grey_bullet,
        add_style_slider_blue,
        add_style_slider_green,
        add_style_slider_red,
        add_style_slider_white,
        add_style_text_flavor_gray_dark,
        add_style_text_flavor_gray_light,
        add_style_text_flavor_orange,
        add_style_text_flavor_white,
        add_style_text_label_white,
        add_style_check_default,
        add_style_screen_black,
        add_style_roller_lcars_16,
        add_style_container_bullet_button,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_pip_empty,
        remove_style_pip_filled,
        remove_style_pip_hidden,
        remove_style_button_grey_accent,
        remove_style_button_white_accent,
        remove_style_button_grey_default,
        remove_style_button_grey_selected,
        remove_style_button_red_default,
        remove_style_button_red_selected,
        remove_style_text_button_grey,
        remove_style_text_button_red,
        remove_style_text_flavor_black,
        remove_style_button_grey_rounded,
        remove_style_button_grey_bullet,
        remove_style_slider_blue,
        remove_style_slider_green,
        remove_style_slider_red,
        remove_style_slider_white,
        remove_style_text_flavor_gray_dark,
        remove_style_text_flavor_gray_light,
        remove_style_text_flavor_orange,
        remove_style_text_flavor_white,
        remove_style_text_label_white,
        remove_style_check_default,
        remove_style_screen_black,
        remove_style_roller_lcars_16,
        remove_style_container_bullet_button,
    };
    remove_style_funcs[styleIndex](obj);
}

