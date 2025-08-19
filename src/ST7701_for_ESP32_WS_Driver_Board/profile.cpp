#include "profile.h"

// setup a simplified keyboard
//static void remap_keyboard(lv_event_t* e)
//{
//    static const char* kb_map[] = { "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
//                                    "A", "S", "D", "F", "G", "H", "J", "K", "L",  LV_SYMBOL_NEW_LINE, "\n",
//                                    "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "?", "\n",
//                                    LV_SYMBOL_CLOSE, " ",  " ", " ", LV_SYMBOL_OK, NULL
//    };
//
//    /*Set the relative width of the buttons and other controls*/
//    static const lv_buttonmatrix_ctrl_t kb_ctrl[] = { LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_6,
//                                                     LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_6,
//                                                     LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4, LV_BUTTONMATRIX_CTRL_WIDTH_4,
//                                                     LV_BUTTONMATRIX_CTRL_WIDTH_2, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_HIDDEN | LV_BUTTONMATRIX_CTRL_WIDTH_2), LV_BUTTONMATRIX_CTRL_WIDTH_6, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_HIDDEN | LV_BUTTONMATRIX_CTRL_WIDTH_2), LV_BUTTONMATRIX_CTRL_WIDTH_2
//    };
//    
//    /*Create a keyboard and add the new map as USER_1 mode*/
//    lv_keyboard_set_map(objects.kbd_profile, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
//    lv_keyboard_set_mode(objects.kbd_profile, LV_KEYBOARD_MODE_USER_1);
//}

// reset username
void profile_undo(lv_event_t* e)
{
    play_random_beep();
    lv_textarea_set_text(objects.txt_profile_username, config.user.displayName);
}

// save username
void profile_save(lv_event_t* e)
{
    const char* newName = lv_textarea_get_text(objects.txt_profile_username);
    if (strlen(newName) > 0)
    {
        play_random_beep();
        strlcpy(config.user.displayName, newName, sizeof(config.user.displayName));
        saveAndApplyBoardConfig(config);
		lv_screen_load(objects.main);
    }
}
