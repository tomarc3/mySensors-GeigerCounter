/**
 * Dependencies:
 * - https://github.com/mysensors/MySensors/releases/tag/2.2.0
 * - https://github.com/olikraus/u8g2
 */


/**
 * Wireing
 */

// interrupts
#define EVENT_PIN 3         // the interrupt pin listening for Geiger tube events

// SPI
#define SPI_CE_PIN 9
#define SPI_CSN_PIN 10


/**
 * MySensors library configuration
 */
 
// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached 
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN SPI_CE_PIN
#define MY_RF24_CS_PIN SPI_CSN_PIN

#define MY_DEBUG_VERBOSE_RF24
 
#include <SPI.h>
#include <MySensors.h>  


/**
 * SSD1306 Oled Display
 */
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);


/**
 * Global variables
 */
#define CHILD_ID_CPM 0
MyMessage msg(CHILD_ID_CPM, V_VAR1);


#define LOG_PERIOD 20000    //logging period in milliseconds
#define MINUTE_PERIOD 60000

volatile unsigned long counts = 0; // Geiger tube event counter
unsigned long cpm = 0;             // counts per minute
unsigned long previousMillis;      // time measurement


/**
 * 
 */
void ISR_event() { // Captures count of events from Geiger counter board
  counts++;
}


/**
 * 
 */
void setup() {
  // do nothing
}


/**
 * Present this sensor node to the network
 */
void presentation() { 
  // Send the sketch version information to the gateway
  sendSketchInfo("GeigerCounter", "0.1");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_CPM, S_CUSTOM);
}


/**
 * Before this node starts
 */
void before() {
  Serial.begin(115200);

  // init SSD1306 128x64 oled display
  u8x8.begin();
  u8x8.setPowerSave(0);
  displaySplash();

  // init interrupts to capture tube events
  pinMode(EVENT_PIN, INPUT);                                             // set pin for capturing tube events
  interrupts();                                                          // enable interrupts
  attachInterrupt(digitalPinToInterrupt(EVENT_PIN), ISR_event, FALLING); // define interrupt on falling edge

  Serial.println("setup complete.");
}


/**
 * 
 */
void displayRadioactivity(unsigned long cpm) {
  u8x8.clearLine(2);
  u8x8.setCursor(0,2);
  u8x8.print(cpm);
  u8x8.print(" cpm");
}


/**
 * 
 */
void displaySplash() {
  u8x8.clear();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);
  u8x8.print("Geiger Counter");
  u8x8.setCursor(0,2);
  u8x8.print("Sensor v0.1");

  wait(2000);

  u8x8.clear();
  u8x8.setCursor(0,0);
  u8x8.print("Radioactivity:");
  u8x8.setCursor(0,2);
  u8x8.print("Measuring ...");
}


/**
 * 
 */
void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > LOG_PERIOD) {
    previousMillis = currentMillis;
    cpm = counts * MINUTE_PERIOD / LOG_PERIOD;
    counts = 0;

    Serial.print("Radioacrivity: ");
    Serial.print(cpm);
    Serial.println("cpm");

    displayRadioactivity(cpm);

    send(msg.set(cpm, 1));
  }
}
