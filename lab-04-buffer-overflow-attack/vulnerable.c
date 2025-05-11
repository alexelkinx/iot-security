// WARNING: This file is intentionally vulnerable and used for educational purposes only.
// Do not use this code in production.

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"

volatile char adminMode = 0;

void setAdminMode()
{
    adminMode = 1;
}

void vulnerableMQTTHandler(const char *message)
{
    char smallbuffer[48];
    // WARNING: This function is intentionally vulnerable.
    // It uses strcpy without checking the input length, which allows buffer overflow.
    // An attacker can overwrite stack memory, including the return address.
    strcpy(smallbuffer, message);

    // In a normal flow, processing continues here.
    printf("Processed MQTT Message: %s\n", smallbuffer);
    if (strcmp(smallbuffer, "admin") == 0)
    {
        setAdminMode();
    }
    fflush(stdout);
}

// Input that can be modified by the attacker to exploit the overflow
char buffer[80] = "MQTT message";

void app_main(void)

{
    printf("Hello world!\n");
    for (;;)
    {
        vulnerableMQTTHandler(buffer);
        if (adminMode)
        {
            printf("ADMIN MODE!!!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
