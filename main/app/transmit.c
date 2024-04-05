/* Copyright 2024 kamilsss655
 * https://github.com/kamilsss655
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "transmit.h"
#include "hardware/audio.h"
#include "hardware/ptt.h"
#include "settings.h"

static const char *TAG = "APP/TRANSMIT";

// Task to transmit morse code message
void TRANSMIT_MorseCode(void *pvParameters)
{
    TRANSMIT_MorseCodeParam_t *param = (TRANSMIT_MorseCodeParam_t *)pvParameters;

    uint16_t dot_duration_ms = 1000 / gSettings.beacon.morse_code.baud;

    AUDIO_TransmitStart();

    PTT_Press();

    ESP_LOGI(TAG, "Transmitting <Morse code>: %s", param->input);
    ESP_LOGI(TAG, "dot_duration_ms: %d", dot_duration_ms);

    for (uint8_t i = 0; i < param->len; i++)
    {
        switch (param->input[i])
        {
        case '.':
            AUDIO_PlayTone(gSettings.beacon.morse_code.tone_freq, dot_duration_ms);
            vTaskDelay(dot_duration_ms / portTICK_PERIOD_MS);
            break;
        case '-':
            AUDIO_PlayTone(gSettings.beacon.morse_code.tone_freq, dot_duration_ms * 3);
            vTaskDelay(dot_duration_ms / portTICK_PERIOD_MS);
            break;
        }
        vTaskDelay((dot_duration_ms * 3) / portTICK_PERIOD_MS);
    }

    PTT_Release();

    AUDIO_TransmitStop();

    // Delete self
    vTaskDelete(NULL);
}