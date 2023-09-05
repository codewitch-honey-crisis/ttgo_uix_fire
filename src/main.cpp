// Adapted from https://codelibary.com/snippet/794/fire-effect-demonstration
#define SHOW_FPS
#include <Arduino.h>
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#define LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_DATA0 19 /*!< for 1-line SPI, this also refereed as MOSI */
#define PIN_NUM_PCLK 18
#define PIN_NUM_CS 5
#define PIN_NUM_DC 16
#define PIN_NUM_RST 23
#define PIN_NUM_BK_LIGHT 4
#define LCD_HRES 135
#define LCD_VRES 240
#define PIN_BUTTON_A 35
#define PIN_BUTTON_B 0

#include <button.hpp>
#include <gfx.hpp>
#include <uix.hpp>
using namespace arduino;
using namespace gfx;
using namespace uix;

#define V_WIDTH 60
#define V_HEIGHT 32
#define BUF_WIDTH 60
#define BUF_HEIGHT 38
#define PALETTE_SIZE (256 * 3)
#define INT_SIZE 2
// VGA color palette for flames
unsigned char pal[PALETTE_SIZE] = {0, 0, 0, 0, 0, 24, 0, 0, 24, 0, 0, 28,
                                   0, 0, 32, 0, 0, 32, 0, 0, 36, 0, 0, 40,
                                   8, 0, 40, 16, 0, 36, 24, 0, 36, 32, 0, 32,
                                   40, 0, 28, 48, 0, 28, 56, 0, 24, 64, 0, 20,
                                   72, 0, 20, 80, 0, 16, 88, 0, 16, 96, 0, 12,
                                   104, 0, 8, 112, 0, 8, 120, 0, 4, 128, 0, 0,
                                   128, 0, 0, 132, 0, 0, 136, 0, 0, 140, 0, 0,
                                   144, 0, 0, 144, 0, 0, 148, 0, 0, 152, 0, 0,
                                   156, 0, 0, 160, 0, 0, 160, 0, 0, 164, 0, 0,
                                   168, 0, 0, 172, 0, 0, 176, 0, 0, 180, 0, 0,
                                   184, 4, 0, 188, 4, 0, 192, 8, 0, 196, 8, 0,
                                   200, 12, 0, 204, 12, 0, 208, 16, 0, 212, 16, 0,
                                   216, 20, 0, 220, 20, 0, 224, 24, 0, 228, 24, 0,
                                   232, 28, 0, 236, 28, 0, 240, 32, 0, 244, 32, 0,
                                   252, 36, 0, 252, 36, 0, 252, 40, 0, 252, 40, 0,
                                   252, 44, 0, 252, 44, 0, 252, 48, 0, 252, 48, 0,
                                   252, 52, 0, 252, 52, 0, 252, 56, 0, 252, 56, 0,
                                   252, 60, 0, 252, 60, 0, 252, 64, 0, 252, 64, 0,
                                   252, 68, 0, 252, 68, 0, 252, 72, 0, 252, 72, 0,
                                   252, 76, 0, 252, 76, 0, 252, 80, 0, 252, 80, 0,
                                   252, 84, 0, 252, 84, 0, 252, 88, 0, 252, 88, 0,
                                   252, 92, 0, 252, 96, 0, 252, 96, 0, 252, 100, 0,
                                   252, 100, 0, 252, 104, 0, 252, 104, 0, 252, 108, 0,
                                   252, 108, 0, 252, 112, 0, 252, 112, 0, 252, 116, 0,
                                   252, 116, 0, 252, 120, 0, 252, 120, 0, 252, 124, 0,
                                   252, 124, 0, 252, 128, 0, 252, 128, 0, 252, 132, 0,
                                   252, 132, 0, 252, 136, 0, 252, 136, 0, 252, 140, 0,
                                   252, 140, 0, 252, 144, 0, 252, 144, 0, 252, 148, 0,
                                   252, 152, 0, 252, 152, 0, 252, 156, 0, 252, 156, 0,
                                   252, 160, 0, 252, 160, 0, 252, 164, 0, 252, 164, 0,
                                   252, 168, 0, 252, 168, 0, 252, 172, 0, 252, 172, 0,
                                   252, 176, 0, 252, 176, 0, 252, 180, 0, 252, 180, 0,
                                   252, 184, 0, 252, 184, 0, 252, 188, 0, 252, 188, 0,
                                   252, 192, 0, 252, 192, 0, 252, 196, 0, 252, 196, 0,
                                   252, 200, 0, 252, 200, 0, 252, 204, 0, 252, 208, 0,
                                   252, 208, 0, 252, 208, 0, 252, 208, 0, 252, 208, 0,
                                   252, 212, 0, 252, 212, 0, 252, 212, 0, 252, 212, 0,
                                   252, 216, 0, 252, 216, 0, 252, 216, 0, 252, 216, 0,
                                   252, 216, 0, 252, 220, 0, 252, 220, 0, 252, 220, 0,
                                   252, 220, 0, 252, 224, 0, 252, 224, 0, 252, 224, 0,
                                   252, 224, 0, 252, 228, 0, 252, 228, 0, 252, 228, 0,
                                   252, 228, 0, 252, 228, 0, 252, 232, 0, 252, 232, 0,
                                   252, 232, 0, 252, 232, 0, 252, 236, 0, 252, 236, 0,
                                   252, 236, 0, 252, 236, 0, 252, 240, 0, 252, 240, 0,
                                   252, 240, 0, 252, 240, 0, 252, 240, 0, 252, 244, 0,
                                   252, 244, 0, 252, 244, 0, 252, 244, 0, 252, 248, 0,
                                   252, 248, 0, 252, 248, 0, 252, 248, 0, 252, 252, 0,
                                   252, 252, 4, 252, 252, 8, 252, 252, 12, 252, 252, 16,
                                   252, 252, 20, 252, 252, 24, 252, 252, 28, 252, 252, 32,
                                   252, 252, 36, 252, 252, 40, 252, 252, 40, 252, 252, 44,
                                   252, 252, 48, 252, 252, 52, 252, 252, 56, 252, 252, 60,
                                   252, 252, 64, 252, 252, 68, 252, 252, 72, 252, 252, 76,
                                   252, 252, 80, 252, 252, 84, 252, 252, 84, 252, 252, 88,
                                   252, 252, 92, 252, 252, 96, 252, 252, 100, 252, 252, 104,
                                   252, 252, 108, 252, 252, 112, 252, 252, 116, 252, 252, 120,
                                   252, 252, 124, 252, 252, 124, 252, 252, 128, 252, 252, 132,
                                   252, 252, 136, 252, 252, 140, 252, 252, 144, 252, 252, 148,
                                   252, 252, 152, 252, 252, 156, 252, 252, 160, 252, 252, 164,
                                   252, 252, 168, 252, 252, 168, 252, 252, 172, 252, 252, 176,
                                   252, 252, 180, 252, 252, 184, 252, 252, 188, 252, 252, 192,
                                   252, 252, 196, 252, 252, 200, 252, 252, 204, 252, 252, 208,
                                   252, 252, 208, 252, 252, 212, 252, 252, 216, 252, 252, 220,
                                   252, 252, 224, 252, 252, 228, 252, 252, 232, 252, 252, 236,
                                   252, 252, 240, 252, 252, 244, 252, 252, 248, 252, 252, 252};



// downloaded from fontsquirrel.com and header generated with
// https://honeythecodewitch.com/gfx/generator
#define ARCHITECTS_DAUGHTER_IMPLEMENTATION
#include <assets/architects_daughter.hpp>
static const open_font& text_font = architects_daughter;

// declare the format of the screen
using screen_t = screen<rgb_pixel<16>>;
// declare the control types to match the screen
using label_t = label<typename screen_t::control_surface_type>;
using canvas_t = canvas<typename screen_t::control_surface_type>;
using color_t = color<typename screen_t::pixel_type>;
// for access to RGB8888 colors which controls use
using color32_t = color<rgba_pixel<32>>;
// declare the TTGO buttons
using button_a_t = int_button<PIN_BUTTON_A, 10, true>;
using button_b_t = int_button<PIN_BUTTON_B, 10, true>;


template<typename ControlSurfaceType>
class fire_box : public control<ControlSurfaceType> {
    int draw_state = 0;
    int p1[BUF_HEIGHT][BUF_WIDTH];   // VGA buffer, quarter resolution w/extra lines
    unsigned int i, j, k, l, delta;  // looping variables, counters, and data
    char ch;         
public:
    using control_surface_type = ControlSurfaceType;
    using base_type = control<control_surface_type>;
    using pixel_type = typename base_type::pixel_type;
    using palette_type = typename base_type::palette_type;
    fire_box(uix::invalidation_tracker& parent, const palette_type* palette = nullptr)
        : base_type(parent, palette) {
    }
    fire_box(fire_box&& rhs) {
        do_move_control(rhs);
        draw_state = 0;
    }
    fire_box& operator=(fire_box&& rhs) {
        do_move_control(rhs);
        draw_state = 0;
        return *this;
    }
    fire_box(const fire_box& rhs) {
        do_copy_control(rhs);
        draw_state = 0;
    }
    fire_box& operator=(const fire_box& rhs) {
        do_copy_control(rhs);
        draw_state = 0;
        return *this;
    }
    virtual void on_before_render() {
        switch (draw_state) {
            case 0:
                // Initialize the buffer to 0s
                for (i = 0; i < BUF_HEIGHT; i++) {
                    for (j = 0; j < BUF_WIDTH; j++) {
                        p1[i][j] = 0;
                    }
                }
                draw_state = 1;
            case 1:
                // Transform current buffer
                for (i = 1; i < BUF_HEIGHT; ++i) {
                    for (j = 0; j < BUF_WIDTH; ++j) {
                        if (j == 0)
                            p1[i - 1][j] = (p1[i][j] +
                                            p1[i - 1][79] +
                                            p1[i][j + 1] +
                                            p1[i + 1][j]) >>
                                           2;
                        else if (j == 79)
                            p1[i - 1][j] = (p1[i][j] +
                                            p1[i][j - 1] +
                                            p1[i + 1][0] +
                                            p1[i + 1][j]) >> 2
                                           ;
                        else
                            p1[i - 1][j] = (p1[i][j] +
                                            p1[i][j - 1] +
                                            p1[i][j + 1] +
                                            p1[i + 1][j]) >>
                                           2;

                        if (p1[i][j] > 11)
                            p1[i][j] = p1[i][j] - 12;
                        else if (p1[i][j] > 3)
                            p1[i][j] = p1[i][j] - 4;
                        else {
                            if (p1[i][j] > 0) p1[i][j]--;
                            if (p1[i][j] > 0) p1[i][j]--;
                            if (p1[i][j] > 0) p1[i][j]--;
                        }
                    }
                }
                delta = 0;
                for (j = 0; j < BUF_WIDTH; j++) {
                    if (rand()%10 < 5) {
                        delta = (rand()&1) * 255;
                    }
                    p1[BUF_HEIGHT - 2][j] = delta;
                    p1[BUF_HEIGHT - 1][j] = delta;
                }
        }
    }
    virtual void on_paint(control_surface_type& destination, const srect16& clip) override {
        rgb_pixel<16> col;
        for(int y = clip.y1;y<=clip.y2;++y) {
            for(int x = clip.x1;x<=clip.x2;++x) {
                int i = y/4;
                int j =x/4;
                uint8_t* pc = &pal[p1[i][j]*3];
                uint8_t r = *pc;
                ++pc;
                uint8_t g = *pc;
                ++pc;
                uint8_t b = *pc;
                col = rgb_pixel<16>(r>>3,g>>2,b>>3);
                point16 pt(x,y);
                destination.point(pt,col);
            }
        }
    }
};

using fire_box_t = fire_box<screen_t::control_surface_type>;
// UIX allows you to use two buffers for maximum DMA efficiency
// you don't have to, but performance is significantly better
// declare 64KB across two buffers for transfer
constexpr static const int lcd_buffer_size = 32 * 1024;
uint8_t lcd_buffer1[lcd_buffer_size];
uint8_t lcd_buffer2[lcd_buffer_size];
// this is the handle from the esp panel api
esp_lcd_panel_handle_t lcd_handle;

// the TTGO buttons
button_a_t button_a;
button_b_t button_b;

// the main screen
screen_t main_screen({LCD_VRES, LCD_HRES}, sizeof(lcd_buffer1), lcd_buffer1, lcd_buffer2);

// the controls
label_t fps_label(main_screen);
fire_box_t anim_canvas(main_screen);

// tell UIX the DMA transfer is complete
static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t* edata, void* user_ctx) {
    main_screen.flush_complete();
    return true;
}
// tell the lcd panel api to transfer data via DMA
static void uix_flush(const rect16& bounds, const void* bmp, void* state) {
    int x1 = bounds.x1, y1 = bounds.y1, x2 = bounds.x2 + 1, y2 = bounds.y2 + 1;
    esp_lcd_panel_draw_bitmap(lcd_handle, x1, y1, x2, y2, (void*)bmp);
}
// initialize the screen using the esp panel API
void lcd_panel_init() {
    pinMode(PIN_NUM_BK_LIGHT, OUTPUT);

    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.sclk_io_num = PIN_NUM_PCLK;
    buscfg.mosi_io_num = PIN_NUM_DATA0;
    buscfg.miso_io_num = -1;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = sizeof(lcd_buffer1) + 8;

    // Initialize the SPI bus on VSPI (SPI3)
    spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config;
    memset(&io_config, 0, sizeof(io_config));
    io_config.dc_gpio_num = PIN_NUM_DC,
    io_config.cs_gpio_num = PIN_NUM_CS,
    io_config.pclk_hz = LCD_PIXEL_CLOCK_HZ,
    io_config.lcd_cmd_bits = 8,
    io_config.lcd_param_bits = 8,
    io_config.spi_mode = 0,
    io_config.trans_queue_depth = 10,
    io_config.on_color_trans_done = lcd_flush_ready;
    // Attach the LCD to the SPI bus
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI3_HOST, &io_config, &io_handle);

    lcd_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config;
    memset(&panel_config, 0, sizeof(panel_config));
    panel_config.reset_gpio_num = PIN_NUM_RST;
    panel_config.color_space = ESP_LCD_COLOR_SPACE_RGB;
    panel_config.bits_per_pixel = 16;

    // Initialize the LCD configuration
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &lcd_handle);

    // Turn off backlight to avoid unpredictable display on the LCD screen while initializing
    // the LCD panel driver. (Different LCD screens may need different levels)
    digitalWrite(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_OFF_LEVEL);

    // Reset the display
    esp_lcd_panel_reset(lcd_handle);

    // Initialize LCD panel
    esp_lcd_panel_init(lcd_handle);
    // esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_SLPOUT, NULL, 0);
    //  Swap x and y axis (Different LCD screens may need different options)
    esp_lcd_panel_swap_xy(lcd_handle, true);
    esp_lcd_panel_set_gap(lcd_handle, 40, 52);
    esp_lcd_panel_mirror(lcd_handle, false, true);
    esp_lcd_panel_invert_color(lcd_handle, true);
    // Turn on the screen
    esp_lcd_panel_disp_off(lcd_handle, false);
    // Turn on backlight (Different LCD screens may need different levels)
    digitalWrite(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);
}

// initialize the screen and controls
void screen_init() {
        const rgba_pixel<32> transparent(0, 0, 0, 0);

    anim_canvas.bounds(main_screen.bounds());
    fps_label.text_color(color32_t::red);
    fps_label.text_open_font(&text_font);
    fps_label.text_line_height(40);
    fps_label.padding({0, 0});
    rgba_pixel<32> bg(0, 0, 0, 224);
    fps_label.background_color(bg);
    fps_label.text_justify(uix_justify::bottom_right);
    fps_label.bounds(srect16(0, main_screen.bounds().y2 - fps_label.text_line_height() + 2, main_screen.bounds().x2, main_screen.bounds().y2));
    fps_label.visible(false);
    main_screen.register_control(anim_canvas);
#ifdef SHOW_FPS
    main_screen.register_control(fps_label);
#endif
    main_screen.on_flush_callback(uix_flush);
}
// set up the hardware
void setup() {
    Serial.begin(115200);
    // init the display
    lcd_panel_init();
    // init the UI screen
    screen_init();
    // init the buttons
    button_a.initialize();
    button_b.initialize();
    // set button callbacks
    button_a.on_pressed_changed([](bool pressed, void* state) {
        Serial.println("button_a");
        fps_label.visible(pressed);
        
    });
    button_b.on_pressed_changed([](bool pressed, void* state) {
        Serial.println("button_b");
        fps_label.visible(pressed);
    });
    Serial.println("Booted");
}
// keep our stuff up to date and responsive
void loop() {
    // update the buttons
    button_a.update();
    button_b.update();
    static int frames = 0;
    static char szfps[16];
    // if we're on the animation screen

    static uint32_t fps_ts = 0;
    uint32_t ms = millis();
    // force the canvas to repaint
    anim_canvas.invalidate();
    ++frames;

    if (ms > fps_ts + 1000) {
        fps_ts = ms;
        snprintf(szfps, sizeof(szfps), "fps: %d", frames);
        Serial.println(szfps);
#ifdef SHOW_FPS
        fps_label.text(szfps);
#endif
        frames = 0;
    }

    main_screen.update();
}