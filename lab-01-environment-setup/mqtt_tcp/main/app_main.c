#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "nvs_flash.h"

#define WIFI_SSID "RASPI-ALEX"
#define WIFI_PASS "11223344"
#define MQTT_BROKER_URI "mqtt://192.168.1.128" // docker exec -it mosquitto ip a

static const char *TAG = "mqtt_app";
static esp_mqtt_client_handle_t client;

static bool initial_connection = true; // Flag to track initial connection phase
static bool mqtt_connected = false;    // Track MQTT connection status

// === MQTT event handler ===
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "Preparing to connect to MQTT broker...");
        break;

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to MQTT broker");
        mqtt_connected = true; // Set the flag to true
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGE(TAG, "Disconnected from MQTT broker, retrying...");
        mqtt_connected = false; // Set the flag to false
        esp_mqtt_client_reconnect(client);
        break;

    default:
        ESP_LOGW(TAG, "Unhandled MQTT event ID: %" PRId32, event_id);
        break;
    }
}

// === MQTT Init ===
static void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

// === Wi-Fi event handler ===
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Attempting to connect to Wi-Fi...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (initial_connection)
        {
            ESP_LOGW(TAG, "Wi-Fi not connected yet, retrying...");
        }
        else
        {
            ESP_LOGE(TAG, "Wi-Fi disconnected, retrying...");
        }
        esp_wifi_connect(); // Retry Wi-Fi connection
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Wi-Fi connected. IP: " IPSTR ", Mask: " IPSTR ", Gateway: " IPSTR,
                 IP2STR(&event->ip_info.ip),
                 IP2STR(&event->ip_info.netmask),
                 IP2STR(&event->ip_info.gw));
        initial_connection = false; // Clear the initial connection flag
        mqtt_init();                // Initialize MQTT after getting IP
    }
}

// === Wi-Fi Init ===
static void wifi_init(void)
{
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// === Main ===
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_log_level_set("wifi", ESP_LOG_ERROR);              // Log only Wi-Fi errors
    esp_log_level_set("esp_netif_handlers", ESP_LOG_WARN); // Log only warnings and errors

    wifi_init();

    // Loop for publishing MQTT messages
    while (1)
    {
        if (mqtt_connected) // Check if MQTT client is connected
        {
            esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "Data packet published");
        }
        else
        {
            ESP_LOGW(TAG, "MQTT client not connected, skipping publish");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Publish every 1 second
    }
}