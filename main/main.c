/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int BTN_RED = 26;
const int BTN_BLUE = 5;
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

const float time = 0.5;

const int f_red = 495;
const int f_blue = 440;
const int f_green = 396;
const int f_yellow = 352;

const int vec_ordem[4] = {LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE};

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

void led_buzzer(int led, int buzzer, float time, int frequency) {
    gpio_put(led, 1);
    int delay = 1e6/(2*frequency);
    for (int i = 0; i < time*frequency; i++) {
      gpio_put(buzzer, 1);
      sleep_us(delay);
      gpio_put(buzzer, 0);
      sleep_us(delay);
    }
    gpio_put(led, 0);

}

void form_level(int vec[], int n, int time_start) {
    printf("tocando");
    srand(time_start);
    int j;

    for (int i = 0; i < n; i++) {
        j = rand() %4;
        vec[i] = j;
        led_buzzer(vec_ordem[j], BUZZER, time, fs[j]);
    }
}

void game(int vec[], int n, int nivel) {
    int game_on = 1;
    int count = 0;
    int j;
    while (game_on) {
        j = vec[count];
        if (btnf_red) {
            printf("red");
            led_buzzer(LED_RED, BUZZER, time, f_red);
            btnf_red = 0;
            if (vec_ordem[j] != LED_RED) {
                printf("Voce perdeu");
                nivel = 0;
                game_on = 0;
                count = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_blue) {
            printf("blue");
            led_buzzer(LED_BLUE, BUZZER, time, f_blue);
            btnf_blue = 0;
            if (vec_ordem[j] != LED_BLUE) {
                printf("Voce perdeu");
                nivel = 0;
                game_on = 0;
                count = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_green) {
            printf("green");
            led_buzzer(LED_GREEN, BUZZER, time, f_green);
            btnf_green = 0;
            if (vec_ordem[j] != LED_GREEN) {
                printf("Voce perdeu");
                nivel = 0;
                game_on = 0;
                count = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_yellow) {
            printf("yellow");
            led_buzzer(LED_YELLOW, BUZZER, time, f_yellow);
            btnf_yellow = 0;
            if (vec_ordem[j] != LED_YELLOW) {
                printf("Voce perdeu");
                game_on = 0;
                nivel = 0;
                count = 0;
                sleep_ms(2000);
            }
            count ++;
        }

        if (count == 4+nivel) {
            printf("Voce ganhou esse nivel");
            nivel ++;
            game_on = 0;
            count = 0;
            sleep_ms(2000);
        }
    }
}
int main() {

    int nivel = 0;

    stdio_init_all();

    pin_init();

    gpio_set_irq_enabled_with_callback(BTN_RED, GPIO_IRQ_EDGE_FALL, true,
                                     &btn_callback);

    gpio_set_irq_enabled(BTN_BLUE, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_GREEN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_YELLOW, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_PLAY, GPIO_IRQ_EDGE_FALL, true);

    int time_start;

    while (true) {

        int vec_random[4+nivel];


        if (btnf_play) {
            time_start = to_ms_since_boot(get_absolute_time());
            form_level(vec_random, 4+nivel, time_start);
            game(vec_random, nivel+4, nivel);
            btnf_play = 0;
        }
    }
}