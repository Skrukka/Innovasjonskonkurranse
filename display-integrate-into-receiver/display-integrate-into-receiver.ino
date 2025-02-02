#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_DC 2
#define TFT_CS 15
#define TFT_MOSI 4
#define TFT_CLK 16
#define TFT_RST 0
#define TFT_MISO 0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

typedef struct struct_message {
    float distances[16];
} struct_message;

struct_message myData;

#define NUM_ROWS 8
#define NUM_COLS 16
float distanceData[NUM_ROWS][NUM_COLS];





#define TEST_ROWS 128
#define TEST_COLS 16
#define TEST_INTERVAL 1000  // 1 second

float sineWave[TEST_ROWS][TEST_COLS]; // 2D array with 128x16 32-bit floats
unsigned long lastCallTime = 0;
int currentRow = 0;





void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    shiftAndStoreData(myData.distances);
    drawGraph();
}

void shiftAndStoreData(float newDistances[NUM_COLS]) {
    for (int i = 0; i < NUM_ROWS - 1; i++) {
        memcpy(distanceData[i], distanceData[i + 1], NUM_COLS * sizeof(float));
    }
    memcpy(distanceData[NUM_ROWS - 1], newDistances, NUM_COLS * sizeof(float));
}

void drawGraph() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(100, 5);
    tft.print("Graph");
    
    int graphHeight = 160;
    int baseY = 159;
    
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS - 1; col++) {
            int x1 = row * NUM_COLS + col;
            int y1 = baseY - (mapDistance(distanceData[row][col]) / 400.0) * graphHeight;
            int x2 = row * NUM_COLS + col + 1;
            int y2 = baseY - (mapDistance(distanceData[row][col + 1]) / 400.0) * graphHeight;
            tft.drawLine(x1, y1, x2, y2, ST77XX_GREEN);
        }
    }
}

float mapDistance(float value) {
    return 2.0 + (value + 1.0) * (400.0 - 2.0) / 2.0;
}








void generateSineWave() {
    randomSeed(analogRead(A0)); // Initialize random seed

    for (int i = 0; i < TEST_ROWS; i++) {
        for (int j = 0; j < TEST_COLS; j++) {
            float x = (float)j / TEST_COLS * 2.0 * PI;  // Normalize over 0 to 2π
            float noise = random(-500, 500) / 1000.0;  // Small noise in range [-0.5, 0.5]
            sineWave[i][j] = sin(x + (i * 2 * PI / TEST_ROWS)) + noise;
        }
    }
}













void setup() {
    Serial.begin(115200);

    generateSineWave();

    // WiFi.mode(WIFI_STA);
    // if (esp_now_init() != ESP_OK) {
    //     Serial.println("Error initializing ESP-NOW");
    //     return;
    // }
    // esp_now_register_recv_cb(OnDataRecv);

    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST77XX_BLACK);
}



void loop() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastCallTime >= TEST_INTERVAL) {
        lastCallTime = currentTime;
        
        memcpy(&distanceData, sineWave[currentRow], sizeof(distanceData));
        drawGraph();

        // Move to the next row, looping back after 128
        currentRow = (currentRow + 1) % TEST_ROWS;
    }
}
