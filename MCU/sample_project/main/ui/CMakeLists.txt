SET(SOURCES components/ui_comp_label3.c
    components/ui_comp.c
    screens/ui_Screen1.c
    screens/ui_Screen2.c
    screens/ui_Screen3.c
    screens/ui_Screen4.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_xinhuo1_png.c
    images/ui_img_bat16_png.c
    images/ui_img_wenshidu32_png.c
    fonts/ui_font_chinese28.c
    fonts/ui_font_chineseLarge.c
    fonts/ui_font_chineseMicro.c
    fonts/ui_font_chineseMin.c
    fonts/ui_font_chineseNano.c
    fonts/ui_font_WSChinese16ALL.c
    fonts/ui_font_WSChinese32ALL.c)

add_library(ui ${SOURCES})
