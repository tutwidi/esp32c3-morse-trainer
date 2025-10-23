/*******************************************************
 * ESP32-C3 MORSE TRAINER v6.1 – (FINAL VISUAL)
 * - ST7735 TFT (SPI) + SI5351 (I2C)
 * - Iambic Right-Hand Paddle (GPIO0/1)
 * - Header: WPM + Voltage + Last Decoded Char
 * - Footer: Tone bar + Real-time dot/dash indicator
 * - Highlight decoded character in table
 *******************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <si5351.h>

/* ======== KONFIGURASI PIN ======== */
#define I2C_SDA   8
#define I2C_SCL   9
#define TFT_CS    7
#define TFT_DC    3
#define TFT_RST   5
#define TFT_SCK   10
#define TFT_MOSI  6
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

#define PADDLE_DIT 0
#define PADDLE_DAH 1
#define BUZZER_PIN 2
#define LED_PIN    4

/* ======== VARIABEL ======== */
const int SW = 128, SH = 160;
int  WPM = 15;
int  ditTime = 80;
String morseBuffer = "";
char decodedChar = ' ';
unsigned long lastKeyTime = 0;
bool ditActive = false, dahActive = false;
Si5351 si5351;

/* ======== WARNA ======== */
#define C_BG        ST77XX_BLACK
#define C_TXT       ST77XX_WHITE
#define C_HILITE_BG ST77XX_YELLOW
#define C_HILITE_TX ST77XX_BLACK
#define C_DITBAR    ST77XX_RED
#define C_DAHBAR    0xFD60
#define C_GRID      0x39E7

/* ======== POSISI UI ======== */
const int HEADER_H = 18;
const int TABLE_Y0 = HEADER_H + 2;
const int ROW_H    = 16;
const int FOOTER_H = 26;
const int BAR_Y    = SH - FOOTER_H + 4;
const int BAR_X0   = 4;
const int BAR_W    = SW - 8;
const int BAR_H    = 6;

/* ======== TABEL MORSE ======== */
struct Row { const char* left; const char* right; };
Row morseTable[] = {
  {"E", "T"},
  {"I A", "M N"},
  {"W U S R", "D G K O Y"},
  {"V P L J H F", "B C Q X Z"},
  {"5 4 3 2 1", "6 7 8 9 0"},
  {"& ? / = +", "– . , ; :"}
};
const int NROWS = sizeof(morseTable)/sizeof(morseTable[0]);

/* ======== UTIL ======== */
void calcDit() { ditTime = 1200 / WPM; }
void beepStart() { tone(BUZZER_PIN, 800); digitalWrite(LED_PIN, HIGH); }
void beepStop()  { noTone(BUZZER_PIN);  digitalWrite(LED_PIN, LOW); }

/* ======== HEADER ======== */
void drawHeader() {
  tft.fillRect(0, 0, SW, HEADER_H, C_BG);
  tft.setTextSize(1); tft.setTextColor(C_TXT);
  tft.setCursor(2, 4);
  tft.print("WPM: "); tft.print(WPM);

  // voltage placeholder
  tft.setCursor(60, 4);
  tft.print("3.7V");

  // last decoded char
  tft.setTextSize(2);
  tft.setCursor(100, 1);
  tft.print(decodedChar);
  tft.setTextSize(1);
}

/* ======== TABEL MORSE ======== */
void drawTable(char highlight = '\0') {
  tft.fillRect(0, TABLE_Y0, SW, SH - FOOTER_H - TABLE_Y0, C_BG);
  tft.setTextSize(1);
  int y = TABLE_Y0 + 2;

  for (int i = 0; i < NROWS; i++) {
    String left = morseTable[i].left;
    String right = morseTable[i].right;
    bool hl = (left.indexOf(highlight) >= 0 || right.indexOf(highlight) >= 0);

    if (hl) tft.fillRect(0, y - 1, SW, ROW_H, C_HILITE_BG);
    else if (i % 2) tft.fillRect(0, y - 1, SW, ROW_H, 0x2104);

    tft.setTextColor(hl ? C_HILITE_TX : C_TXT);
    tft.setCursor(3, y);  tft.print(left);
    tft.setCursor(60, y); tft.print("|");
    tft.setCursor(68, y); tft.print(right);
    y += ROW_H;
  }
}

/* ======== FOOTER ======== */
void footerBase() {
  tft.fillRect(0, SH - FOOTER_H, SW, FOOTER_H, C_BG);
  tft.drawFastHLine(BAR_X0, BAR_Y, BAR_W, C_GRID);
}

/* ======== BAR & REALTIME VISUAL ======== */
void drawToneBar(bool isDit) {
  uint16_t c = isDit ? C_DITBAR : C_DAHBAR;
  int dur = isDit ? ditTime : ditTime * 3;
  int step = BAR_W / (dur / 10);
  int pos = BAR_X0;

  unsigned long start = millis();
  while (millis() - start < dur) {
    tft.fillRect(BAR_X0, BAR_Y - 2, BAR_W, BAR_H, C_BG);
    tft.fillRect(pos, BAR_Y - 2, 8, BAR_H, c);
    // real-time visual below bar
    if (isDit) { tft.setCursor(BAR_X0 + 20, BAR_Y + 8); tft.print(". "); }
    else { tft.setCursor(BAR_X0 + 20, BAR_Y + 8); tft.print("- "); }
    pos += step;
    delay(10);
  }

  tft.fillRect(BAR_X0, BAR_Y - 2, BAR_W, BAR_H + 8, C_BG);
  tft.drawFastHLine(BAR_X0, BAR_Y, BAR_W, C_GRID);
}

/* ======== DECODER ======== */
char decodeMorse(String code) {
  if (code == ".-") return 'A'; if (code == "-...") return 'B';
  if (code == "-.-.") return 'C'; if (code == "-..") return 'D';
  if (code == ".") return 'E';   if (code == "..-.") return 'F';
  if (code == "--.") return 'G'; if (code == "....") return 'H';
  if (code == "..") return 'I';  if (code == ".---") return 'J';
  if (code == "-.-") return 'K'; if (code == ".-..") return 'L';
  if (code == "--") return 'M';  if (code == "-.") return 'N';
  if (code == "---") return 'O'; if (code == ".--.") return 'P';
  if (code == "--.-") return 'Q'; if (code == ".-.") return 'R';
  if (code == "...") return 'S'; if (code == "-") return 'T';
  if (code == "..-") return 'U'; if (code == "...-") return 'V';
  if (code == ".--") return 'W'; if (code == "-..-") return 'X';
  if (code == "-.--") return 'Y'; if (code == "--..") return 'Z';
  if (code == "-----") return '0'; if (code == ".----") return '1';
  if (code == "..---") return '2'; if (code == "...--") return '3';
  if (code == "....-") return '4'; if (code == ".....") return '5';
  if (code == "-....") return '6'; if (code == "--...") return '7';
  if (code == "---..") return '8'; if (code == "----.") return '9';
  return '?';
}

/* ======== SETUP ======== */
void setup() {
  pinMode(PADDLE_DIT, INPUT_PULLUP);
  pinMode(PADDLE_DAH, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(C_BG);

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  si5351.output_enable(SI5351_CLK0, 0);

  calcDit();
  drawHeader();
  drawTable();
  footerBase();
}

/* ======== LOOP ======== */
void loop() {
  bool dit = !digitalRead(PADDLE_DIT);
  bool dah = !digitalRead(PADDLE_DAH);

  // DIT tekan
  if (dit && !ditActive) {
    ditActive = true;
    beepStart();
    morseBuffer += ".";
    drawToneBar(true);
    beepStop();
    lastKeyTime = millis();
  } else if (!dit && ditActive) {
    ditActive = false;
  }

  // DAH tekan
  if (dah && !dahActive) {
    dahActive = true;
    beepStart();
    morseBuffer += "-";
    drawToneBar(false);
    beepStop();
    lastKeyTime = millis();
  } else if (!dah && dahActive) {
    dahActive = false;
  }

  // Deteksi akhir huruf
  if (morseBuffer.length() > 0 && millis() - lastKeyTime > ditTime * 3) {
    decodedChar = decodeMorse(morseBuffer);
    morseBuffer = "";
    drawHeader();
    drawTable(decodedChar);
  }
}
