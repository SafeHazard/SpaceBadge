#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_ui_element___main_divider;
extern const lv_img_dsc_t img_ui_element___subdivider;
extern const lv_img_dsc_t img_16x16;
extern const lv_img_dsc_t img_stars;
extern const lv_img_dsc_t img_ui_element___badge_disabled;
extern const lv_img_dsc_t img_ui_element___badge_enabled;
extern const lv_img_dsc_t img_ui_element___battery_000;
extern const lv_img_dsc_t img_ui_element___battery_025;
extern const lv_img_dsc_t img_ui_element___battery_050;
extern const lv_img_dsc_t img_ui_element___battery_075;
extern const lv_img_dsc_t img_ui_element___battery_100;
extern const lv_img_dsc_t img_ui_element___wi_fi_disabled;
extern const lv_img_dsc_t img_ui_element___wi_fi_enabled;
extern const lv_img_dsc_t img_ui___not_ready;
extern const lv_img_dsc_t img_ui___ready;
extern const lv_img_dsc_t img_ui___docs_qr_code;
extern const lv_img_dsc_t img_ui___shuttle;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[17];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/