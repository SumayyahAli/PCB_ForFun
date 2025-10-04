
#include <TFT_eSPI.h>
#include "DHTesp.h"

// ----- USER PINS --------------------------------------------------
#define DHT_PIN    34          // DHT11 DATA
#define BTN_BUSY   32          
#define BTN_FREE   33          

// ----- CONSTANTS --------------------------------------------------
const uint32_t REFRESH_MS  = 2000;   // DHT poll period
const uint32_t DEBOUNCE_MS = 50;  //100; // button debounce #####I'll replace it with HW debouncing RC

// ----- GLOBALS ----------------------------------------------------
TFT_eSPI tft;
DHTesp   dht;
bool     busy = false;                 // current status -->State Machine 
uint32_t lastTemp = 0, lastBtn = 0; //debouncing 

// ---------- paint whole screen with status colour -----------------
void drawStatus(bool b)
{
  uint16_t bg   = b ? TFT_MAROON : TFT_OLIVE;
  const char* s = b ? "  BUSY"  : "AVAILABLE";

  tft.fillScreen(bg);
  tft.setTextColor(TFT_WHITE, bg);
  tft.setCursor(100, 120, 4);           
  tft.print(s);
}

// ------------------------------------------------------------------
void drawReadings(float t, float h)
{
  // overwrite just the numbers area so status background remains
  tft.fillRect(10, 10, 220, 55, busy ? TFT_MAROON : TFT_OLIVE);

  tft.setCursor(10, 10, 4); 
  tft.setTextColor(TFT_ORANGE, busy ? TFT_MAROON : TFT_OLIVE);
  tft.printf("T: %.1f C", t);
  tft.setCursor(10, 40, 4); 
  tft.setTextColor(0xBF5D, busy ? TFT_MAROON : TFT_OLIVE);
  tft.printf("H: %.1f %%", h);
    tft.setCursor(30, 210, 2); 
    tft.setTextColor(TFT_PINK, busy ? TFT_MAROON : TFT_OLIVE);
    tft.printf("Sumayyah Alshehri");

}

// ------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(2500);                         // DHT need 2msec min
  
  tft.init();  tft.setRotation(1);

  dht.setup(DHT_PIN, DHTesp::DHT11);

  pinMode(BTN_BUSY, INPUT_PULLUP);
  pinMode(BTN_FREE, INPUT_PULLUP);

  drawStatus(busy);                    // green screen on at beggineing 
}

// ------------------------------------------------------------------
void loop()
{
  uint32_t now = millis(); // for the debouncing 

  // State machine 
  // -------- handle buttons ----------
  if (now - lastBtn > DEBOUNCE_MS)
  {
    if (digitalRead(BTN_BUSY) == LOW) 
     { 
      busy = true; 
      drawStatus(busy); 
      lastBtn = now;
     }
    if (digitalRead(BTN_FREE) == LOW)  
    { 
      busy = false;
       drawStatus(busy);
        lastBtn = now;
     }
  }

  // -------- read sensor -------------
  if (now - lastTemp >= REFRESH_MS)
  {
    lastTemp = now; // update timing 
    auto d = dht.getTempAndHumidity();
    if (!dht.getStatus())
    {
      drawReadings(d.temperature, d.humidity);
      Serial.printf("T %.1f°C  H %.1f%%\n", d.temperature, d.humidity);
    }
    else
      Serial.println("DHT read failed");
  }
}
