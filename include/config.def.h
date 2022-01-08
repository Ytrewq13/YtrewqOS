#ifndef CONFIG_DEF_H
#define CONFIG_DEF_H 1

// Console fg/bg (BGR) colors
#define FG_COLOR 0xe0e0e0  // Foreground (light grey)
#define BG_COLOR 0x202020  // Background (dark grey)

// The BGR color values used for each type of test result
#define CONFIG_COLOR_TEST_PASS 0x00ff00  // Test pass (green)
#define CONFIG_COLOR_TEST_WARN 0x00e8e8  // Warning (yellow)
#define CONFIG_COLOR_TEST_FAIL 0x0000ff  // Failed test (red)
#define CONFIG_COLOR_TEST_SKIP 0xffaa99  // Skipped tests - aborted batch (blue)

// The minimum number of rows below the console before we start scrolling (the
// number of rows at the bottom of the screen which are not usable in normal
// scrolling mode)
#define CONFIG_CONSOLE_MIN_ROWS_BELOW 1

#define TEST_ON_BOOT 1
#define TEST_VERBOSE 2

#endif /* config_def_h */
