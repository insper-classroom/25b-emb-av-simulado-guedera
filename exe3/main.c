/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

const int LED_PIN_B = 8;
const int LED_PIN_Y = 13;
const int LED_PIN_R = 22;

QueueHandle_t xQueueInput;

void input_task(void* p) {
    vTaskDelay(pdMS_TO_TICKS(350));
    int test_case;

    test_case = 5;
    xQueueSend(xQueueInput, &test_case, 0);

    test_case = 2;
    xQueueSend(xQueueInput, &test_case, 0);

    while (true) {
        vTaskDelay(portMAX_DELAY);
    }
}

QueueHandle_t xQueueLed1;
QueueHandle_t xQueueLed2;
SemaphoreHandle_t xSemaphoreLed3;
volatile bool piscar_vermelho = false;

void task_led_3(void *p) {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    for (;;) {
        if (xSemaphoreTake(xSemaphoreLed3, portMAX_DELAY) == pdTRUE) {
            while (piscar_vermelho) {
                gpio_put(LED_PIN_R, 1);
                vTaskDelay(pdMS_TO_TICKS(100));
                gpio_put(LED_PIN_R, 0);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            gpio_put(LED_PIN_R, 0);
        }
    }
}

void task_led_2(void *p) {
    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    int contador = 0;
    for (;;) {
        if (xQueueReceive(xQueueLed2, &contador, portMAX_DELAY) == pdTRUE) {
            if (contador > 0) {
                gpio_put(LED_PIN_B, 1);
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_put(LED_PIN_B, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
                contador--;
            }
            xQueueSend(xQueueLed1, &contador, portMAX_DELAY);
        }
    }
}

void task_led_1(void *p) {
    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);
    int contador = 0;
    for (;;) {
        xQueueReceive(xQueueInput, &contador, portMAX_DELAY);
        while (contador > 0) {
            piscar_vermelho = true;
            xSemaphoreGive(xSemaphoreLed3);
            gpio_put(LED_PIN_Y, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(LED_PIN_Y, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            piscar_vermelho = false;
            contador--;
            if (contador == 0) break;
            xQueueSend(xQueueLed2, &contador, portMAX_DELAY);
            xQueueReceive(xQueueLed1, &contador, portMAX_DELAY);
        }
    }
}


int main() {
    stdio_init_all();

    xQueueInput = xQueueCreate(32, sizeof(int));
    xTaskCreate(input_task, "Input", 256, NULL, 1, NULL);

    xQueueLed1 = xQueueCreate(16, sizeof(int));
    xQueueLed2 = xQueueCreate(16, sizeof(int));
    xSemaphoreLed3 = xSemaphoreCreateBinary();

    xTaskCreate(task_led_1, "LED1", 256, NULL, 2, NULL);
    xTaskCreate(task_led_2, "LED2", 256, NULL, 2, NULL);
    xTaskCreate(task_led_3, "LED3", 256, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1) {}

    return 0;
}
