/*
 * ESP32_Homekit_2_Devices.ino
 *
 * This example shows how to:
 * 1. define a temperature sensor accessory and its characteristics (in my_accessory.c).
 * 2. report the sensor value to HomeKit (just random value here, you need to change it to your real sensor value).
 *
 *  Revised on: 2022-05-16
 *      Author: Cyrus Lau
 *
 * Note:
 *
 * You are recommended to read the Apple's specification doc before using this library.
 * https://developer.apple.com/homekit/specification/
 *
 * This HomeKit library is mostly written in C,
 * you can define your accessory/service/characteristic in a .c file,
 * since the library provides convenient Macro (C only, CPP can not compile) to do this.
 * But it is possible to do this in .cpp or .ino (just not so conveniently), do it yourself if you like.
 * Check out homekit/characteristics.h and use the Macro provided to define your accessory.
 *
 * Generally, the Arduino libraries (e.g. sensors, ws2812) are written in cpp,
 * you can include and use them in a .ino or a .cpp file (but can NOT in .c).
 * A .ino is a .cpp indeed.
 *
 * You can define some variables in a .c file, e.g. int my_value = 1;,
 * and you can access this variable in a .ino or a .cpp by writing extern "C" int my_value;.
 *
 * So, if you want use this HomeKit library and other Arduino Libraries together,
 * 1. define your HomeKit accessory/service/characteristic in a .c file
 * 2. in your .ino, include some Arduino Libraries and you can use them normally
 *                  write extern "C" homekit_characteristic_t xxxx; to access the characteristic defined in your .c file
 *                  write your logic code (eg. read sensors) and
 *                  report your data by writing your_characteristic.value.xxxx_value = some_data; homekit_characteristic_notify(..., ...)
 * done.
 */

// mainProg
  #include <Arduino.h>
  #include <arduino_homekit_server.h>
  #include "wifi_info.h"

  // Report Part
  #define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);
  static uint32_t next_heap_millis = 0;
  static uint32_t next_report_millis = 0;
// mainProg end

// sensors
  // Temperature and Humidity Sensor
    // #include <DHT.h>
    // Include AM2320 library into the sketch
    #include <AM2320.h>
    // Create an instance of sensor
    AM2320 sensor;
    
    //#define DHTPIN 2     // Digital pin connected to the DHT sensor 
    // Uncomment the type of sensor in use:
    //#define DHTTYPE    DHT11     // DHT 11
    //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
    //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
  
    //DHT dht(DHTPIN, DHTTYPE);
    
  // XXX sensor
    // MOTION
    #define PIN_MOTION 5
    // MOTION end
    
    // OBSTACLE
    #define PIN_OBSTACLE 17
    // OBSTACLE end
// sensors end




// accessCFile
  // access your homekit characteristics defined in my_accessory.c
  extern "C" homekit_server_config_t config;
  extern "C" void accessory_init();
  
  // sensors 2
    // TEMPERATURE_SENSOR
    extern "C" homekit_characteristic_t cha_temperature; 
    // TEMPERATURE_SENSOR end
    
    // HUMIDITY_SENSOR
    extern "C" homekit_characteristic_t cha_humidity; 
    // HUMIDITY_SENSOR end
    
    // MOTION_SENSOR
    extern "C" homekit_characteristic_t cha_motion; 
    // MOTION_SENSOR end
    
    // OBSTACLE_SENSOR
    extern "C" homekit_characteristic_t cha_obstacle; 
    // OBSTACLE_SENSOR end

    // LED
    extern "C" bool led_power; 
    // LED end

    // FAN
    extern "C" bool fan_power; 
    extern "C" void fan_update();
    extern "C" homekit_value_t fan_on_get();
    // FAN end
  // sensors 2 end
// accessCFile end





// mainProg 2
void setup() {
  Serial.begin(115200);
  
  //dht.begin();
  sensor.begin(); // call sensor.begin() to initialize the library
  
  wifi_connect(); // wifi_info.h
  
  //homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
  my_homekit_setup();

  // MOTION 2
    pinMode(PIN_MOTION, INPUT);
  // MOTION 2 end

  // OBSTACLE 2
    pinMode(PIN_OBSTACLE, INPUT);
  // OBSTACLE 2 end
}

void loop() {
  my_homekit_loop(); 
  delay(1000);
}
// mainProg 2 end










//==============================
// Homekit setup and loop
//==============================

// mainProg 3
void my_homekit_setup() {
  accessory_init();
  arduino_homekit_setup(&config);
}

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  
  if (t > next_report_millis) {
    // report sensor values every 10 seconds
    next_report_millis = t + 10 * 1000;
    my_homekit_report();
  }
  
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
        ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
  }
}

void my_homekit_report() {  
  // sensor.measure() returns boolean value
  // - true indicates measurement is completed and success
  // - false indicates that either sensor is not ready or crc validation failed
  //   use getErrorCode() to check for cause of error.
  if (sensor.measure()) {
    // sensors
      // TEMPERATURE_SENSOR 3
      //float temperature_value = dht.readTemperature();
      temperature_value = sensor.getTemperature();
      // TEMPERATURE_SENSOR 3 end
      
      // HUMIDITY_SENSOR 3
      //float humidity_value = dht.readHumidity();
      humidity_value = sensor.getHumidity();
      // HUMIDITY_SENSOR 3 end

      if (led_power) {
        if (temperature_value > 30) {
          fan_power = true;  
          fan_on_get();
          fan_update();
        } else {
          fan_power = false;
          fan_on_get();
          fan_update();
        }
      
        // MOTION_SENSOR 3
        int val_digital_motion = digitalRead(PIN_MOTION);
        if (val_digital_motion == HIGH) {
          motion_value = true;
          Serial.println("Motion value TRUE");
        } else {
          motion_value = false;
          Serial.println("Motion value FALSE");
        }
        // MOTION_SENSOR 3 end
    
        // OBSTACLE_SENSOR 3
        int val_digital_obstacle = digitalRead(PIN_OBSTACLE);
        if (val_digital_obstacle == LOW) {
          obstacle_value = true;
          Serial.println("Obstacle value TRUE");
        } else {
          obstacle_value = false;
          Serial.println("Obstacle value FALSE");
        }
        // OBSTACLE_SENSOR 3 end

        Serial.println("Read ABC obstacle: ");
        Serial.print("\t");
        Serial.print(obstacle_value);
        Serial.print("\n");
      }
    // sensors 3 end

    updateRealtimeWebChart(); // wifi_info.h
    insertIntoMySQL(); // wifi_info.h
    
  } else {  // error has occured
    int errorCode = sensor.getErrorCode();
    switch (errorCode) {
      case 1: Serial.println("ERR: Sensor is offline"); break;
      case 2: Serial.println("ERR: CRC validation failed."); break;
    }    
  }

  // sensors 4
    // TEMPERATURE_SENSOR 4
    cha_humidity.value.float_value = humidity_value;
    // TEMPERATURE_SENSOR 4 end
  
    // HUMIDITY_SENSOR 4
    cha_temperature.value.float_value = temperature_value;
    // HUMIDITY_SENSOR 4 end
  
    // MOTION_SENSOR 4
    cha_motion.value.bool_value = motion_value;
    // MOTION_SENSOR 4 end
  
    // OBSTACLE_SENSOR 4
    cha_obstacle.value.bool_value = obstacle_value;
    // OBSTACLE_SENSOR 4 end
  // sensors 4 end

  // sensors 5
    // TEMPERATURE_SENSOR 5
    //LOG_D("Current temperature: %.1f", temperature_value);
    homekit_characteristic_notify(&cha_temperature, cha_temperature.value);
    // TEMPERATURE_SENSOR 5 end
  
    // HUMIDITY_SENSOR 5
    //LOG_D("Current Humidity: %.1f", humidity_value);
    homekit_characteristic_notify(&cha_humidity, cha_humidity.value);
    // HUMIDITY_SENSOR 5 end
  
    // MOTION_SENSOR 5
    homekit_characteristic_notify(&cha_motion, cha_motion.value);
    // MOTION_SENSOR 5 end
  
    // OBSTACLE_SENSOR 5
    homekit_characteristic_notify(&cha_obstacle, cha_obstacle.value);
    // OBSTACLE_SENSOR 5 end
  // sensors 5 end
}
// mainProg 3 end

//==============================
// Homekit setup and loop end
//==============================








int random_value(int min, int max) {
  return min + random(max - min);
}
