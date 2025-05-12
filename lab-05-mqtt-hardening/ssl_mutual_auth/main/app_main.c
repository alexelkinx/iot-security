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
#define WIFI_PASS "F4$rT2bX!j8M@wQz"
#define MQTT_BROKER_URI "mqtts://192.168.1.13:8884"

static const char *TAG_MQTT = "mqtt_tls";
static const char *TAG_WIFI = "wi-fi";
static const char *TAG_SYSTEM = "system";

static esp_mqtt_client_handle_t client;

static bool initial_connection = true; // Flag to track initial connection phase
static bool mqtt_connected = false;    // Track MQTT connection status

// External references to embedded certificates and keys
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t ca_cert_pem_start[] asm("_binary_ca_crt_start");

// === MQTT event handler ===
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG_MQTT, "Preparing to connect to MQTT broker...");
        break;

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "Connected to MQTT broker at URI: %s", MQTT_BROKER_URI);
        mqtt_connected = true; // Set the flag to true

        esp_mqtt_client_subscribe(client, "/topic/qos0", 0); // Subscribe to the topic
        ESP_LOGI(TAG_MQTT, "Subscribed to topic: /topic/qos0");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGE(TAG_MQTT, "Disconnected from MQTT broker, reason: %" PRId32, event_id);
        mqtt_connected = false;          // Set the flag to false
        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait 5 seconds before reconnecting
        esp_mqtt_client_reconnect(client);
        break;

    case MQTT_EVENT_DATA:
        esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
        ESP_LOGI(TAG_MQTT, "Received message. Topic: %.*s, Data: %.*s",
                 event->topic_len, event->topic,
                 event->data_len, event->data);
        break;

    default:
        ESP_LOGW(TAG_MQTT, "Unhandled MQTT event ID: %" PRId32, event_id);
        break;
    }
}

// === MQTT Init ===
static void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .broker.verification.certificate = (const char *)ca_cert_pem_start, // CA certificate
        .credentials = {
            .authentication = {
                .certificate = (const char *)client_cert_pem_start, // Client certificate
                .key = (const char *)client_key_pem_start,          // Client private key
            },
        },
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
        ESP_LOGI(TAG_WIFI, "Attempting to connect to Wi-Fi...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (initial_connection)
        {
            ESP_LOGD(TAG_WIFI, "Not connected yet, retrying...");
        }
        else
        {
            ESP_LOGE(TAG_WIFI, "Disconnected, retrying...");
        }
        esp_wifi_connect(); // Retry Wi-Fi connection
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG_WIFI, "Connected. IP: " IPSTR ", Mask: " IPSTR ", Gateway: " IPSTR,
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
    ESP_LOGI(TAG_SYSTEM, "Initializing NVS...");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG_SYSTEM, "Initializing network interface...");
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(TAG_SYSTEM, "Creating default event loop...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_log_level_set("wifi", ESP_LOG_ERROR);              // Log only Wi-Fi errors
    esp_log_level_set("esp_netif_handlers", ESP_LOG_WARN); // Log only warnings and errors

    wifi_init();

    // Loop for publishing MQTT messages
    int count = 0;
    while (1)
    {
        if (mqtt_connected) // Check if MQTT client is connected
        {
            char buf[10];
            sprintf(buf, "data%d", count);
            esp_mqtt_client_publish(client, "/topic/qos0", buf, 0, 0, 0);
            ESP_LOGI(TAG_MQTT, "Data packet published: %s", buf);
            count = (count + 1) % 10; // Increment and wrap count
        }
        else
        {
            ESP_LOGD(TAG_MQTT, "MQTT client not connected, skipping publish");
        }
        vTaskDelay(pdMS_TO_TICKS(3000)); // Publish every 3 second
    }
}