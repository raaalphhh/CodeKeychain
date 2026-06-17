#include "morse_decoder.h"
#include "hid_dev.h"
#include <string.h>

#ifndef HID_KEY_SLASH
#define HID_KEY_SLASH 0x38
#endif

#ifndef HID_KEY_SEMICOLON
#define HID_KEY_SEMICOLON 0x33
#endif

#ifndef HID_KEY_APOSTROPHE
#define HID_KEY_APOSTROPHE 0x34
#endif

#ifndef HID_KEY_MOD_LSHIFT
#define HID_KEY_MOD_LSHIFT 0x02
#endif

#ifndef HID_KEY_MOD_LCTRL
#define HID_KEY_MOD_LCTRL 0x01
#endif

#ifndef HID_KEY_BACKSPACE
#define HID_KEY_BACKSPACE 0x2A
#endif

#ifndef HID_KEY_ENTER
#define HID_KEY_ENTER 0x28
#endif

#ifndef HID_KEY_SPACE
#define HID_KEY_SPACE 0x2C
#endif

typedef struct
{
    const char *morse;
    uint8_t key;
    uint8_t modifier;
} morse_entry_t;

static const morse_entry_t morse_table[] = {
    // Letters
    {".-", HID_KEY_A, 0},
    {"-...", HID_KEY_B, 0},
    {"-.-.", HID_KEY_C, 0},
    {"-..", HID_KEY_D, 0},
    {".", HID_KEY_E, 0},
    {"..-.", HID_KEY_F, 0},
    {"--.", HID_KEY_G, 0},
    {"....", HID_KEY_H, 0},
    {"..", HID_KEY_I, 0},
    {".---", HID_KEY_J, 0},
    {"-.-", HID_KEY_K, 0},
    {".-..", HID_KEY_L, 0},
    {"--", HID_KEY_M, 0},
    {"-.", HID_KEY_N, 0},
    {"---", HID_KEY_O, 0},
    {".--.", HID_KEY_P, 0},
    {"--.-", HID_KEY_Q, 0},
    {".-.", HID_KEY_R, 0},
    {"...", HID_KEY_S, 0},
    {"-", HID_KEY_T, 0},
    {"..-", HID_KEY_U, 0},
    {"...-", HID_KEY_V, 0},
    {".--", HID_KEY_W, 0},
    {"-..-", HID_KEY_X, 0},
    {"-.--", HID_KEY_Y, 0},
    {"--..", HID_KEY_Z, 0},

    // Numbers
    {".----", HID_KEY_1, 0},
    {"..---", HID_KEY_2, 0},
    {"...--", HID_KEY_3, 0},
    {"....-", HID_KEY_4, 0},
    {".....", HID_KEY_5, 0},
    {"-....", HID_KEY_6, 0},
    {"--...", HID_KEY_7, 0},
    {"---..", HID_KEY_8, 0},
    {"----.", HID_KEY_9, 0},
    {"-----", HID_KEY_0, 0},

    // Common ITU punctuation
    {".-.-.-", HID_KEY_DOT, 0},                         // .
    {"--..--", HID_KEY_COMMA, 0},                       // ,
    {"..--..", HID_KEY_SLASH, HID_KEY_MOD_LSHIFT},      // ?
    {"-.-.--", HID_KEY_1, HID_KEY_MOD_LSHIFT},          // !
    {".----.", HID_KEY_APOSTROPHE, 0},                  // '
    {"-..-.", HID_KEY_SLASH, 0},                        // /
    {"-.--.", HID_KEY_9, HID_KEY_MOD_LSHIFT},           // (
    {"-.--.-", HID_KEY_0, HID_KEY_MOD_LSHIFT},          // )
    {".-...", HID_KEY_7, HID_KEY_MOD_LSHIFT},           // &
    {"---...", HID_KEY_SEMICOLON, HID_KEY_MOD_LSHIFT},  // :
    {"-.-.-.", HID_KEY_SEMICOLON, 0},                   // ;
    {"-...-", HID_KEY_EQUAL, 0},                        // =
    {".-.-.", HID_KEY_EQUAL, HID_KEY_MOD_LSHIFT},       // +
    {"-....-", HID_KEY_MINUS, 0},                       // -
    {"..--.-", HID_KEY_MINUS, HID_KEY_MOD_LSHIFT},      // _
    {".-..-.", HID_KEY_APOSTROPHE, HID_KEY_MOD_LSHIFT}, // "
    {"...-..-", HID_KEY_4, HID_KEY_MOD_LSHIFT},         // $
    {".--.-.", HID_KEY_2, HID_KEY_MOD_LSHIFT},          // @

    {"........", HID_KEY_BACKSPACE, 0},
    {"---------", HID_KEY_BACKSPACE, HID_KEY_MOD_LCTRL},
    {".-.-", HID_KEY_ENTER, 0}, // Enter / new line
    {"..--", HID_KEY_SPACE, 0}, // Space
};

morse_result_t morse_decode(const char *morse)
{
    morse_result_t result = {
        .key = 0,
        .modifier = 0,
        .found = 0};

    for (int i = 0; i < sizeof(morse_table) / sizeof(morse_table[0]); i++)
    {
        if (strcmp(morse, morse_table[i].morse) == 0)
        {
            result.key = morse_table[i].key;
            result.modifier = morse_table[i].modifier;
            result.found = 1;
            break;
        }
    }

    return result;
}