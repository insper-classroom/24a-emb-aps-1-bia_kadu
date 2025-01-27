/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/flash.h>

# define FLASH_WRITE_START (PICO_FLASH_SIZE_BYTES - 4096);
# define FLASH_READ_START (PICO_FLASH_SIZE_BYTES - 4096 + XIP_BASE);

#define NOTE_FS3 185
#define NOTE_E3 165
#define NOTE_B2 123

#define NOTE_FS5 740
#define NOTE_E5 659
#define NOTE_B4 494
#define NOTE_AS5 932

const int BTN_RED = 26;
const int BTN_BLUE = 4;
const int BTN_GREEN = 7;
const int BTN_YELLOW = 27;

const int BTN_PLAY = 22;

const int LED_RED = 19;
const int LED_BLUE = 3;
const int LED_GREEN = 6;
const int LED_YELLOW = 20;

const int LED_PLAY = 13;

const int BUZZER = 15;

volatile int btnf_red= 0;
volatile int btnf_blue = 0;
volatile int btnf_green = 0;
volatile int btnf_yellow = 0;
volatile int btnf_play = 0;
volatile bool timer_fired = false;

const float time = 0.5;

const int f_red = 495;
const int f_blue = 440;
const int f_green = 396;
const int f_yellow = 352;

const int leds[4] = {LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE};

const int fs[4] = {f_red, f_yellow, f_green, f_blue};

void pin_init() {
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);

    gpio_init(LED_PLAY);
    gpio_set_dir(LED_PLAY, GPIO_OUT);

    gpio_init(BTN_RED);
    gpio_set_dir(BTN_RED, GPIO_IN);
    gpio_pull_up(BTN_RED);

    gpio_init(BTN_BLUE);
    gpio_set_dir(BTN_BLUE, GPIO_IN);
    gpio_pull_up(BTN_BLUE);

    gpio_init(BTN_GREEN);
    gpio_set_dir(BTN_GREEN, GPIO_IN);
    gpio_pull_up(BTN_GREEN);

    gpio_init(BTN_YELLOW);
    gpio_set_dir(BTN_YELLOW, GPIO_IN);
    gpio_pull_up(BTN_YELLOW);

    gpio_init(BTN_PLAY);
    gpio_set_dir(BTN_PLAY, GPIO_IN);
    gpio_pull_up(BTN_PLAY);
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4 && gpio == BTN_RED) {
        btnf_red = 1;
    }
    if (events == 0x4 && gpio == BTN_BLUE) {
        btnf_blue = 1;
    }
    if (events == 0x4 && gpio == BTN_GREEN) {
        btnf_green = 1;
    }
    if (events == 0x4 && gpio == BTN_YELLOW) {
        btnf_yellow = 1;
    }
    if (events == 0x4 && gpio == BTN_PLAY) {
        btnf_play = 1;
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    timer_fired = true;
    return 0;
}

void play_buzzer(int buzzer, float time, int frequency) {
    int delay = 1e6 / (2*frequency);
    for(int i = 0; i < time * frequency; i++) {
        gpio_put(buzzer, 1);
        sleep_us(delay);
        gpio_put(buzzer, 0);
        sleep_us(delay);
    }
}

void led_buzzer(int led, int buzzer, float time, int frequency) {
    gpio_put(led, 1);
    play_buzzer(buzzer, time, frequency);
    gpio_put(led, 0);
    sleep_us(1e5);
}

void play_lost() {
    play_buzzer(BUZZER, 0.25, NOTE_FS5);
    sleep_ms(0.3*0.25);
    play_buzzer(BUZZER, 0.25, NOTE_E5);
    sleep_ms(0.3*0.25);
    play_buzzer(BUZZER, 1, NOTE_B4);
    sleep_ms(0.3*1);
}

void play_win() {
    play_buzzer(BUZZER, 0.25, NOTE_FS5);
    sleep_ms(0.3*0.25);
    play_buzzer(BUZZER, 0.5, NOTE_AS5);
    sleep_ms(0.3*0.5);
}

void form_level(int vec[], int nivel, int time_start) {
    int i;
    int j;
    if (nivel == 0){
        srand(time_start);

        for (i = 0; i < 4; i++) {
            j = rand() %4;
            vec[i] = j;
            led_buzzer(leds[j], BUZZER, time, fs[j]);
        }

    } else {
        j = rand() %4;
        vec[3+nivel] = j;

        for (i = 0; i < 4+nivel; i++) {
            j = vec[i];
            led_buzzer(leds[j], BUZZER, time, fs[j]);
        }
    }
}

int phase(const int vec[], int n) {
    int count = 0;

    timer_fired = false;
    btnf_red = 0;
    btnf_yellow = 0;
    btnf_green = 0;
    btnf_blue = 0;

    alarm_id_t alarm = 0;
    alarm = add_alarm_in_ms(3000, alarm_callback, NULL, false);

    while (true) {
        int j = vec[count];

        if (btnf_red) {
            led_buzzer(LED_RED, BUZZER, time, f_red);
            btnf_red = 0;
            if (leds[j] != LED_RED) {
                printf("Voce perdeu \n");
                return true;
            } else if (!timer_fired) {
                cancel_alarm(alarm);
                alarm = add_alarm_in_ms(3000, alarm_callback, NULL, false);
            }
            count++;
        }

        else if (btnf_yellow) {
            led_buzzer(LED_YELLOW, BUZZER, time, f_yellow);
            btnf_yellow = 0;
            if (leds[j] != LED_YELLOW) {
                printf("Voce perdeu \n");
                return true;
            } else if (!timer_fired) {
                cancel_alarm(alarm);
                alarm = add_alarm_in_ms(3000, alarm_callback, NULL, false);
            }
            count++;
        }

        else if (btnf_green) {
            led_buzzer(LED_GREEN, BUZZER, time, f_green);
            btnf_green = 0;
            if (leds[j] != LED_GREEN) {
                printf("Voce perdeu \n");
                return true;
            } else if (!timer_fired) {
                cancel_alarm(alarm);
                alarm = add_alarm_in_ms(3000, alarm_callback, NULL, false);
            }
            count++;
        }

        else if (btnf_blue) {
            led_buzzer(LED_BLUE, BUZZER, time, f_blue);
            btnf_blue = 0;
            if (leds[j] != LED_BLUE) {
                printf("Voce perdeu \n");
                return true;
            } else if (!timer_fired) {
                cancel_alarm(alarm);
                alarm = add_alarm_in_ms(3000, alarm_callback, NULL, false);
            }
            count++;
        }

        if (timer_fired) {
            printf("Timer: Voce perdeu\n");
            return true;
        }

        if (count == n) {
            printf("Voce ganhou esse nivel \n");
            return false;
        }
    }
}

void check_record(int nivel) {
    char *flashStr = (char *)FLASH_READ_START;
    char *ptr;
    int value;
    value = strtol(flashStr, &ptr, 10);

    if (nivel > value) {
        char record[FLASH_PAGE_SIZE];
        sprintf(record, "%d", nivel);

        flash_range_erase (PICO_FLASH_SIZE_BYTES - 4096, 4096);

        flash_range_program (PICO_FLASH_SIZE_BYTES - 4096, (const uint8_t*)record, FLASH_PAGE_SIZE);

        printf("Novo recorde: %s \n", flashStr);
    }
}

void get_record() {
    char *flashStr = (char *)FLASH_READ_START;
    printf("Recorde atual: %s \n", flashStr);
}

int hold() {
    while (!btnf_play) {
        gpio_put(LED_PLAY, 1);
        sleep_ms(1000);
        gpio_put(LED_PLAY, 0);
        sleep_ms(1000);
    }
    get_record();
    return to_ms_since_boot(get_absolute_time());
}

void show_result(int nivel) {
    gpio_put(leds[0], 1);
    gpio_put(leds[2], 1);
    sleep_ms(250);

    gpio_put(leds[0], 0);
    gpio_put(leds[2], 0);
    sleep_ms(250);

    gpio_put(leds[1], 1);
    gpio_put(leds[3], 1);
    sleep_ms(250);

    gpio_put(leds[1], 0);
    gpio_put(leds[3], 0);
    sleep_ms(250);

    play_lost();
    
    for (int i = 0; i < nivel; i++) {
        int iled = i % 4;
        gpio_put(leds[iled], 1);
        sleep_ms(250);
        gpio_put(leds[iled],0);
        sleep_ms(250);
    }
}

void show_win() {
    for (int i = 0; i < 4; i++) {
        gpio_put(leds[i],1);
    }
    sleep_ms(500);
    for (int i = 0; i < 4; i++) {
        gpio_put(leds[i],0);
    }
}

int main() {

    stdio_init_all();

    pin_init();

    gpio_set_irq_enabled_with_callback(BTN_RED, GPIO_IRQ_EDGE_FALL, true,
                                     &btn_callback);

    gpio_set_irq_enabled(BTN_BLUE, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_GREEN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_YELLOW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PLAY, GPIO_IRQ_EDGE_FALL, true);

    int nivel = 0;
    int time_start;
    int vec_random[100];

    while (true) {
        bool lost;
        if (!btnf_play) {
            time_start = hold();
        }
        form_level(vec_random, nivel, time_start);
        lost = phase(vec_random, nivel+4);
        if (lost) {
            check_record(nivel-1);
            show_result(nivel);
            
            nivel = 0;
            btnf_play = 0;
        } else {
            show_win();
            play_win();
            nivel ++;
        }
        sleep_ms(1000);
    }
}