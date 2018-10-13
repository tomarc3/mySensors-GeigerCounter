#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(-1);

#define LOG_PERIOD 20000    //logging period in milliseconds
#define MINUTE_PERIOD 60000
#define EVENT_PIN 2         // the interrupt pin listening for Geiger tube events

volatile unsigned long counts = 0; // Geiger tube event counter
unsigned long cpm = 0;             // counts per minute
unsigned long previousMillis;      // time measurement


void ISR_event() { // Captures count of events from Geiger counter board
  counts++;
}


void setup() {
  Serial.begin(115200);

  // init SSD1306 128x64 oled display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  displaySplashScreen();

  // init interrupts to capture tube events
  pinMode(EVENT_PIN, INPUT);                                             // set pin for capturing tube events
  interrupts();                                                          // enable interrupts
  attachInterrupt(digitalPinToInterrupt(EVENT_PIN), ISR_event, FALLING); // define interrupt on falling edge

  Serial.println("setup complete.");
}


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


void displaySplashScreen() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("My Geiger");
  display.println("Counter");
  display.display();

  delay(2000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Radioactivity:");
  display.println("");
  display.println("Measuring ...");
  display.display();
}


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
  }
}
