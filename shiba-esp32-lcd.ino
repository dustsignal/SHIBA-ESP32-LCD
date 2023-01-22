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

void setup() {
  lcd.begin();                           // Initialize the LCD connected
  lcd.backlight();                       // Turn on the backlight on LCD.
  WiFi.begin(ssid, password);            // Setup wifi connection
  lcd.setCursor(0, 0);                   // Set cursor to the first character space  (0) of the first line (0)
  lcd.print("Establish WiFi..");         // Print a message to the cursor position
  while (WiFi.status() != WL_CONNECTED)  // Connect to the WiFi network
  {
    lcd.setCursor(0, 1);
    delay(500);
    lcd.write(0);
    delay(250);
  }
  lcd.println();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Connected to: ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(2500);
  lcd.clear();
}

void loop() {
  double currentPrice = getSHIBPrice();  // Get the data
  lcd.setCursor(0, 0);                   // Set cursor line 1 character 1
  lcd.print("SHIB: $");                  // Print current price
  lcd.print(currentPrice, 8);            // Current price at 8 decimal places
  if (prevPrice > 0) {                   // Check for price change
    double change = currentPrice - prevPrice;
    lcd.setCursor(0, 1);
    if (change > 0) {
      lcd.print("Up: $");
      lcd.print(change, 8);
    } else if (change < 0) {
      lcd.print("Down: $");
      lcd.print(change, 8);
    } else {
      lcd.print("No Change");
    }
  }
  prevPrice = currentPrice;  // Update previous price
  delay(19 * 60 * 1000);     // Wait for 19 minuets before updating -- change 19 to whatever you want. FYI Coingecko has a limit on free API requests.
}

double getSHIBPrice() {
  HTTPClient http;
  // Make API call
  http.begin("https://api.coingecko.com/api/v3/simple/price?ids=shiba-inu&vs_currencies=usd&precision=9");
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
