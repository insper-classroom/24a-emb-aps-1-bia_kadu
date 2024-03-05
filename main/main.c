/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const int BTN_RED = 13;
const int BTN_BLUE = 11;
const int BTN_GREEN = 12;
const int BTN_YELLOW = 10;

const int LED_RED = 9;
const int LED_BLUE = 7;
const int LED_GREEN = 8;
const int LED_YELLOW = 6;

const int BUZZER = 14;

volatile int btnf_red= 0;
volatile int btnf_blue = 0;
volatile int btnf_green = 0;
volatile int btnf_yellow = 0;

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
void sound(int buzzer,float time, int frequency){
    int delay = 1e6/(2*frequency);
    for (int i = 0; i < time*frequency; i++) {
      gpio_put(buzzer, 1);
      sleep_us(delay);
      gpio_put(buzzer, 0);
      sleep_us(delay);
    }
}

int main() {

    float time = 0.5;

    int f_red = 495;
    int f_blue = 440;
    int f_green = 396;
    int f_yellow = 352;

    int vec_ordem[4] = {LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE};

    int fs[4] = {f_red, f_yellow, f_green, f_blue};

    int count = 0;
    int nivel = 0;

    stdio_init_all();

    pin_init();

    gpio_set_irq_enabled_with_callback(BTN_RED, GPIO_IRQ_EDGE_FALL, true,
                                     &btn_callback);

    gpio_set_irq_enabled(BTN_BLUE, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_GREEN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_YELLOW, GPIO_IRQ_EDGE_FALL, true);

    int tocou = 0;
    int j;


    while (true) {

        if (!tocou) {
            for (int i = 0; i < 4+nivel; i++) {
                j = i%4;
                led_buzzer(vec_ordem[j], BUZZER, time, fs[j]);
            }
            tocou = 1;
        }

        if (btnf_red) {
            led_buzzer(LED_RED, BUZZER, time, f_red);
            btnf_red = 0;
            if (vec_ordem[count%4] != LED_RED) {
                printf("Voce perdeu");
                nivel = 0;
                count = 0;
                tocou = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_blue) {
            led_buzzer(LED_BLUE, BUZZER, time, f_blue);
            btnf_blue = 0;
            if (vec_ordem[count%4] != LED_BLUE) {
                printf("Voce perdeu");
                nivel = 0;
                count = 0;
                tocou = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_green) {
            led_buzzer(LED_GREEN, BUZZER, time, f_green);
            btnf_green = 0;
            if (vec_ordem[count%4] != LED_GREEN) {
                printf("Voce perdeu");
                nivel = 0;
                count = 0;
                tocou = 0;
                sleep_ms(2000);
            }
            count ++;
        }
        if (btnf_yellow) {
            led_buzzer(LED_YELLOW, BUZZER, time, f_yellow);
            btnf_yellow = 0;
            if (vec_ordem[count%4] != LED_YELLOW) {
                printf("Voce perdeu");
                nivel = 0;
                count = 0;
                tocou = 0;
                sleep_ms(2000);
            }
            count ++;
        }

        if (count == 4+nivel) {
            printf("Voce ganhou esse nivel");
            nivel ++;
            count = 0;
            tocou = 0;
            sleep_ms(2000);
        }

    }
}