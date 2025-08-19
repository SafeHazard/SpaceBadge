#include "images.h"

const ext_img_desc_t images[17] = {
    { "UI Element - Main Divider", &img_ui_element___main_divider },
    { "UI Element - Subdivider", &img_ui_element___subdivider },
    { "16x16", &img_16x16 },
    { "stars", &img_stars },
    { "UI Element - Badge Disabled", &img_ui_element___badge_disabled },
    { "UI Element - Badge Enabled", &img_ui_element___badge_enabled },
    { "UI Element - Battery 000", &img_ui_element___battery_000 },
    { "UI Element - Battery 025", &img_ui_element___battery_025 },
    { "UI Element - Battery 050", &img_ui_element___battery_050 },
    { "UI Element - Battery 075", &img_ui_element___battery_075 },
    { "UI Element - Battery 100", &img_ui_element___battery_100 },
    { "UI Element - WiFi Disabled", &img_ui_element___wi_fi_disabled },
    { "UI Element - WiFi Enabled", &img_ui_element___wi_fi_enabled },
    { "UI - Not Ready", &img_ui___not_ready },
    { "UI - Ready", &img_ui___ready },
    { "UI - Docs QR Code", &img_ui___docs_qr_code },
    { "UI - Shuttle", &img_ui___shuttle },
};
