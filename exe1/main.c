#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

#define BTN1_PIN 11
#define BTN2_PIN 12
#define BTN3_PIN 13
#define LED1_PIN 14
#define LED2_PIN 15
#define LED3_PIN 16

bool led1_blink = false;
bool led2_blink = false;
bool led3_blink = false;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN1_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        led1_blink = true;
    } else if (gpio == BTN2_PIN && (events & GPIO_IRQ_EDGE_RISE)) {
        led2_blink = true;
    } else if (gpio == BTN3_PIN) {
        if (events & GPIO_IRQ_EDGE_FALL) {
            led3_blink = true;
        } else if (events & GPIO_IRQ_EDGE_RISE) {
            led3_blink = false;
        }
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(BTN1_PIN);
    gpio_set_dir(BTN1_PIN, GPIO_IN);
    gpio_pull_up(BTN1_PIN);
    
    gpio_init(BTN2_PIN);
    gpio_set_dir(BTN2_PIN, GPIO_IN);
    gpio_pull_up(BTN2_PIN);
    
    gpio_init(BTN3_PIN);
    gpio_set_dir(BTN3_PIN, GPIO_IN);
    gpio_pull_up(BTN3_PIN);
    
    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    
    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    
    gpio_init(LED3_PIN);
    gpio_set_dir(LED3_PIN, GPIO_OUT);
    
    gpio_set_irq_enabled_with_callback(BTN1_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BTN2_PIN, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(BTN3_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    uint32_t last_time = 0;
    bool led_state = false;

    while (true) {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        if (current_time - last_time >= 500) {
            led_state = !led_state;
            last_time = current_time;
            
            if (led1_blink) {
                gpio_put(LED1_PIN, led_state);
            } else {
                gpio_put(LED1_PIN, 0);
            }
            
            if (led2_blink) {
                gpio_put(LED2_PIN, led_state);
            } else {
                gpio_put(LED2_PIN, 0);
            }
            
            if (led3_blink) {
                gpio_put(LED3_PIN, led_state);
            } else {
                gpio_put(LED3_PIN, 0);
            }
        }
    }
}
