/*
 * my_accessory.c
 * Define the accessory in C language using the Macro in characteristics.h
 *
 *  Revised on: 2022-05-16
 *      Author: Cyrus Lau
 */






/*
 * 
 * 
 * If setting the content related to the Home App, please refer to characteristics.h in Arduino-HomeKit-ESP32-library and HAP-Specification-Non-Commercial-Version.pdf by Apple
 *  
 *  
 */






// mainProg 4
  #include <analogWrite.h>
  #include <Arduino.h>
  #include <homekit/types.h>
  #include <homekit/homekit.h>
  #include <homekit/characteristics.h>
  #include <stdio.h>
  #include <port.h>
// mainProg 4 end






// sensors 6
  // *If the sensor does not using I2C
  
    // LIGHTBULB
    #define PIN_LED  13
    //#define LED_RANGE 255
    //int led_bri = 100; //[0, 100]
    bool led_power = false; //true or flase
    // LIGHTBULB end
    
    // FAN
    int PIN_INA = 19; 
    int PIN_INB = 18;
    #define FAN_SPEED_RANGE 255
    float fan_speed = 100; //[0, 100]
    bool fan_power = false; //true or flase
    // FAN end
    
//    // MOTION
//    #define PIN_MOTION 5
//    // MOTION end
//    
//    // OBSTACLE
//    #define PIN_OBSTACLE 17
//    // OBSTACLE end
// sensors 6 end










// homekit
  // *All sensors
  
    // LIGHTBULB 6
      void led_update() {
        if (led_power) {
//          int val = (int) (led_bri * 1.0 * LED_RANGE / 100.0 + 0.5f);
          analogWrite(PIN_LED, 255);
//          printf("LED ON (pwm: %d of %2d)\n", led_bri, val);
        } else {
          analogWrite(PIN_LED, 0);
          printf("LED OFF\n");
        }
      }
      //// LIGHTBULB 6.1
        //homekit_value_t light_bri_get() {
        //  return HOMEKIT_INT(led_bri);
        //}
        //void led_bri_set(homekit_value_t value) {
        //  if (value.format != homekit_format_int) {
        //    return;
        //  }
        //  led_bri = value.int_value;
        //  led_update();
        //}
      //// LIGHTBULB 6.1 end
      //// LIGHTBULB 6.2
        homekit_value_t led_on_get() {
          return HOMEKIT_BOOL(led_power);
        }
        void led_on_set(homekit_value_t value) {
         if (value.format != homekit_format_bool) {
            printf("Invalid on-value format: %d\n", value.format);
            return;
          }
          led_power = value.bool_value;
          if (led_power) {
//            if (led_bri < 1) {
//              led_bri = 100;
//            }
          }
          led_update();
        }
      // LIGHTBULB 6.2 end
    // LIGHTBULB 6 end
    
    // TEMPERATURE_SENSOR
      // For TEMPERATURE_SENSOR,
      // the required characteristics are: CURRENT_TEMPERATURE
      // the optional characteristics are: NAME, STATUS_ACTIVE, STATUS_FAULT, STATUS_TAMPERED, STATUS_LOW_BATTERY
      // See HAP section 8.41 and characteristics.h
      // (required) format: float; HAP section 9.35; min 0, max 100, step 0.1, unit celsius
      homekit_characteristic_t cha_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);
    // TEMPERATURE_SENSOR end
    
    // HUMIDITY_SENSOR
      homekit_characteristic_t cha_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);
    // HUMIDITY_SENSOR end
    
    // FAN 6
      void fan_update() {
        if (fan_power) {
          // fan_speed 0-100
          // FAN_SPEED_RANGE 255
          int val = (int) (fan_speed * 1.0 * (FAN_SPEED_RANGE - 80) / 100.0 + 80.5f);
//          int val = (int) (fan_speed * 1.0 * FAN_SPEED_RANGE / 100.0 + 0.5f);
          analogWrite(PIN_INB, 0);
          analogWrite(PIN_INA, 255);
          analogWrite(PIN_INA, val);
          printf("FAN ON (Speed: %d of %2d)\n", fan_speed, val);
        } else {
          analogWrite(PIN_INA, 0);
          analogWrite(PIN_INB, 0);
          printf("FAN OFF\n");
        }
      }
      
      // FAN 6.1
        homekit_value_t fan_speed_get() {
          return HOMEKIT_INT(fan_speed);
        }
        void fan_speed_set(homekit_value_t value) {
          if (value.format != homekit_format_int) {
            return;
          }
          fan_speed = value.int_value;
          fan_update();
        }
      // FAN 6.1 end
      
      // FAN 6.2
        homekit_value_t fan_on_get() {
          return HOMEKIT_BOOL(fan_power);
        }
        void fan_on_set(homekit_value_t value) {
         if (value.format != homekit_format_bool) {
            printf("Invalid on-value format: %d\n", value.format);
            return;
          }
          fan_power = value.bool_value;
          if (fan_power == 1) {
            if (fan_speed < 1) {
              fan_speed = 100;
            }
          }
          fan_update();
        }
      // FAN 6.2 end
    // FAN 6 end
    
    // MOTION 3
      homekit_characteristic_t cha_motion = HOMEKIT_CHARACTERISTIC_(MOTION_DETECTED, false);
    // MOTION 3 end
    
    // OBSTACLE 3
      homekit_characteristic_t cha_obstacle = HOMEKIT_CHARACTERISTIC_(MOTION_DETECTED, false);
    // OBSTACLE 3 end
// homekit end







// sensors 7
  // *If the sensor does not using I2C
    void accessory_init() { // Pin Setting
      // LIGHTBULB 2
        pinMode(PIN_LED, OUTPUT);
        led_update();
      // LIGHTBULB 2 end
    
      // FAN 2
        pinMode(PIN_INA, OUTPUT);
        pinMode(PIN_INB, OUTPUT);
        fan_update();
      // FAN 2 end
    
//      // MOTION 2
//        pinMode(PIN_MOTION, INPUT);
//      // MOTION 2 end
//    
//      // OBSTACLE 2
//        pinMode(PIN_OBSTACLE, INPUT);
//      // OBSTACLE 2 end
    }
// sensors 7 end







// homekit 2
  // *If the sensor does not using I2C or digitalRead()
    // Init Reset
    // Generally this is called when paired successfully or click the "Identify Accessory" button in Home APP.
    void my_accessory_identify(homekit_value_t _value) {
      printf("accessory identify\n");
      
      for (int j = 0; j < 3; j++) { 
        // Turn on all but passive sensors
          // LIGHTBULB 3
            led_power = true;
            led_update();
          // LIGHTBULB 3 end
      
          // FAN 3
            fan_power = true;
            fan_update();
          // FAN 3 end
          
          delay(100);

        // Turn off all but passive sensors
          // LIGHTBULB 4
            led_power = false;
            led_update();
          // LIGHTBULB 4 end
      
          // FAN 4
            fan_power = false;
            fan_update();
          // FAN 4 end
          
          delay(100);
      }
    }
// homekit 2 end







// homekit 3
  // Access your Home App
  homekit_accessory_t *accessories[] = {
    
      // Accessory 1
      HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_sensor, .services=(homekit_service_t*[]) {
          // Accessory Configuration
          HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
              HOMEKIT_CHARACTERISTIC(NAME, "Smart Chair"),
              HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
              HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
              HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/ESP32"),
              HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
              HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
              NULL
          }),
          // homekit 4
            // Sensor(s) :
              // TEMPERATURE_SENSOR 2
                HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
                    HOMEKIT_CHARACTERISTIC(NAME, "Temperature"),//optional
                    &cha_temperature,
                    NULL
                }),
              // TEMPERATURE_SENSOR 2 end
              
              // HUMIDITY_SENSOR 2
                HOMEKIT_SERVICE(HUMIDITY_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
                    HOMEKIT_CHARACTERISTIC(NAME, "Humidity"),
                    &cha_humidity,
                    NULL
                }),
              // HUMIDITY_SENSOR 2 end
      
              // MOTION 4
                HOMEKIT_SERVICE(MOTION_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
                    HOMEKIT_CHARACTERISTIC(NAME, "Seat"),
                    &cha_motion,
                    NULL
                }),
              // MOTION 4 end
              
              // OBSTACLE 4
                HOMEKIT_SERVICE(MOTION_SENSOR, .characteristics=(homekit_characteristic_t*[]) {
                    HOMEKIT_CHARACTERISTIC(NAME, "Back"),
                    &cha_obstacle,
                    NULL
                }),
              // OBSTACLE 4 end
          // homekit 4 end
          NULL
      }),
      
      // Accessory 2
      HOMEKIT_ACCESSORY(.id=2, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
          // Accessory Configuration
          HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
              HOMEKIT_CHARACTERISTIC(NAME, "Posture Check"),
              HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
              HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
              HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/ESP32"),
              HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
              HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
              NULL
          }),
            // homekit 4
              // Sensor(s) :       
                // LIGHTBULB 5
                    HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
                        HOMEKIT_CHARACTERISTIC(NAME, "Posture Check"),
                        //HOMEKIT_CHARACTERISTIC(BRIGHTNESS, 100, .getter=light_bri_get, .setter=led_bri_set), // LIGHTBULB 6.1
                        HOMEKIT_CHARACTERISTIC(ON, false, .getter=led_on_get, .setter=led_on_set),  // LIGHTBULB 6.2
                        NULL
                    }),
                // LIGHTBULB 5 end
          // homekit 4 end
          NULL
      }),

      // Accessory 3
      HOMEKIT_ACCESSORY(.id=3, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]) {
          // Accessory Configuration
          HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
              HOMEKIT_CHARACTERISTIC(NAME, "Fan"),
              HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
              HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
              HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/ESP32"),
              HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
              HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
              NULL
          }),
            // homekit 4
              // Sensor(s) :
                // FAN 5
                HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
                    HOMEKIT_CHARACTERISTIC(NAME, "Seat Fan"),
                    HOMEKIT_CHARACTERISTIC(BRIGHTNESS, 100, .getter=fan_speed_get, .setter=fan_speed_set), // FAN 6.1
                    HOMEKIT_CHARACTERISTIC(ON, false, .getter=fan_on_get, .setter=fan_on_set), // FAN 6.2
                    NULL
                }),
                // FAN 5 end
          // homekit 4 end
          NULL
      }),
  
      NULL
  };
  
  homekit_server_config_t config = {
      .accessories = accessories,
      .password = "111-11-111"
  };
// homekit 3 end
