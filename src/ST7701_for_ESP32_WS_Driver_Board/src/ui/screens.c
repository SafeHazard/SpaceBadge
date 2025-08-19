#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

extern objects_t objects;

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            // pip_main_bottom
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_main_bottom = obj;
            lv_obj_set_pos(obj, 106, 75);
            lv_obj_set_size(obj, 14, 14);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_pip_hidden(obj);
        }
        {
            // pip_main_bottom_mid
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_main_bottom_mid = obj;
            lv_obj_set_pos(obj, 106, 51);
            lv_obj_set_size(obj, 14, 14);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_pip_hidden(obj);
            lv_obj_set_style_outline_width(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_pad(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // pip_main_top_mid
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_main_top_mid = obj;
            lv_obj_set_pos(obj, 106, 27);
            lv_obj_set_size(obj, 14, 14);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_pip_filled(obj);
        }
        {
            // pip_main_top
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_main_top = obj;
            lv_obj_set_pos(obj, 106, 3);
            lv_obj_set_size(obj, 14, 14);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_pip_hidden(obj);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_main_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_main_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_main_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_main_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_main_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_main_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_main_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_main_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 38);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_main_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_main_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            // lbl_main_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_main_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            // flavor_main1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main1 = obj;
            lv_obj_set_pos(obj, 111, 133);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main2 = obj;
            lv_obj_set_pos(obj, 111, 151);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main3
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main3 = obj;
            lv_obj_set_pos(obj, 111, 169);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main4
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main4 = obj;
            lv_obj_set_pos(obj, 111, 187);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main5
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main5 = obj;
            lv_obj_set_pos(obj, 111, 204);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main6
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main6 = obj;
            lv_obj_set_pos(obj, 111, 222);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            // flavor_main7
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.flavor_main7 = obj;
            lv_obj_set_pos(obj, 111, 240);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_black(obj);
            lv_label_set_text(obj, "00 00 04 08 15 16 23 42");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 106, 272);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___subdivider);
        }
        {
            // img_main_avatar
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_main_avatar = obj;
            lv_obj_set_pos(obj, 138, 3);
            lv_obj_set_size(obj, 82, 82);
            lv_image_set_src(obj, &img_16x16);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_STRETCH);
        }
        {
            // btn_main_avatar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.btn_main_avatar = obj;
            lv_obj_set_pos(obj, 120, 0);
            lv_obj_set_size(obj, 120, 89);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // img_main_badge
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_main_badge = obj;
            lv_obj_set_pos(obj, 145, 292);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___badge_enabled);
        }
        {
            // img_main_battery
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_main_battery = obj;
            lv_obj_set_pos(obj, 106, 292);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        }
        {
            // img_main_wifi
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_main_wifi = obj;
            lv_obj_set_pos(obj, 180, 292);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___wi_fi_enabled);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_avatar() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.avatar = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    add_style_screen_black(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_avatar_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_avatar_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_avatar_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_avatar_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_avatar_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_avatar_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            // roller_avatar_component
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.roller_avatar_component = obj;
            lv_obj_set_pos(obj, 102, 122);
            lv_obj_set_size(obj, 134, 160);
            lv_roller_set_options(obj, "", LV_ROLLER_MODE_INFINITE);
            add_style_roller_lcars_16(obj);
        }
        {
            // lbl_avatar_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_avatar_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            // img_avatar_avatar
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_avatar_avatar = obj;
            lv_obj_set_pos(obj, 128, 5);
            lv_obj_set_size(obj, 82, 82);
            lv_image_set_src(obj, &img_16x16);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_STRETCH);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 102, 287);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___subdivider);
            lv_image_set_scale(obj, 265);
        }
        {
            // lbl_avatar_unlocks
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_avatar_unlocks = obj;
            lv_obj_set_pos(obj, 102, 297);
            lv_obj_set_size(obj, 94, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0/255 Unlocked");
        }
    }
    
    tick_screen_avatar();
}

void tick_screen_avatar() {
}

void create_screen_profile() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.profile = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_profile_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_profile_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    add_style_button_red_selected(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj3 = obj;
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_profile_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_profile_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 216, 44);
            lv_obj_set_size(obj, 24, 37);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_container_bullet_button(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_profile_save
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_profile_save = obj;
            lv_obj_set_pos(obj, 181, 44);
            lv_obj_set_size(obj, 58, 37);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SAVE");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj5 = obj;
            lv_obj_set_pos(obj, 100, 44);
            lv_obj_set_size(obj, 24, 37);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_profile_undo
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_profile_undo = obj;
            lv_obj_set_pos(obj, 100, 44);
            lv_obj_set_size(obj, 58, 37);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 1, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "UNDO");
                }
            }
        }
        {
            // txt_profile_username
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.txt_profile_username = obj;
            lv_obj_set_pos(obj, 100, 0);
            lv_obj_set_size(obj, 140, 39);
            lv_textarea_set_max_length(obj, 32);
            lv_textarea_set_placeholder_text(obj, "Username");
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.obj6 = obj;
            lv_obj_set_pos(obj, 0, 104);
            lv_obj_set_size(obj, 240, 216);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffa0a4b8), LV_PART_ITEMS | LV_STATE_DEFAULT);
        }
    }
    lv_keyboard_set_textarea(objects.obj6, objects.txt_profile_username);
    
    tick_screen_profile();
}

void tick_screen_profile() {
}

void create_screen_mission() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.mission = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_mission_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_selected(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_mission_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_mission_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_mission_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_mission_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj7 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_mission_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            // lbl_mission_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj8 = obj;
            lv_obj_set_pos(obj, 102, 0);
            lv_obj_set_size(obj, 24, 37);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbfc1cb), LV_PART_MAIN | LV_STATE_DISABLED);
        }
        {
            // btn_mission_host
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_mission_host = obj;
            lv_obj_set_pos(obj, 102, 0);
            lv_obj_set_size(obj, 82, 37);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -2, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Host/Solo");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj9 = obj;
            lv_obj_set_pos(obj, 102, 44);
            lv_obj_set_size(obj, 24, 37);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbfc1cb), LV_PART_MAIN | LV_STATE_DISABLED);
        }
        {
            // btn_mission_join
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_mission_join = obj;
            lv_obj_set_pos(obj, 103, 44);
            lv_obj_set_size(obj, 81, 37);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -21, 1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Join");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 102, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_line_space(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1\n2\n3\n4\n5\n6\nAll");
        }
        {
            // lbl_mission_xp1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp1 = obj;
            lv_obj_set_pos(obj, 126, 122);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xp2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp2 = obj;
            lv_obj_set_pos(obj, 126, 150);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xp3
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp3 = obj;
            lv_obj_set_pos(obj, 126, 178);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xp4
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp4 = obj;
            lv_obj_set_pos(obj, 126, 206);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xp5
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp5 = obj;
            lv_obj_set_pos(obj, 126, 233);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xp6
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xp6 = obj;
            lv_obj_set_pos(obj, 126, 261);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/28800");
        }
        {
            // lbl_mission_xpall
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_mission_xpall = obj;
            lv_obj_set_pos(obj, 126, 289);
            lv_obj_set_size(obj, 110, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_label_set_text(obj, "/172200");
        }
    }
    
    tick_screen_mission();
}

void tick_screen_mission() {
}

void create_screen_host() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.host = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_host_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_selected(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_host_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_host_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_host_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_host_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj10 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_host_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_host_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            // lbl_host_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_host_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            // ddl_host_games
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.ddl_host_games = obj;
            lv_obj_set_pos(obj, 105, 283);
            lv_obj_set_size(obj, 128, 32);
            lv_dropdown_set_options(obj, "Select\nAssimilation\nLineCon\nPacket Filter\nWiring\nDefender\nSpeeder");
            lv_dropdown_set_dir(obj, LV_DIR_TOP);
            lv_dropdown_set_symbol(obj, LV_SYMBOL_UP);
            lv_dropdown_set_selected(obj, 0);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 98, 5);
            lv_obj_set_size(obj, 142, 26);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Host Away Mission");
        }
        {
            // cnt_host_start
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_host_start = obj;
            lv_obj_set_pos(obj, 210, 31);
            lv_obj_set_size(obj, 30, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_container_bullet_button(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_host_start
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_host_start = obj;
            lv_obj_set_pos(obj, 178, 31);
            lv_obj_set_size(obj, 62, 50);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Engage");
                }
            }
        }
        {
            // list_host_players
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.list_host_players = obj;
            lv_obj_set_pos(obj, 106, 144);
            lv_obj_set_size(obj, 127, 138);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // cnt_host_kick
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_host_kick = obj;
            lv_obj_set_pos(obj, 105, 31);
            lv_obj_set_size(obj, 24, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbfc1cb), LV_PART_MAIN | LV_STATE_DISABLED);
        }
        {
            // btn_host_kick
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_host_kick = obj;
            lv_obj_set_pos(obj, 105, 31);
            lv_obj_set_size(obj, 62, 50);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Kick");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 106, 122);
            lv_obj_set_size(obj, 127, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Current Crew:");
        }
        {
            // spin_host_placebo
            lv_obj_t *obj = lv_spinner_create(parent_obj);
            objects.spin_host_placebo = obj;
            lv_obj_set_pos(obj, 104, 120);
            lv_obj_set_size(obj, 21, 21);
            lv_spinner_set_anim_params(obj, 1000, 60);
            lv_obj_set_style_arc_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_host();
}

void tick_screen_host() {
}

void create_screen_join() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.join = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_mission_mission_2
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_mission_mission_2 = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_selected(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_join_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_join_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_join_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_join_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_join_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_join_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DISABLED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_join_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_join_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj11 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_join_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_join_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            // lbl_join_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_join_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            // cnt_join_ready
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_join_ready = obj;
            lv_obj_set_pos(obj, 210, 31);
            lv_obj_set_size(obj, 30, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbfc1cb), LV_PART_MAIN | LV_STATE_DISABLED);
        }
        {
            // btn_join_ready
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_join_ready = obj;
            lv_obj_set_pos(obj, 170, 31);
            lv_obj_set_size(obj, 70, 50);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 5, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Ready");
                }
            }
        }
        {
            // ddl_join_games
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.ddl_join_games = obj;
            lv_obj_set_pos(obj, 104, 284);
            lv_obj_set_size(obj, 128, 32);
            lv_dropdown_set_options(obj, "Select\nAssimilation\nLineCon\nPacket Filter\nWiring\nDefender\nSpeeder");
            lv_dropdown_set_dir(obj, LV_DIR_TOP);
            lv_dropdown_set_symbol(obj, LV_SYMBOL_UP);
            lv_dropdown_set_selected(obj, 0);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 98, 5);
            lv_obj_set_size(obj, 142, 26);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Join Away Mission");
        }
        {
            // roller_join_games
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.roller_join_games = obj;
            lv_obj_set_pos(obj, 105, 141);
            lv_obj_set_size(obj, 127, 141);
            lv_roller_set_options(obj, "No Crews Found", LV_ROLLER_MODE_NORMAL);
            lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 106, 122);
            lv_obj_set_size(obj, 127, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Select a Crew:");
        }
        {
            // img_join_readystate
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_join_readystate = obj;
            lv_obj_set_pos(obj, 120, 40);
            lv_obj_set_size(obj, 32, 32);
            lv_image_set_src(obj, &img_ui___not_ready);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_STRETCH);
        }
        {
            // spin_join_placebo
            lv_obj_t *obj = lv_spinner_create(parent_obj);
            objects.spin_join_placebo = obj;
            lv_obj_set_pos(obj, 104, 120);
            lv_obj_set_size(obj, 21, 21);
            lv_spinner_set_anim_params(obj, 1000, 60);
            lv_obj_set_style_arc_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_join();
}

void tick_screen_join() {
}

void create_screen_contacts() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.contacts = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    add_style_screen_black(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_contacts_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_contacts_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_selected(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_contacts_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_contacts_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_contacts_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj12 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_contacts_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_contacts_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            // lbl_contacts_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_contacts_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            // img_contacts_avatar
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_contacts_avatar = obj;
            lv_obj_set_pos(obj, 104, 3);
            lv_obj_set_size(obj, 32, 32);
            lv_image_set_src(obj, &img_16x16);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_STRETCH);
        }
        {
            // lbl_contacts_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_contacts_name = obj;
            lv_obj_set_pos(obj, 137, 10);
            lv_obj_set_size(obj, 103, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Select A Contact");
        }
        {
            // check_contacts_crew
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.check_contacts_crew = obj;
            lv_obj_set_pos(obj, 104, 42);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Crew");
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_check_default(obj);
        }
        {
            // check_contacts_block
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.check_contacts_block = obj;
            lv_obj_set_pos(obj, 173, 42);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Block");
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_check_default(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_contacts_xp
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_contacts_xp = obj;
            lv_obj_set_pos(obj, 105, 67);
            lv_obj_set_size(obj, 68, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0000");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 179, 67);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Total XP");
        }
        {
            // tabview_contacts
            lv_obj_t *obj = lv_tabview_create(parent_obj);
            objects.tabview_contacts = obj;
            lv_obj_set_pos(obj, 101, 122);
            lv_obj_set_size(obj, 137, 198);
            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
            lv_tabview_set_tab_bar_size(obj, 24);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
            lv_obj_set_style_radius(obj, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // tab_contacts_scan
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Scan");
                    objects.tab_contacts_scan = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // roller_contacts_scan
                            lv_obj_t *obj = lv_roller_create(parent_obj);
                            objects.roller_contacts_scan = obj;
                            lv_obj_set_pos(obj, -13, -13);
                            lv_obj_set_size(obj, 137, 174);
                            lv_roller_set_options(obj, "", LV_ROLLER_MODE_INFINITE);
                            add_style_roller_lcars_16(obj);
                        }
                    }
                }
                {
                    // tab_contacts_crew
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Crew");
                    objects.tab_contacts_crew = obj;
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // roller_contacts_crew
                            lv_obj_t *obj = lv_roller_create(parent_obj);
                            objects.roller_contacts_crew = obj;
                            lv_obj_set_pos(obj, -13, -13);
                            lv_obj_set_size(obj, 137, 174);
                            lv_roller_set_options(obj, "", LV_ROLLER_MODE_INFINITE);
                            add_style_roller_lcars_16(obj);
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_contacts();
}

void tick_screen_contacts() {
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_settings_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_settings_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_settings_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_avatar_settings_1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_avatar_settings_1 = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    add_style_button_grey_selected(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj13 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_settings_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_settings_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            // lbl_settings_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_settings_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 98, 127);
            lv_obj_set_size(obj, 142, 193);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // check_settings_airplanemode
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.check_settings_airplanemode = obj;
                    lv_obj_set_pos(obj, 6, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "Cloak Enable");
                    add_style_check_default(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 41, 30);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_text_flavor_white(obj);
                    lv_label_set_text(obj, "(Airplane Mode)");
                }
                {
                    // check_settings_badgemode
                    lv_obj_t *obj = lv_checkbox_create(parent_obj);
                    objects.check_settings_badgemode = obj;
                    lv_obj_set_pos(obj, 6, 61);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_checkbox_set_text(obj, "Badge Mode");
                    add_style_check_default(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 12, 91);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_text_flavor_white(obj);
                    lv_label_set_text(obj, "Show badge after (sec):");
                }
                {
                    // btn_settings_delay_down
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_delay_down = obj;
                    lv_obj_set_pos(obj, 6, 112);
                    lv_obj_set_size(obj, 43, 37);
                    add_style_button_grey_rounded(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "-");
                        }
                    }
                }
                {
                    // lbl_settings_badgedelay
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_settings_badgedelay = obj;
                    lv_obj_set_pos(obj, 49, 118);
                    lv_obj_set_size(obj, 44, LV_SIZE_CONTENT);
                    add_style_text_label_white(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "60");
                }
                {
                    // btn_settings_delay_up
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_delay_up = obj;
                    lv_obj_set_pos(obj, 93, 112);
                    lv_obj_set_size(obj, 43, 37);
                    add_style_button_grey_rounded(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "+");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 8, 163);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_text_label_white(obj);
                    lv_label_set_text(obj, "Show Usernames:");
                }
                {
                    // btn_settings_usernames
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_usernames = obj;
                    lv_obj_set_pos(obj, 9, 188);
                    lv_obj_set_size(obj, 126, 37);
                    add_style_button_grey_rounded(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_settings_usernames
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_settings_usernames = obj;
                            lv_obj_set_pos(obj, -1, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "None");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 6, 237);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_text_label_white(obj);
                    lv_label_set_text(obj, "Show Game Hosts:");
                }
                {
                    // btn_settings_gamehosts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_settings_gamehosts = obj;
                    lv_obj_set_pos(obj, 7, 261);
                    lv_obj_set_size(obj, 126, 37);
                    add_style_button_grey_rounded(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lbl_settings_gamehosts
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_settings_gamehosts = obj;
                            lv_obj_set_pos(obj, 0, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "None");
                        }
                    }
                }
            }
        }
        {
            // sld_settings_volume
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.sld_settings_volume = obj;
            lv_obj_set_pos(obj, 105, 49);
            lv_obj_set_size(obj, 129, 32);
            lv_slider_set_value(obj, 50, LV_ANIM_OFF);
            add_style_slider_blue(obj);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 105, 19);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Volume");
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
}

void create_screen_info() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.info = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 0, 63);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ui_element___main_divider);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 144);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_info_mission
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_mission = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 6, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Mission");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 188);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_info_contacts
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_contacts = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 2, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Contacts");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 275);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_white_accent(obj);
                }
                {
                    // btn_info_info
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_info = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_selected(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Info");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 231);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_info_settings
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_settings = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff6c748c), LV_PART_MAIN | LV_STATE_PRESSED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff9ea4ba), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 5, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Settings");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, -7);
            lv_obj_set_size(obj, 98, 44);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // btn_info_profile
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_profile = obj;
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 98, 37);
                    add_style_button_red_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 20, 1);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_red(obj);
                            lv_label_set_text(obj, "Profile");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj14 = obj;
            lv_obj_set_pos(obj, 0, 37);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 7);
                    lv_obj_set_size(obj, 13, 37);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_button_grey_accent(obj);
                }
                {
                    // btn_info_main
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_info_main = obj;
                    lv_obj_set_pos(obj, 21, 7);
                    lv_obj_set_size(obj, 77, 37);
                    add_style_button_grey_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 14, -3);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_text_button_grey(obj);
                            lv_label_set_text(obj, "Main");
                        }
                    }
                }
            }
        }
        {
            // lbl_info_random
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_info_random = obj;
            lv_obj_set_pos(obj, 31, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_button_grey(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "random #s");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 104, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Network Nodes:");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 104, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Free RAM: ");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 103, 46);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Free PSRAM: ");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 104, 69);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Uptime:");
        }
        {
            // lbl_info_nodecount
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_info_nodecount = obj;
            lv_obj_set_pos(obj, 178, 1);
            lv_obj_set_size(obj, 62, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // lbl_info_freeram
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_info_freeram = obj;
            lv_obj_set_pos(obj, 178, 24);
            lv_obj_set_size(obj, 62, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // lbl_info_freepsram
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_info_freepsram = obj;
            lv_obj_set_pos(obj, 178, 46);
            lv_obj_set_size(obj, 62, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // lbl_info_uptime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_info_uptime = obj;
            lv_obj_set_pos(obj, 161, 69);
            lv_obj_set_size(obj, 79, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
            add_style_text_flavor_white(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0");
        }
        {
            // cnt_info_intro
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_info_intro = obj;
            lv_obj_set_pos(obj, 102, 271);
            lv_obj_set_size(obj, 36, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            add_style_container_bullet_button(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_info_intro
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_info_intro = obj;
            lv_obj_set_pos(obj, 102, 271);
            lv_obj_set_size(obj, 59, 50);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Intro");
                }
            }
        }
        {
            // cnt_info_outro
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_info_outro = obj;
            lv_obj_set_pos(obj, 199, 271);
            lv_obj_set_size(obj, 36, 50);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_container_bullet_button(obj);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // btn_info_outro
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_info_outro = obj;
            lv_obj_set_pos(obj, 176, 271);
            lv_obj_set_size(obj, 59, 50);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Outro");
                }
            }
        }
        {
            // btn_info_credits
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_info_credits = obj;
            lv_obj_set_pos(obj, 102, 115);
            lv_obj_set_size(obj, 134, 50);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Thanks & Credits");
                }
            }
        }
        {
            // btn_info_documentation
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_info_documentation = obj;
            lv_obj_set_pos(obj, 103, 167);
            lv_obj_set_size(obj, 134, 50);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Documentation");
                }
            }
        }
        {
            // btn_info_disclaimers
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_info_disclaimers = obj;
            lv_obj_set_pos(obj, 102, 219);
            lv_obj_set_size(obj, 134, 50);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Disclaimers");
                }
            }
        }
        {
            // pnl_info_credits
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_info_credits = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_info_credits
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_info_credits = obj;
                    lv_obj_set_pos(obj, -11, -10);
                    lv_obj_set_size(obj, 232, 603);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_scroll_dir(obj, LV_DIR_RIGHT);
                    add_style_text_label_white(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Huge thanks to the following:\n- YOU for supporting us!\n- My family for sticking with me for this year-long project\n- Every IP owner/creator who inspired a part of this project\n- Our suppliers: Waveshare, MakerHawk, GS-JJ & Amazon\n- Michael Okuda: LCARS design\n- Jim @ theLCARS.com\n- @grandkhan221b on tumblr for sharing their amazing art\n- trekcore.com for collecting and sharing so much audio\n- Everyone who contributed to open source projects we used, including ESP32, LVGL, audio-tools, painlessMesh, Async TCP, TaskScheduler, Arduino, EEZ Studio, Invoke AI and all the talented contributors on HuggingFace\n- Special thanks to both the Visual Studio 2022 and Visual Micro development teams\n- Anyone whose posts helped me write better code\n- The range of AIs (and all the training material they learned from) that helped me - Claude and ChatGPT in particular (and the adults that paid for Pro on each for me)\n- Anyone else I forgot");
                }
            }
        }
        {
            // pnl_info_disclaimers
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.pnl_info_disclaimers = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_info_disclaimers
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_info_disclaimers = obj;
                    lv_obj_set_pos(obj, -11, -10);
                    lv_obj_set_size(obj, 232, 3874);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
                    lv_obj_set_scroll_dir(obj, LV_DIR_RIGHT);
                    add_style_text_label_white(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Disclaimers (boring one 1st):\nINTELLECTUAL PROPERTY \n•This project is an independent fan creation not affiliated with, endorsed by, or sponsored by any intellectual property holders referenced herein. All original artwork was created by the project team or generated using AI tools under my direction.\n•Any resemblance to existing intellectual properties is intended as parody, homage, or fair use commentary in the spirit of fan appreciation. Reference materials may have been used for inspiration, but all final implementations represent transformative works.\n•This product is sold based on its technical functionality and craftsmanship, not on any intellectual property references. Purchasers are advised that they are buying an electronic badge/device, and any IP-related content is incidental to the core product.\n•No trademark or copyright infringement is intended. All trademarks and copyrights remain the property of their respective owners. If any intellectual property holder objects to references made herein, please contact us for immediate resolution.\n•This disclaimer does not constitute legal advice. Users assume all risks associated with the use of this product.\n\nOther Disclaimers (Funny!):\n•Not all disclaimers apply\n•Glitches caused by Wesley Crusher\n•Excludes carbon-based life forms\n•Not effective against zombie apocalypse\n•May cause spontaneous dancing\n•Side effects include sudden urge to yodel\n•Not recommended for time travelers\n•Does not work in parallel universes\n•May attract alien attention\n•Not suited for interdimensional use\n•May cause existential crisis\n•Not responsible for temporal paradoxes\n•Does not prevent alien abduction\n•May void warranty on reality\n•Not effective during solar eclipses\n•Disclaimer may be better than product\n•This disclaimer is not responsible for other disclaimers\n•May cause uncontrollable urge to read more disclaimers\n•Not liable for disappointing your pets\n•Does not grant superpowers (unfortunately)\n•May cause neighbors to become suspiciously friendly\n•Not recommended for use by fictional characters\n•Do not eat \n•Do not use as ice cream topping \n•Do not use while sleeping \n•Do not use while awake\n•This product moves when used \n•Only use when stationary\n•Not intended for use on county roads\n•Do not attempt to stop chain with hands or genitals \n•Do not use for personal hygiene \n•Remove child before folding \n•Do not use as ladder \n•Do not eat toner \n•Warning: Cape does not enable user to fly \n•Not suitable for children aged 36 months or over \n•Do not put any person in this washer \n•Do not use in shower \n•This product is not to be used as a facial mask \n•Do not use orally after using rectally \n•May contain nuts \n•Contents may settle during shipment \n•Do not iron clothes on body \n•Not for weight control \n•Do not use near fire, flame, or sparks \n•Caution: Contents hot \n•Do not turn upside down \n•Do not use house paint on face \n•Do not drive with sunshield in place \n•Do not eat if seal is broken \n•Suitable for vegetarians \n•Warning: Has been found to cause cancer in laboratory mice \n•Do not attempt to swallow \n•Remove wrapper before eating \n•Warning: Knots in hammock are tied tight but we can't guarantee they won't come undone\n•Fragile: Do not drop penguins\n•Do not machine wash or dry clean\n•Some cars not compatible with some car seats\n•Please store in the cold section of the refrigerator\n•Caution: Shoots rubber bands\n•Warning: Do not climb inside this bag and zip it up\n•Recycled flush water unsafe for drinking\n•Do not use as ear plugs\n•Not dishwasher safe\n•Warning: May contain traces of milk, soy, meat, nuts, fruit, arbitrary food-like or food-adjacent items, tax returns, mayonnaise or coherent C++ code\n•For external use only\n•Do not use if allergic to any ingredients\n•Consult your doctor before use\n•Not intended to diagnose, treat, cure, or prevent any disease\n•Keep out of reach of children\n•If irritation occurs, discontinue use immediately\n•Do not use on broken skin\n•May cause drowsiness\n•Do not operate heavy machinery after use\n•Not for use during pregnancy or nursing\n•Individual results may vary\n•This product has not been evaluated by the FDA\n•May contain nuts, soy, or dairy\n•Consume within 3 days of opening\n•Not suitable for vegetarians/vegans\n•Contains artificial colors and flavors\n•Best served chilled\n•Do not microwave in original packaging\n•May contain traces of alcohol\n•Shake well before use\n•Do not shake\n•Store in a cool, dry place\n•Contains caffeine\n•May cause staining of teeth\n•Do not expose to water or moisture\n•Use only with approved chargers\n•Risk of electric shock\n•Contains small parts\n•Do not disassemble\n•Warranty void if seal is broken\n•May interfere with pacemakers\n•Not responsible for data loss\n•For indoor use only\n•Do not expose to extreme temperatures\n•May cause interference with radio equipment\n•Terms and conditions subject to change\n•No warranty expressed or implied\n•Swim at your own risk\n•May collect personal data\n•Requires internet connection\n•Does not connect to the internet\n•Compatible devices only\n•Beta software -use with caution\n•Maximum liability limited to purchase price\n•May contain third-party content\n•Regular backups recommended\n•Not responsible for lost data\n•Professional installation recommended\n•Not for highway use\n•Maximum weight capacity: -1 lbs\n•Check local laws before use\n•Adult supervision required\n•Helmet required\n•Not suitable for commercial use\n•Not suitable for residential use\n•Regular maintenance required\n•Speed restrictions apply\n•Weather conditions may affect performance\n•Past performance does not guarantee future results\n•Investment values may go up or down\n•You may lose money\n•Seek independent financial advice\n•Not FDIC insured\n•Subject to market risks\n•Fees and charges apply\n•Early withdrawal penalties may apply\n•Not available in all states\n•Minimum balance required\n•Credit approval required\n•Not responsible for lost or stolen items\n•Use at your own risk\n•Results not guaranteed\n•Subject to availability\n•Prices subject to change without notice\n•All sales final\n•No returns or exchanges\n•Void where prohibited\n•One per customer\n•Limited time offer\n•Not responsible for typographical errors\n•Management reserves the right to refuse service\n•Must be 18 or older\n•Designed for ages 0-17\n•Not available to residents of [your location]\n•Valid ID required\n•Parental consent required for minors\n•Not valid in Quebec\n•Excluding Hawaii and Alaska\n•International shipping not available\n•Age verification required\n•Restricted in certain jurisdictions\n•Do not dispose of in regular trash\n•Recycle according to local regulations\n•Contains hazardous materials\n•Use in well-ventilated area\n•Avoid contact with skin and eyes\n•Do not incinerate\n•Keep away from heat sources\n•Flammable -keep away from open flames\n•May be harmful if swallowed\n•Wash hands after use\n•Adult assembly required\n•Not suitable for children under 36 months\n•Choking hazard\n•Protective equipment recommended\n•Play area should be clear of obstacles\n•Maximum user weight applies\n•Inspect before each use\n•Not a life-saving device\n•Supervision required at all times\n•Professional use only\n•Batteries not included\n•Contains Lithium-ion batteries\n•Some assembly required\n•Comes fully assembled\n•Sold separately\n•Accessories not included\n•Model shown not actual size\n•Colors may vary from images shown\n•Simulated results\n•Dramatization -do not attempt\n•Closed course -professional driver\n•Your mileage may vary\n•Not all features available in all areas\n•Subject to credit approval\n•While supplies last\n•Cannot be combined with other offers\n•For entertainment use only\n•Caution: product not entertaining");
                }
            }
        }
        {
            // cnt_info_qr
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_info_qr = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // img_info_doc_qr
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.img_info_doc_qr = obj;
                    lv_obj_set_pos(obj, 0, 44);
                    lv_obj_set_size(obj, 240, 240);
                    lv_image_set_src(obj, &img_ui___docs_qr_code);
                    lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_DEFAULT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                }
            }
        }
    }
    
    tick_screen_info();
}

void tick_screen_info() {
}

void create_screen_badge() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.badge = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // img_badge_avatar
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_badge_avatar = obj;
            lv_obj_set_pos(obj, 72, 7);
            lv_obj_set_size(obj, 150, 150);
            lv_image_set_src(obj, &img_16x16);
            lv_image_set_inner_align(obj, LV_IMAGE_ALIGN_STRETCH);
            lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.obj15 = obj;
            lv_obj_set_pos(obj, 62, 288);
            lv_obj_set_size(obj, 163, 29);
            lv_scale_set_mode(obj, LV_SCALE_MODE_HORIZONTAL_TOP);
            lv_scale_set_range(obj, 0, 28800);
            lv_scale_set_total_tick_count(obj, 7);
            lv_scale_set_major_tick_every(obj, 1);
            lv_scale_set_label_show(obj, true);
            lv_obj_set_style_length(obj, 5, LV_PART_ITEMS | LV_STATE_DEFAULT);
            lv_obj_set_style_length(obj, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_12, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xff2196f3), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_line_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_badge_game1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game1 = obj;
            lv_obj_set_pos(obj, 9, 161);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 1:");
        }
        {
            // lbl_badge_game2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game2 = obj;
            lv_obj_set_pos(obj, 9, 183);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 2:");
        }
        {
            // lbl_badge_game3
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game3 = obj;
            lv_obj_set_pos(obj, 9, 205);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 3:");
        }
        {
            // lbl_badge_game4
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game4 = obj;
            lv_obj_set_pos(obj, 9, 226);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 4:");
        }
        {
            // lbl_badge_game5
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game5 = obj;
            lv_obj_set_pos(obj, 9, 248);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 5:");
        }
        {
            // lbl_badge_game6
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_badge_game6 = obj;
            lv_obj_set_pos(obj, 9, 270);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            add_style_text_label_white(obj);
            lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Game 6:");
        }
        {
            // bar_badge_game1
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game1 = obj;
            lv_obj_set_pos(obj, 62, 165);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // bar_badge_game2
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game2 = obj;
            lv_obj_set_pos(obj, 62, 187);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // bar_badge_game3
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game3 = obj;
            lv_obj_set_pos(obj, 62, 209);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // bar_badge_game4
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game4 = obj;
            lv_obj_set_pos(obj, 62, 230);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // bar_badge_game5
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game5 = obj;
            lv_obj_set_pos(obj, 62, 252);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // bar_badge_game6
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_badge_game6 = obj;
            lv_obj_set_pos(obj, 62, 274);
            lv_obj_set_size(obj, 163, 10);
            lv_bar_set_range(obj, 0, 28800);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
        {
            // pip_badge_top
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_badge_top = obj;
            lv_obj_set_pos(obj, 19, 9);
            lv_obj_set_size(obj, 28, 28);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            add_style_pip_hidden(obj);
        }
        {
            // pip_badge_top_mid
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_badge_top_mid = obj;
            lv_obj_set_pos(obj, 19, 49);
            lv_obj_set_size(obj, 28, 28);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            add_style_pip_hidden(obj);
        }
        {
            // pip_badge_bottom_mid
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_badge_bottom_mid = obj;
            lv_obj_set_pos(obj, 19, 89);
            lv_obj_set_size(obj, 28, 28);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            add_style_pip_hidden(obj);
        }
        {
            // pip_badge_bottom
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.pip_badge_bottom = obj;
            lv_obj_set_pos(obj, 19, 129);
            lv_obj_set_size(obj, 28, 28);
            lv_led_set_color(obj, lv_color_hex(0xff000000));
            lv_led_set_brightness(obj, 255);
            add_style_pip_hidden(obj);
        }
        {
            // cnt_badge_tappad
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_badge_tappad = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_badge();
}

void tick_screen_badge() {
}

void create_screen_game_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.game_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    tick_screen_game_screen();
}

void tick_screen_game_screen() {
}

void create_screen_queue() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.queue = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // img_queue_bkgrnd
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_queue_bkgrnd = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_stars);
        }
        {
            // img_queue_portrait
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_queue_portrait = obj;
            lv_obj_set_pos(obj, 102, 53);
            lv_obj_set_size(obj, 150, 150);
        }
        {
            // img_queue_explanation
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.img_queue_explanation = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 150, 150);
        }
        {
            // cnt_queue_bubble
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_queue_bubble = obj;
            lv_obj_set_pos(obj, 0, 193);
            lv_obj_set_size(obj, 240, 127);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff1952c6), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lbl_queue_speech
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_queue_speech = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 240, 125);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_lcars_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // cnt_queue_tap
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cnt_queue_tap = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 240, 320);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // btn_queue_skip
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_queue_skip = obj;
            lv_obj_set_pos(obj, 193, 0);
            lv_obj_set_size(obj, 47, 22);
            add_style_button_grey_bullet(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_lcars_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "SKIP");
                }
            }
        }
    }
    
    tick_screen_queue();
}

void tick_screen_queue() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_avatar,
    tick_screen_profile,
    tick_screen_mission,
    tick_screen_host,
    tick_screen_join,
    tick_screen_contacts,
    tick_screen_settings,
    tick_screen_info,
    tick_screen_badge,
    tick_screen_game_screen,
    tick_screen_queue,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_avatar();
    create_screen_profile();
    create_screen_mission();
    create_screen_host();
    create_screen_join();
    create_screen_contacts();
    create_screen_settings();
    create_screen_info();
    create_screen_badge();
    create_screen_game_screen();
    create_screen_queue();
}
