// v0.0000000001

#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Create the lcd object address 0x3F and 16 columns x 2 rows

const char* ssid = "YOURSSID"; // Enter your wifi SSID
const char* password = "YOURPASSWORD"; // Enter your wifi password

double prevPrice = 0;

// Down arrow
byte customChar1[8] = {
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00000
};

// Up arrow
byte customChar2[8] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};

void setup() {
  lcd.begin();                           // Initialize the LCD connected
  lcd.backlight();                       // Turn on the backlight on LCD.
  lcd.createChar(0, customChar1);
  lcd.createChar(1, customChar2);
  WiFi.begin(ssid, password);            // Setup wifi connection
  lcd.setCursor(0, 0);                   // Set cursor to the first character space  (0) of the first line (0)
  lcd.print("Establish WiFi..");         // Print a message to the cursor position
  while (WiFi.status() != WL_CONNECTED)  // Connect to the WiFi network
  {
    lcd.setCursor(0, 1);
    lcd.write(0);
    delay(2000);
  }
  lcd.println();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Connected to: ");
  lcd.setCursor(0, 1);  // Display message once connected
  lcd.print(ssid);
  delay(2500);
  lcd.clear();
}

void loop() {
  double currentPrice = getSHIBPrice();   // Get the data
  lcd.setCursor(0, 0);                    // Set cursor line 1 character 1
  lcd.print("SHIB $");                    // Print current price
  lcd.print(currentPrice, 13);            // Current price at 8 decimal places
  if (prevPrice > 0) {                    // Check for price change
    double change = currentPrice - prevPrice;
    lcd.setCursor(0, 1);
    if (change > 0) {
      lcd.print((char)1);
      lcd.print(" $");
      lcd.print(change, 13);
    } else if (change < 0) {
      lcd.print((char)1);
      lcd.print(" $");
      lcd.print(change, 13);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("   No  Change   ");
    }
  }

  prevPrice = currentPrice;               // Update previous price
  delay(2 * 60 * 1000);                   // Wait some amount of minuets before updating
}

double getSHIBPrice() {
  HTTPClient http;

  // Make API call
  http.begin("https://api.coingecko.com/api/v3/simple/price?ids=shiba-inu&vs_currencies=usd&precision=14");
  int httpCode = http.GET();

  if (httpCode > 0) {
    // Parse JSON response
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, http.getString());

    // Get current price
    double currentPrice = doc["shiba-inu"]["usd"];
    return currentPrice;
  } else {
    return 0;
  }
}
