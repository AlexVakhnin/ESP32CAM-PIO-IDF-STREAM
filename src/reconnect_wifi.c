#include "reconnect_wifi.h"

int wifi_connect_status = 0;

#define ESP_WIFI_SSID "Alpha3"
#define ESP_WIFI_PASS "asus_"
#define ESP_MAXIMUM_RETRY 5
#define WIFI_CONNECTED_BIT BIT0
//#define WIFI_FAIL_BIT BIT1

//int sta_retry_num = 0;
EventGroupHandle_t sta_wifi_event_group;
EventBits_t bits;

//ОБРАБОТЧИК WIFI
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    // patched up event handler ...

    if (event_base == WIFI_EVENT)
    
    switch (event_id){
        case WIFI_EVENT_STA_START: 
            esp_wifi_connect(); 
            ESP_LOGI("WIFI_STA", "START connecting to AP...");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI("WIFI_STA", "CONNECTED to AP");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(sta_wifi_event_group, WIFI_CONNECTED_BIT);
            //if (sta_retry_num < ESP_MAXIMUM_RETRY) {
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                esp_wifi_connect();
            //    sta_retry_num++;
                ESP_LOGI("WIFI_STA", "DISCONNECTED, trying to reconnect.......");
                
            //} else {
            //    xEventGroupSetBits(sta_wifi_event_group, WIFI_FAIL_BIT);
            //    ESP_LOGI("WIFI_STA", "could not connect to AP");
            //};
            break;
        default:
            ESP_LOGI("WIFI_STA", "unhandled WITI event: ID=%u",event_id);
        } 

        if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI("WIFI_STA", "GOT_IP:" IPSTR, IP2STR(&event->ip_info.ip));
           // sta_retry_num = 0;
            xEventGroupSetBits(sta_wifi_event_group, WIFI_CONNECTED_BIT);
            //xEventGroupClearBits(sta_wifi_event_group, WIFI_FAIL_BIT);
        } 
}

//ИНИЦИАЛИЗАЦИЯ WIFI
esp_err_t wifi_sta_init(void)
{
        //Create an event group
        sta_wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
        assert(sta_netif);

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        //Registration event handling service
        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler,
                                                            NULL, &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler,
                                                            NULL, &instance_got_ip));

        wifi_config_t wifi_config = {
            .sta = {
                .ssid = ESP_WIFI_SSID,
                .password = ESP_WIFI_PASS,
                //Equipment from the AP listener sign
                //.listen_interval = DEFAULT_LISTEN_INTERVAL, //?????????????
                //Setting your password means that the device will connect all security modes, including WEP, WPA. However, these patterns are not recommended, if your access point does not support WPA2, comment to enable other modes
                .threshold.authmode = WIFI_AUTH_WPA2_PSK,

                .pmf_cfg = {
                    .capable = true,
                    .required = false
                },
            },
        };

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
        ESP_ERROR_CHECK(esp_wifi_start() );
        //ESP_LOGI("WIFI_STA", "esp_wifi_set_ps().");
        esp_wifi_set_ps(WIFI_PS_NONE);  //POWER SAVE ?????????????
        ESP_LOGI("WIFI_STA", "wifi_init_sta finished.");

    //ожидаем события от драйвера WIFI


	// While loops keeps event handler alive ********************************************************
        //while(1){
            bits = xEventGroupWaitBits(sta_wifi_event_group, WIFI_CONNECTED_BIT /*| WIFI_FAIL_BIT*/,
                                       pdFALSE, pdFALSE, portMAX_DELAY);

            /*if (bits & WIFI_FAIL_BIT) {
                ESP_LOGI("WIFI_STA", "Failed to connect, restarting station");
                ESP_ERROR_CHECK(esp_wifi_stop());
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                ESP_ERROR_CHECK(esp_wifi_start());
            } else*/ 
            if (bits & WIFI_CONNECTED_BIT) {
                wifi_connect_status = 1;
                ESP_LOGI("WIFI_STA", "SUCCESS: connected to ap SSID:%s password:%s",
                        ESP_WIFI_SSID, ESP_WIFI_PASS);
            //    break; //при успешном коннекте выходим из цикла
          //  };
        
            //vTaskDelay(10000 / portTICK_PERIOD_MS);
            };

        // Should never reach here, unless you terminate the while loop to end event_handling....
        /*
        ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
        ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
        vEventGroupDelete(sta_wifi_event_group);
        */
        return ESP_OK;

}

/*
//RECONNECT
esp_err_t wifi_sta_reconnect(void){

    bits = xEventGroupWaitBits(sta_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                       pdFALSE, pdFALSE, portMAX_DELAY);

        if (bits & WIFI_FAIL_BIT) {
            ESP_LOGI("WIFI_STA", "Failed to connect, restarting station");
            ESP_ERROR_CHECK(esp_wifi_stop());
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP_ERROR_CHECK(esp_wifi_start());
        }


return ESP_OK;
}
*/