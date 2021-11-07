#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
 
// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN 20  // you can change this to whatever you like

/* 
 * This include brings in static arrays which contain audio samples. 
 * if you want to know how to make these please see the python code
 * for converting audio samples into static arrays. 
 */
#include "sounddata.h"
#include "message.h"
char isPlaying = 0;
int wav_position = 0;
int WAV_DATA_LENGTH = 21482;
uint8_t *WAV_DATA;
/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (WAV_DATA_LENGTH<<3) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 
        pwm_set_gpio_level(AUDIO_PIN, WAV_DATA[wav_position>>3]);  
        wav_position++;
    } else {
        // reset to start
        pwm_set_gpio_level(AUDIO_PIN, 0);
        isPlaying = 0;
        wav_position = 0;
    }
}

void choose_sample(int i)
{

    wav_position = 0;
    switch(i){
        case -1:
            WAV_DATA_LENGTH = len_s;
            WAV_DATA = samp_s;
            break;

        case 0:
            WAV_DATA_LENGTH = len_0;
            WAV_DATA = samp_0;
            break;

        case 1:
            WAV_DATA_LENGTH = len_1;
            WAV_DATA = samp_1;
            break;

        case 2:
            WAV_DATA_LENGTH = len_2;
            WAV_DATA = samp_2;
            break;

        case 3:
            WAV_DATA_LENGTH = len_3;
            WAV_DATA = samp_3;
            break;

        case 4:
            WAV_DATA_LENGTH = len_4;
            WAV_DATA = samp_4;
            break;

        case 5:
            WAV_DATA_LENGTH = len_5;
            WAV_DATA = samp_5;
            break;

        case 6:
            WAV_DATA_LENGTH = len_6;
            WAV_DATA = samp_6;
            break;

        case 7:
            WAV_DATA_LENGTH = len_7;
            WAV_DATA = samp_7;
            break;

        case 8:
            WAV_DATA_LENGTH = len_8;
            WAV_DATA = samp_8;
            break;

        case 9:
            WAV_DATA_LENGTH = len_9;
            WAV_DATA = samp_9;
            break;

        case 10:
            WAV_DATA_LENGTH = len_10;
            WAV_DATA = samp_10;
            break;

        case 11:
            WAV_DATA_LENGTH = len_11;
            WAV_DATA = samp_11;
            break;

        case 12:
            WAV_DATA_LENGTH = len_12;
            WAV_DATA = samp_12;
            break;

        case 13:
            WAV_DATA_LENGTH = len_13;
            WAV_DATA = samp_13;
            break;

        case 14:
            WAV_DATA_LENGTH = len_14;
            WAV_DATA = samp_14;
            break;

        case 15:
            WAV_DATA_LENGTH = len_15;
            WAV_DATA = samp_15;
            break;

        case 16:
            WAV_DATA_LENGTH = len_16;
            WAV_DATA = samp_16;
            break;

        case 17:
            WAV_DATA_LENGTH = len_17;
            WAV_DATA = samp_17;
            break;

        case 18:
            WAV_DATA_LENGTH = len_18;
            WAV_DATA = samp_18;
            break;

        case 19:
            WAV_DATA_LENGTH = len_19;
            WAV_DATA = samp_19;
            break;

        case 20:
            WAV_DATA_LENGTH = len_20;
            WAV_DATA = samp_20;
            break;

        case 21:
            WAV_DATA_LENGTH = len_21;
            WAV_DATA = samp_21;
            break;

        case 22:
            WAV_DATA_LENGTH = len_22;
            WAV_DATA = samp_22;
            break;

        case 23:
            WAV_DATA_LENGTH = len_23;
            WAV_DATA = samp_23;
            break;

        case 24:
            WAV_DATA_LENGTH = len_24;
            WAV_DATA = samp_24;
            break;

        case 25:
            WAV_DATA_LENGTH = len_25;
            WAV_DATA = samp_25;
            break;

        case 26:
            WAV_DATA_LENGTH = len_e;
            WAV_DATA = samp_e;
            break;
    }
}

void playSample(int n)
{
    choose_sample(n);
    printf("Playing %d sample\n", n);

    isPlaying = 1;
    while(isPlaying == 1) {
        __wfi(); // Wait for Interrupt
    }
}

int main(void) {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
    stdio_init_all();
    set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);


    while(true){
        for(int i = 0; i < msg1_len; i++)
        {
            playSample((int)message1[i]);
        }
    }
}
