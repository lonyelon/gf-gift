#include <Arduino.h>
#include <SensorPCF8563.hpp>
#include <WiFi.h>
#include <esp_sntp.h>
#include "esp_adc_cal.h"

#include "config.hpp"
#include "datelib.hpp"
#include "../font/roboto-14pt.h"
#include "../font/roboto-mono-7pt.h"
#include "../font/roboto-mono-12pt.h"
#include "../font/roboto-mono-16pt.h"
#include "../font/roboto-mono-40pt.h"

const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

SensorPCF8563 rtc;

int vref = 1100;
char buf[128];

void draw() {
    epd_poweron();
    epd_clear();

    // It is better to perform the battery measurement after the power on
    // and some time for thing to settle.
    delay(10); // Make adc measurement more accurate
    uint16_t v = analogRead(BATT_PIN);
    float battery_voltage = ((float)v/4095.0)*2.0*3.3*(vref/1000.0);
    String voltage = String(battery_voltage) + "V";

    Rect_t img_area = {
        .x = SCREEN_W - IMG_W,
        .y = 0,
        .width = IMG_W,
        .height = SCREEN_H,
    };
    epd_draw_grayscale_image(img_area, images[millis() % images.size()]);

    auto current_clock = std::chrono::system_clock::now();
    std::time_t current_time_t = std::chrono::system_clock::to_time_t(current_clock);
    std::tm current_tm = *std::localtime(&current_time_t);

    std::tm anniversary_tm = {
        .tm_min = ANNIVERSARY_MIN,
        .tm_hour = ANNIVERSARY_HOUR - 2, // -2 for timezone correction.
        .tm_mday = ANNIVERSARY_DAY, // This one starts at 1.
        .tm_mon = ANNIVERSARY_MONTH - 1, // Remember that 0 is JAN.
        .tm_year = ANNIVERSARY_YEAR - 1900, // Year starts counting on 1900.
    };

    int cursor_x = 150;
    int cursor_y = 100;
    writeln((GFXfont *)&roboto_mono_16pt, "Llevamos juntos",
        &cursor_x, &cursor_y, NULL);

    cursor_x = 30;
    cursor_y += 75;
    auto result = getDateDiff(anniversary_tm, current_tm);
    snprintf(buf, 128, "%02d %02d %02d %02d", result[0].tm_year,
        result[0].tm_mon, result[0].tm_mday, result[0].tm_hour);
    writeln((GFXfont *)&roboto_mono_40pt, buf, &cursor_x, &cursor_y, NULL);

    cursor_x = 50;
    cursor_y += 25;
    writeln((GFXfont *)&roboto_14pt,
            "años           meses           días            horas",
            &cursor_x, &cursor_y, NULL);

    int print_len = snprintf(buf, 128, "(O %d horas; o %d días;",
                             result[1].tm_hour, result[1].tm_mday);
    cursor_x = (SCREEN_W - IMG_W - 15*print_len)/2;
    cursor_y += 100;
    writeln((GFXfont*)&roboto_mono_12pt, buf, &cursor_x, &cursor_y, NULL);

    print_len = snprintf(buf, 128, "o %d semanas; o %d meses)",
        result[1].tm_mday / 7, result[1].tm_mon);
    cursor_x = (SCREEN_W - IMG_W - 15*print_len)/2;
    cursor_y += 30;
    writeln((GFXfont *)&roboto_mono_12pt, buf, &cursor_x, &cursor_y, NULL);

    cursor_x = (SCREEN_W - IMG_W - 20*23)/2;
    cursor_y = SCREEN_H - 35*3;
    writeln((GFXfont *)&roboto_mono_16pt, "Y más que serán, porque",
            &cursor_x, &cursor_y, NULL);

    // I would use a random distribution, but since milliseconds here are
    // almost random, it is ok.
    print_len = snprintf(buf, 128, "%s.",
                         phrases[millis() % phrases.size()]);
    cursor_x = (SCREEN_W - IMG_W - 20*print_len)/2;
    cursor_y += 35,
    writeln((GFXfont *)&roboto_mono_16pt, buf, &cursor_x, &cursor_y, NULL);

    cursor_x = 15;
    cursor_y = SCREEN_H - 10;
    writeln((GFXfont *)&roboto_mono_7pt, (char *)voltage.c_str(),
            &cursor_x, &cursor_y, NULL);

    epd_poweroff_all();
}

void setup() {
    btStop(); // No bluetooth needed.

    // Reduce it to 40Mhz and the ESP32 becomes braindead.
    // setCpuFrequencyMhz(80);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Correct the ADC reference voltage.
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
        ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars
    );

    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
        vref = adc_chars.vref;

    epd_init();

    configTzTime(TIME_ZONE, NTP_SERVER_1, NTP_SERVER_2);
    sntp_set_time_sync_notification_cb([] (struct timeval *t) {
        rtc.hwClockWrite();
        //WiFi.mode(WIFI_OFF); // This would be cool but it crashes.
        draw(); // Refresh screen after getting time info from NTP server.
        esp_sleep_enable_timer_wakeup(20 * 60 * 1000000ULL);
        esp_deep_sleep_start();
    });
}

// If we don't have something in the loop then the program crashes.
void loop() {
    delay(10000);
}
