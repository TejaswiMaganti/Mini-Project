#include <LiquidCrystal_I2C.h>

// Define pins
int IRIN = 7;    // Infrared Input (Entering)
int IROUT = 2;   // Infrared Output (Exiting)
int BUZ = 8;     // Buzzer

// Variables for counting
int Total = 0;
bool IRInTriggered = false;  // Tracks if IRIN has been triggered
bool IROutTriggered = false; // Tracks if IROUT has been triggered

// Initialize the LCD (address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    
    // Initialize the pins
    pinMode(IRIN, INPUT);
    pinMode(IROUT, INPUT);
    pinMode(BUZ, OUTPUT);
    
    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    
    // Print initial message
    lcd.setCursor(0, 0);
    lcd.print("Total Count: ");
    lcd.setCursor(0, 1);
    lcd.print(Total);
}

void loop() {
    // Read sensor states
    int irInState = digitalRead(IRIN);
    int irOutState = digitalRead(IROUT);
    
    Serial.print("IRIN: ");
    Serial.print(irInState);
    Serial.print(" / IROUT: ");
    Serial.print(irOutState);
    
    // Check for the entering case (IRIN -> IROUT)
    if (irInState == 0 && !IRInTriggered) { // First sensor triggered (IRIN)
        IRInTriggered = true;  // Set the flag to indicate IRIN was triggered
    }

    if (irOutState == 0 && IRInTriggered) { // Second sensor triggered (IROUT)
        // Valid entering sequence (IRIN -> IROUT)
        Total++;
        IRInTriggered = false; // Reset the flag after a valid entry
        Serial.println(" -> Person entered");
    }

    // Check for the exiting case (IROUT -> IRIN)
    if (irOutState == 0 && !IROutTriggered) { // First sensor triggered (IROUT)
        IROutTriggered = true;  // Set the flag to indicate IROUT was triggered
    }

    if (irInState == 0 && IROutTriggered) { // Second sensor triggered (IRIN)
        // Valid exiting sequence (IROUT -> IRIN)
        if (Total > 0) {
            Total--;
        }
        IROutTriggered = false; // Reset the flag after a valid exit
        Serial.println(" -> Person exited");
    }

    // Reset triggers if sensors are not triggered anymore (both sensors return HIGH)
    if (irInState == 1) {
        IRInTriggered = false; // Reset the flag when the sensor is no longer triggered
    }

    if (irOutState == 1) {
        IROutTriggered = false; // Reset the flag when the sensor is no longer triggered
    }

    // Display the total count or overload message
    lcd.clear();
    if (Total <= 40) {
        lcd.setCursor(0, 0);
        lcd.print("Total Count: ");
        lcd.setCursor(0, 1);
        lcd.print(Total);
    } else {
        // Overload condition
        lcd.setCursor(0, 0);
        lcd.print("Bus is overloaded");
        
        // Trigger the buzzer
        digitalWrite(BUZ, HIGH);
        delay(1000); // Sound buzzer for 1 second
        digitalWrite(BUZ, LOW);
        
        // Reset the count to zero after buzzer
        Total = 0;
        
        // Display resetting message
        lcd.setCursor(0, 1);
        lcd.print("Resetting...");
        delay(1000); // Hold message for 1 second
    }

    Serial.print(" -> ");
    Serial.println(Total);
    
    delay(500); // Delay to avoid too rapid counting
}
