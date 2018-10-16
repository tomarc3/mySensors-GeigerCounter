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
//#include <Adafruit_SSD1306.h>

//Adafruit_SSD1306 display(4);


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







void ISR_event() { // Captures count of events from Geiger counter board
  counts++;
}

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
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  // displaySplash();

  // init interrupts to capture tube events
  pinMode(EVENT_PIN, INPUT);                                             // set pin for capturing tube events
  interrupts();                                                          // enable interrupts
  attachInterrupt(digitalPinToInterrupt(EVENT_PIN), ISR_event, FALLING); // define interrupt on falling edge

  Serial.println("setup complete.");
}

/*
void displayRadioactivity(unsigned long cpm) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Radioactivity:");
  display.println("");
  display.setTextSize(2);
  display.print(cpm); display.println(" cpm");
  display.display();
}


void displaySplash() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("My Geiger");
  display.println("Counter");
  display.display();

  wait(2000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Radioactivity:");
  display.println("");
  display.println("Measuring ...");
  display.display();
}
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

    // displayRadioactivity(cpm);

    send(msg.set(cpm, 1));
  }
}
