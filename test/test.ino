#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x88, 0x13, 0xbf, 0x69, 0xb7, 0x58};
int count{0};


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  float data[3][16];
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

Adafruit_MPU6050 mpu;


#define NUM_GRAPHS 3
#define NUM_ROWS 8
#define NUM_COLS 16

#define TEST_ROWS 128       // == NUM_ROWS * 16
#define TEST_COLS 16        // == NUM_COLS
#define TEST_INTERVAL 5000

float sineWaves[NUM_GRAPHS][TEST_ROWS][TEST_COLS];
unsigned long lastCallTime = 0;
int currentRow = 0;


void generateSineWave() {
    randomSeed(analogRead(A0)); // Initialize random seed

    for (int i = 0; i < TEST_ROWS; i++) {
        for (int j = 0; j < TEST_COLS; j++) {
            float x = (float)j / TEST_COLS * 2.0 * PI;  // Normalize over 0 to 2π
            // float noise0 = random(-500, 500) / 1000.0;  // Small noise in range [-0.5, 0.5]
            float noise1 = random(-250, 250) / 2500.0;  // Small noise in range [-0.25, 0.25]
            float noise2 = random(-500, 500) / 1000.0;  // Small noise in range [-0.5, 0.5]
            sineWaves[0][i][j] = sin(x + (i * 2 * PI / TEST_ROWS));
            sineWaves[1][i][j] = sin(x + (i * 2 * PI / TEST_ROWS)) + noise1;
            sineWaves[2][i][j] = sin(x + (i * 2 * PI / TEST_ROWS)) + noise2;
        }
    }
}


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  generateSineWave();
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

   Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
}
 
void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // /* Print out the values */
  // Serial.print("Acceleration X: ");
  // Serial.print(a.acceleration.x);
  // Serial.print(", Y: ");
  // Serial.print(a.acceleration.y);
  // Serial.print(", Z: ");
  // Serial.print(a.acceleration.z);
  // Serial.println(" m/s^2");

  // Serial.println("");


  // //Set values to send
  // count+=1;

  // if (count>=15) {
  //   count=0;
  //   // Send message via ESP-NOW
  //   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    
  //   if (result == ESP_OK) {
  //     Serial.println("Sent with success");
  //   }
  //   else {
  //     Serial.println("Error sending the data");
  //   }
  //   delay(500);
  // }




  // unsigned long currentTime = millis();

  myData.data[0][currentRow]=sqrt(pow(a.acceleration.x,2)+pow(a.acceleration.y,2)+pow(a.acceleration.z,2));

  memcpy(&myData.data[1], sineWaves[1][currentRow], sizeof(myData.data[1]));
  memcpy(&myData.data[2], sineWaves[2][currentRow], sizeof(myData.data[2]));
  currentRow+=1;
  if (currentRow>=15) {
      currentRow=0;



      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      delay(200);
  }
}
