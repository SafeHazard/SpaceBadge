#include "BAT_Driver.h"
#include "esp_adc_cal.h"

#define BAT_ADC_PIN 8
#define DEFAULT_VREF 1100  // ESP32 default reference voltage in mV
#define MEASUREMENT_OFFSET 0.990476
#define VOLTAGE_DIVIDER_RATIO 3.0
#define NUM_SAMPLES 10  // Adjust for more/less smoothing

static float voltage_samples[NUM_SAMPLES] = { 0 };
static int sample_index = 0;
esp_adc_cal_characteristics_t adc_chars;

float BAT_analogVolts = 0;

void BAT_Init()
{
    analogReadResolution(12);  // 12-bit ADC (0-4095)
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
}

float BAT_Get_Volts()
{
    int raw = analogRead(BAT_ADC_PIN);
    uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, &adc_chars);
    float new_voltage = (voltage_mv * VOLTAGE_DIVIDER_RATIO / 1000.0) / MEASUREMENT_OFFSET;

    // Store the new voltage sample
    voltage_samples[sample_index] = new_voltage;
    sample_index = (sample_index + 1) % NUM_SAMPLES;  // Circular buffer index

    // Compute moving average
    float sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += voltage_samples[i];
    }
    BAT_analogVolts = sum / NUM_SAMPLES;

    return BAT_analogVolts;
}


int BAT_Get_Percentage(float voltage)
{
    static const struct
    {
        int percent;
        float voltage;
    } battery_table[] = {
        {100, 4.15}, {90, 4.06}, {80, 3.97}, {70, 3.88},
        {60, 3.79}, {50, 3.7}, {40, 3.61}, {30, 3.52},
        {20, 3.43}, {10, 3.34}, {0, 3.25}
    };

    for (size_t i = 0; i < sizeof(battery_table) / sizeof(battery_table[0]); i++)
    {
        if (voltage >= battery_table[i].voltage)
        {
            return battery_table[i].percent;
        }
    }
    return 0;  // Default to 0% if below the lowest threshold
}
