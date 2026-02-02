#include <mega16.h>       
#include <delay.h>        
#include <lcd.h> 
#include <stdio.h>        

#asm
  .equ __lcd_port = 0x15  // Define LCD port as PORTC
#endasm

#define TRIG PORTD.0      // Trigger pin on PD0
#define ECHO PIND.1       // Echo pin on PD1

// Function to send a trigger pulse to HC-SR04
void send_trigger() {
    TRIG = 1;             
    delay_us(10);         
    TRIG = 0;             
}

// Function to measure pulse width using Timer1 (with timeout)
unsigned int measure_pulse() {
    unsigned int count = 0;
    
    TCNT1 = 0;              // Reset Timer1 counter
    TCCR1B = (1 << CS11);   // Start Timer1 with prescaler 8 (1 tick = 0.5µs for 8MHz clock)

    while (ECHO == 0);  // Wait for the echo pin to go HIGH
    TCNT1 = 0;          // Reset Timer again to start counting from 0

    while (ECHO == 1 && count < 60000) {  // Wait for echo pin to go LOW (timeout to prevent infinite loop)
        count = TCNT1;
    }

    TCCR1B = 0;         // Stop Timer1

    return count;       // Return Timer1 value (represents pulse width in 0.5µs steps)
}

// Function to get a highly stable velocity reading
float get_velocity() {
    unsigned int time;
    float velocity, sum = 0;
    int valid_readings = 0;
    int i;
    
    for (i = 0; i < 200; i++) {  // Take 200 readings for ultra stability
        send_trigger();
        time = measure_pulse();
        
        if (time > 200 && time < 60000) { // Ignore extreme readings (noise filtering)
            sum += (2 * 100) / (time * 0.5 * 0.0001); // Velocity in m/s
            valid_readings++;
        }
        
        delay_ms(2); // Small delay between readings
    }
    
    if (valid_readings > 0) {
        velocity = sum / valid_readings; // Average only valid readings
    } else {
        velocity = 0;  // In case no valid readings were obtained
    }
    
    return velocity;
}

// Function to apply moving average filter for smoother temperature values
float smooth_value(float new_value, float old_value) {
    return (0.9 * old_value) + (0.1 * new_value); // 90% old, 10% new
}

// Function to calculate temperature with improved stability
float get_temperature(float velocity, float previous_temp) {
    float new_temp = ((velocity - 331.4) / 0.606);
    return smooth_value(new_temp, previous_temp); // Apply moving average filter
}

// Function to convert float to string with 2 decimal places
void float_to_string(float num, char *str) {
    int whole_part = (int) num;  
    int decimal_part = (int)((num - whole_part) * 100);  // Extract 2 decimal places
    sprintf(str, "%d.%02d", whole_part, decimal_part);  // Format as XX.XX
}

void main() {
    float velocity = 340.0, temperature = 25.0; // Start with an initial stable value
    char buffer[16];

    DDRD |= (1 << PIND0);  // Set PD0 as output (Trigger)
    DDRD &= ~(1 << PIND1); // Set PD1 as input (Echo)

    lcd_init(16);          // Initialize LCD in 16x2 mode
    lcd_clear();

    while(1) {
        float raw_velocity = get_velocity();
        velocity = smooth_value(raw_velocity, velocity); // Apply moving average
        temperature = get_temperature(velocity, temperature); // Stabilized temperature
        
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts("Vel: ");
        float_to_string(velocity, buffer);
        lcd_puts(buffer);
        lcd_puts(" m/s");
        
        lcd_gotoxy(0, 1);
        lcd_puts("Temp: ");
        float_to_string(temperature, buffer);
        lcd_puts(buffer);
        lcd_puts(" C");
        
        delay_ms(500); // Increased delay for better stabilization
    }
}
