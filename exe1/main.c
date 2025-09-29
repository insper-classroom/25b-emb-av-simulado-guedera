#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

#define BTN_PIN 22
#define SW_PIN 28
#define LED_PINS_START 2
#define LED_COUNT 5

volatile int counter = 0;
volatile bool sw_state = false;

void bar_init() {
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_init(LED_PINS_START + i);
        gpio_set_dir(LED_PINS_START + i, GPIO_OUT);
        gpio_put(LED_PINS_START + i, 0);
    }
}

void bar_display(int val) {
    if (val < 0) val = 0;
    if (val > LED_COUNT) val = LED_COUNT;
    
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_put(LED_PINS_START + i, i < val ? 1 : 0);
    }
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == SW_PIN) {
        if (events & GPIO_IRQ_EDGE_FALL) {
            sw_state = true;
        } else if (events & GPIO_IRQ_EDGE_RISE) {
            sw_state = false;
        }
    } else if (gpio == BTN_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        if (sw_state) {
            counter--;
            if (counter < 0) counter = 0;
        } else {
            counter++;
            if (counter > LED_COUNT) counter = LED_COUNT;
        }
        bar_display(counter);
    }
}

int main() {
    stdio_init_all();
    
    bar_init();
    
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);
    
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BTN_PIN, GPIO_IRQ_EDGE_FALL, true);
    
    bar_display(counter);

    while (true) {
        //todo o código fica nos callbacks
    }
}
