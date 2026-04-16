/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "esp_board_init.h"
#include "speech_commands_action.h"
#include "model_path.h"
#include "esp_process_sdkconfig.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "ssd1306.h"
#include <nvs_flash.h>



int detect_flag = 0;
static esp_afe_sr_iface_t *afe_handle = NULL;
static volatile int task_flag = 0;
srmodel_list_t *models = NULL;
static int play_voice = -2;


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_MASTER_SCL_IO 4
#define I2C_MASTER_SDA_IO 5
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define OLED_ADDR 0x3C

ssd1306_handle_t oled_dev;
















// ESP32-S3 Transmitter Code (ESP-IDF)
#include <esp_wifi.h>
#include <esp_now.h>
#include <string.h>
#include <esp_log.h>

static const char *TAG = "ESP32-S3 Transmitter";
uint8_t receiverAddress[] = {0xFC, 0xF5, 0xC4, 0xAD, 0x1B, 0xD6}; 

typedef struct struct_message {
    char msg[32];
} struct_message;

struct_message dataToSend;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Delivery Status: %s", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void initESPNow() {
    ESP_ERROR_CHECK(nvs_flash_init()); // Initialize NVS

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(onSent));

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));
}





void esp_now_task(void *pvParameter) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000)); 
    }
}

















void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void oled_task(void *pvParameter) {
    oled_dev = ssd1306_create(I2C_MASTER_NUM, OLED_ADDR);
    ssd1306_init(oled_dev);
    ssd1306_clear_screen(oled_dev, 0x00);
    ssd1306_draw_string(oled_dev, 0, 2, "Detecting...", 14, 1);
    ssd1306_refresh_gram(oled_dev);

    vTaskDelay(portMAX_DELAY);
}
void Wake_word_detected(void *Wake) {
    ssd1306_clear_screen(oled_dev, 0x00);
    ssd1306_draw_string(oled_dev, 0, 20, "WakeWord Detected", 14, 1);
    ssd1306_refresh_gram(oled_dev);
}
void detecting(void *detect) {
    ssd1306_clear_screen(oled_dev, 0x00);
    ssd1306_draw_string(oled_dev, 0, 2, "Detecting...", 14, 1);
    ssd1306_refresh_gram(oled_dev);
}

void oled_display_message(const char *message) {
    ssd1306_clear_screen(oled_dev, 0x00);
    ssd1306_draw_string(oled_dev, 0, 2, "COMMAND :", 14, 1);
    ssd1306_draw_string(oled_dev, 0, 32, message, 14, 1);
    ssd1306_refresh_gram(oled_dev);
}


void control_light(int command_id) {
    if (command_id == 2) {
        printf("Light ON\n");
        strcpy(dataToSend.msg, "light on");
        esp_now_send(receiverAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));
        oled_display_message("Light ON");
    } else if (command_id == 3) {
        strcpy(dataToSend.msg, "light off");
        esp_now_send(receiverAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));
        printf("Light OFF\n");
        oled_display_message("Light OFF");
    }
}



void feed_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int nch = afe_handle->get_channel_num(afe_data);
    int feed_channel = esp_get_feed_channel();
    assert(nch <= feed_channel);
    int16_t *i2s_buff = malloc(audio_chunksize * sizeof(int16_t) * feed_channel);
    assert(i2s_buff);

    while (task_flag) {
        esp_get_feed_data(false, i2s_buff, audio_chunksize * sizeof(int16_t) * feed_channel);
        afe_handle->feed(afe_data, i2s_buff);
        vTaskDelay(1 / portTICK_PERIOD_MS); // ADD THIS LINE
    }
    free(i2s_buff);
    vTaskDelete(NULL);
}
void detect_Task(void *arg)
{
    esp_afe_sr_data_t *afe_data = arg;
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data);
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, ESP_MN_ENGLISH);
    printf("multinet:%s\n", mn_name);
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);
    model_iface_data_t *model_data = multinet->create(mn_name, 6000);
    int mu_chunksize = multinet->get_samp_chunksize(model_data);
    esp_mn_commands_update_from_sdkconfig(multinet, model_data);
    assert(mu_chunksize == afe_chunksize);
    multinet->print_active_speech_commands(model_data);

    printf("------------detect start------------\n");
    while (task_flag) {
        afe_fetch_result_t* res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) {
            printf("fetch error!\n");
            break;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            Wake_word_detected(NULL);
            printf("WAKEWORD DETECTED\n");
            multinet->clean(model_data);
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            play_voice = -1;
            detect_flag = 1;
            printf("AFE_FETCH_CHANNEL_VERIFIED, channel index: %d\n", res->trigger_channel_id);
        }

        if (detect_flag == 1) {
            esp_mn_state_t mn_state = multinet->detect(model_data, res->data);

            if (mn_state == ESP_MN_STATE_DETECTING) {
                continue;
            }

            if (mn_state == ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                control_light(mn_result->command_id[0]);
                printf("-----------listening-----------\n");
            }

            if (mn_state == ESP_MN_STATE_TIMEOUT) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data);
                printf("timeout\n");
                detecting(NULL);
                afe_handle->enable_wakenet(afe_data);
                detect_flag = 0;
                printf("\n-----------awaits to be waken up-----------\n");
                continue;
            }
        }
    }
    if (model_data) {
        multinet->destroy(model_data);
        model_data = NULL;
    }
    printf("detect exit\n");
    vTaskDelete(NULL);
}

void app_main()
{
    initESPNow();

    xTaskCreate(esp_now_task, "esp_now_task", 4096, NULL, 5, NULL); 


    i2c_master_init();
    xTaskCreate(oled_task, "oled_task", 4096, NULL, 5, NULL);

    models = esp_srmodel_init("model");
    ESP_ERROR_CHECK(esp_board_init(AUDIO_HAL_16K_SAMPLES, 1, 16));
    #if CONFIG_IDF_TARGET_ESP32
        printf("This demo only supports ESP32S3\n");
        return;
    #else
        afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    #endif

    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    #if defined CONFIG_ESP32_S3_BOX_BOARD || defined CONFIG_ESP32_S3_EYE_BOARD || CONFIG_ESP32_S3_DEVKIT_C
        afe_config.aec_init = false;
        #if defined CONFIG_ESP32_S3_EYE_BOARD || CONFIG_ESP32_S3_DEVKIT_C
            afe_config.pcm_config.total_ch_num = 2;
            afe_config.pcm_config.mic_num = 1;
            afe_config.pcm_config.ref_num = 1;
        #endif
    #endif
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);

    task_flag = 1;
    xTaskCreatePinnedToCore(&detect_Task, "detect", 8 * 1024, (void*)afe_data, 15, NULL, 1);
    xTaskCreatePinnedToCore(&feed_Task, "feed", 8 * 1024, (void*)afe_data, 15, NULL, 0);
    
}
