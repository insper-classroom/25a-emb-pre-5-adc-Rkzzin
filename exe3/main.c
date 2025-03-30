#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int data_arr[] = {0, 0, 0, 0, 0};
    int count = 0;
    float media = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            if (count < 5) {
                data_arr[count] = data;


                if (count == 0) {
                    media = data;
                } else if (count == 1) {
                    media = (data_arr[0] + data_arr[1])/2;
                } else if (count == 2) {
                    media = (data_arr[0] + data_arr[1] + data_arr[2])/3;
                } else if (count == 3) {
                    media = (data_arr[0] + data_arr[1] + data_arr[2] + data_arr[3])/4;
                } else if (count == 4) {
                    media = (data_arr[0] + data_arr[1] + data_arr[2] + data_arr[3] + data_arr[4])/5;
                }

                count += 1;
            } else {
                data_arr[0] = data_arr[1];
                data_arr[1] = data_arr[2];
                data_arr[2] = data_arr[3];
                data_arr[3] = data_arr[4];
                data_arr[4] = data;

                media = (data_arr[0] + data_arr[1] + data_arr[2] + data_arr[3] + data_arr[4])/5;
            }

            printf("%f\n", media);

            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}