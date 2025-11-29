/*
 * -----------------------------------------------------------------------------
 * Course:      BME3323 - Introduction to Microcontroller Programming
 * Assignment:  Biomedical Device Firmware Simulation (STM32F4 Logic)
 * Student:     Emir Dulger (22017751)
 * Date:        November 2025
 * -----------------------------------------------------------------------------
 * Description:
 * This software emulates the control logic of a patient monitoring system.
 * It processes a raw data stream (Heart Rate & SpO2) to control visual indicators.
 *
 * Features:
 * 1. Timer PWM Calculation: Adjusts LED brightness based on Heart Rate.
 * 2. GPIO Bitmasking: Activates alarm patterns on Port D based on SpO2 levels.
 * 3. Time Simulation: Generates accurate timestamps (1 kHz sampling) internally.
 * -----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // Necessary for decimal point standardization

// --- HARDWARE CONFIGURATION ---
#define TIMER_RELOAD_VAL 1000   // Timer Auto-Reload Register (ARR)
#define MAX_HR_LIMIT     200.0  // Maximum reference Heart Rate
#define SOURCE_FILE      "patient_data.csv" 

// --- ALARM STATES (GPIO ODR MASKS) ---
#define STATUS_NORMAL    0x0000 // SpO2 >= 95% (All LEDs OFF)
#define STATUS_WARNING   0x5555 // 90% <= SpO2 < 95% (Even Pins ON)
#define STATUS_CRITICAL  0xAAAA // SpO2 < 90% (Odd Pins ON)
#define STATUS_FAILURE   0xFFFF // SpO2 == 0 (Sensor Error - All ON)

// --- FUNCTION PROTOTYPES ---
int compute_pwm_register(int heart_rate_input);
unsigned int evaluate_alarm_condition(int oxygen_sat, char *output_msg);

int main() {
    // Ensure decimal points are treated correctly ('.' instead of ',')
    setlocale(LC_ALL, "C");

    FILE *data_stream;
    
    // Input Variables (Raw Integers)
    int hr_reading, spo2_reading;
    
    // Internal System Timer & Counter
    double sys_time_sec = 0.0; 
    int packet_counter = 0; 

    // Output Registers
    int ccr_output;
    unsigned int gpio_odr;
    char alarm_desc[60];

    // Initialize File Stream
    data_stream = fopen(SOURCE_FILE, "r");
    
    if (data_stream == NULL) {
        printf("[SYSTEM ERROR] Input file '%s' is missing.\n", SOURCE_FILE);
        printf("Please verify the file location.\n");
        return 1;
    }
    
    // Note: No header skipping needed as the file contains raw data only.

    printf("--- BME3323 FIRMWARE SIMULATION: STARTING ---\n");
    printf("--- Processing Patient Vitals... ---\n\n");
    
    // --- MAIN PROCESSING LOOP ---
    // Reads Heart Rate and SpO2 pairs from CSV
    while (fscanf(data_stream, "%d,%d", &hr_reading, &spo2_reading) == 2) {
        
        // 1. Calculate PWM Duty Cycle for Status LED
        ccr_output = compute_pwm_register(hr_reading);

        // 2. Determine Alarm Status for GPIO Port
        gpio_odr = evaluate_alarm_condition(spo2_reading, alarm_desc);

        // 3. Update Counters
        packet_counter++; 
        
        // --- DISPLAY LOGIC (FILTER) ---
        // To avoid flooding the terminal, we print:
        // a) The very first sample (Time: 0.000)
        // b) Every 1000th sample (Every 1.0 second mark)
        // c) Any sample where an ALARM is active (Warning/Critical/Error)
        
        int is_start = (packet_counter == 1);
        int is_second_mark = (packet_counter % 1000 == 0);
        int is_alarm_active = (gpio_odr != STATUS_NORMAL);

        if (is_start || is_second_mark || is_alarm_active) {
            // System Status Line
            printf("Time: %.3f s | BPM: %d | SpO2: %d\n", sys_time_sec, hr_reading, spo2_reading);
            
            // PWM Debug Line
            float duty_perc = (float)ccr_output / 10.0;
            printf("  -> [PWM] Calculated CCR Value: %d (Duty: %.1f%%)\n", ccr_output, duty_perc);
            
            // GPIO Debug Line (Hexadecimal Output)
            printf("  -> [GPIO] Port D ODR Value: 0x%04X (%s)\n", gpio_odr, alarm_desc);
            
            printf("--------------------------------------------------------------\n");
        }

        // Increment system timer by 1ms (1000 Hz Sampling)
        sys_time_sec += 0.001; 
    }
    
    fclose(data_stream);
    printf("\n>>> Simulation Completed Successfully.\n");
    
    // Keep window open
    printf("Press Enter to exit...");
    getchar(); 
    return 0;
}

// ----------------------------------------------------------------------
// --- DRIVER IMPLEMENTATIONS ---
// ----------------------------------------------------------------------

/*
 * Function: compute_pwm_register
 * Purpose: Maps Heart Rate to Timer Capture Compare Register (CCR) value.
 * Scaling: 0-200 BPM -> 0-1000 CCR
 */
int compute_pwm_register(int heart_rate_input) {
    // Clamp input to valid range
    if (heart_rate_input > (int)MAX_HR_LIMIT) heart_rate_input = (int)MAX_HR_LIMIT;
    if (heart_rate_input < 0) heart_rate_input = 0;
    
    // Apply linear mapping formula
    return (int)((float)heart_rate_input / MAX_HR_LIMIT * TIMER_RELOAD_VAL);
}

/*
 * Function: evaluate_alarm_condition
 * Purpose: Returns the correct GPIO bitmask based on SpO2 levels.
 */
unsigned int evaluate_alarm_condition(int oxygen_sat, char *output_msg) {
    // Case 1: Sensor Disconnected or Error
    if (oxygen_sat == 0) {
        strcpy(output_msg, "SENSOR ERROR");
        return STATUS_FAILURE;
    } 
    // Case 2: Critical Hypoxia (<90%)
    else if (oxygen_sat < 90) {
        strcpy(output_msg, "CRITICAL: Odd Pins ON");
        return STATUS_CRITICAL;
    } 
    // Case 3: Low Oxygen Warning (90-94%)
    else if (oxygen_sat < 95) {
        strcpy(output_msg, "WARNING: Even Pins ON");
        return STATUS_WARNING;
    } 
    // Case 4: Stable Condition
    else {
        strcpy(output_msg, "NORMAL");
        return STATUS_NORMAL;
    }
}