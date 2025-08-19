#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: Pip-Empty
lv_style_t *get_style_pip_empty_MAIN_DEFAULT();
void add_style_pip_empty(lv_obj_t *obj);
void remove_style_pip_empty(lv_obj_t *obj);

// Style: Pip-Filled
lv_style_t *get_style_pip_filled_MAIN_DEFAULT();
void add_style_pip_filled(lv_obj_t *obj);
void remove_style_pip_filled(lv_obj_t *obj);

// Style: Pip-Hidden
lv_style_t *get_style_pip_hidden_MAIN_DEFAULT();
void add_style_pip_hidden(lv_obj_t *obj);
void remove_style_pip_hidden(lv_obj_t *obj);

// Style: Button-Grey-Accent
lv_style_t *get_style_button_grey_accent_MAIN_DEFAULT();
void add_style_button_grey_accent(lv_obj_t *obj);
void remove_style_button_grey_accent(lv_obj_t *obj);

// Style: Button-White-Accent
lv_style_t *get_style_button_white_accent_MAIN_DEFAULT();
void add_style_button_white_accent(lv_obj_t *obj);
void remove_style_button_white_accent(lv_obj_t *obj);

// Style: Button-Grey-Default
lv_style_t *get_style_button_grey_default_MAIN_DEFAULT();
void add_style_button_grey_default(lv_obj_t *obj);
void remove_style_button_grey_default(lv_obj_t *obj);

// Style: Button-Grey-Selected
lv_style_t *get_style_button_grey_selected_MAIN_PRESSED();
lv_style_t *get_style_button_grey_selected_MAIN_DEFAULT();
void add_style_button_grey_selected(lv_obj_t *obj);
void remove_style_button_grey_selected(lv_obj_t *obj);

// Style: Button-Red-Default
lv_style_t *get_style_button_red_default_MAIN_PRESSED();
lv_style_t *get_style_button_red_default_MAIN_DEFAULT();
void add_style_button_red_default(lv_obj_t *obj);
void remove_style_button_red_default(lv_obj_t *obj);

// Style: Button-Red-Selected
lv_style_t *get_style_button_red_selected_MAIN_DEFAULT();
lv_style_t *get_style_button_red_selected_MAIN_PRESSED();
void add_style_button_red_selected(lv_obj_t *obj);
void remove_style_button_red_selected(lv_obj_t *obj);

// Style: Text-Button-Grey
lv_style_t *get_style_text_button_grey_MAIN_DEFAULT();
void add_style_text_button_grey(lv_obj_t *obj);
void remove_style_text_button_grey(lv_obj_t *obj);

// Style: Text-Button-Red
lv_style_t *get_style_text_button_red_MAIN_DEFAULT();
lv_style_t *get_style_text_button_red_MAIN_PRESSED();
void add_style_text_button_red(lv_obj_t *obj);
void remove_style_text_button_red(lv_obj_t *obj);

// Style: Text-Flavor-Black
lv_style_t *get_style_text_flavor_black_MAIN_DEFAULT();
void add_style_text_flavor_black(lv_obj_t *obj);
void remove_style_text_flavor_black(lv_obj_t *obj);

// Style: Button-Grey-Rounded
lv_style_t *get_style_button_grey_rounded_MAIN_DEFAULT();
lv_style_t *get_style_button_grey_rounded_MAIN_PRESSED();
void add_style_button_grey_rounded(lv_obj_t *obj);
void remove_style_button_grey_rounded(lv_obj_t *obj);

// Style: Button-Grey-Bullet
lv_style_t *get_style_button_grey_bullet_MAIN_DEFAULT();
lv_style_t *get_style_button_grey_bullet_MAIN_PRESSED();
void add_style_button_grey_bullet(lv_obj_t *obj);
void remove_style_button_grey_bullet(lv_obj_t *obj);

// Style: Slider_Blue
lv_style_t *get_style_slider_blue_MAIN_DEFAULT();
lv_style_t *get_style_slider_blue_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_blue_KNOB_DEFAULT();
lv_style_t *get_style_slider_blue_KNOB_DISABLED();
void add_style_slider_blue(lv_obj_t *obj);
void remove_style_slider_blue(lv_obj_t *obj);

// Style: Slider_Green
lv_style_t *get_style_slider_green_KNOB_DEFAULT();
lv_style_t *get_style_slider_green_KNOB_DISABLED();
lv_style_t *get_style_slider_green_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_green_MAIN_DEFAULT();
void add_style_slider_green(lv_obj_t *obj);
void remove_style_slider_green(lv_obj_t *obj);

// Style: Slider_Red
lv_style_t *get_style_slider_red_MAIN_DEFAULT();
lv_style_t *get_style_slider_red_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_red_KNOB_DEFAULT();
lv_style_t *get_style_slider_red_KNOB_DISABLED();
void add_style_slider_red(lv_obj_t *obj);
void remove_style_slider_red(lv_obj_t *obj);

// Style: Slider_White
lv_style_t *get_style_slider_white_KNOB_DEFAULT();
lv_style_t *get_style_slider_white_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_white_MAIN_DEFAULT();
void add_style_slider_white(lv_obj_t *obj);
void remove_style_slider_white(lv_obj_t *obj);

// Style: Text-Flavor-Gray-Dark
lv_style_t *get_style_text_flavor_gray_dark_MAIN_DEFAULT();
void add_style_text_flavor_gray_dark(lv_obj_t *obj);
void remove_style_text_flavor_gray_dark(lv_obj_t *obj);

// Style: Text-Flavor-Gray-Light
lv_style_t *get_style_text_flavor_gray_light_MAIN_DEFAULT();
void add_style_text_flavor_gray_light(lv_obj_t *obj);
void remove_style_text_flavor_gray_light(lv_obj_t *obj);

// Style: Text-Flavor-Orange
lv_style_t *get_style_text_flavor_orange_MAIN_DEFAULT();
void add_style_text_flavor_orange(lv_obj_t *obj);
void remove_style_text_flavor_orange(lv_obj_t *obj);

// Style: Text-Flavor-White
lv_style_t *get_style_text_flavor_white_MAIN_DEFAULT();
void add_style_text_flavor_white(lv_obj_t *obj);
void remove_style_text_flavor_white(lv_obj_t *obj);

// Style: Text-Label-White
lv_style_t *get_style_text_label_white_MAIN_DEFAULT();
void add_style_text_label_white(lv_obj_t *obj);
void remove_style_text_label_white(lv_obj_t *obj);

// Style: check-default
lv_style_t *get_style_check_default_MAIN_DEFAULT();
lv_style_t *get_style_check_default_MAIN_DISABLED();
lv_style_t *get_style_check_default_INDICATOR_DISABLED();
void add_style_check_default(lv_obj_t *obj);
void remove_style_check_default(lv_obj_t *obj);

// Style: Screen_Black
lv_style_t *get_style_screen_black_MAIN_DEFAULT();
void add_style_screen_black(lv_obj_t *obj);
void remove_style_screen_black(lv_obj_t *obj);

// Style: roller_lcars_16
lv_style_t *get_style_roller_lcars_16_MAIN_DEFAULT();
lv_style_t *get_style_roller_lcars_16_SELECTED_DEFAULT();
void add_style_roller_lcars_16(lv_obj_t *obj);
void remove_style_roller_lcars_16(lv_obj_t *obj);

// Style: container_bullet_button
lv_style_t *get_style_container_bullet_button_MAIN_DEFAULT();
lv_style_t *get_style_container_bullet_button_MAIN_DISABLED();
void add_style_container_bullet_button(lv_obj_t *obj);
void remove_style_container_bullet_button(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/