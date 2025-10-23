/*******************************************************
 * ESP32-C3 MORSE TRAINER v7.0 – YD9CVS (Morse Tree Lite)
 * - ST7735 TFT (SPI) + SI5351 (I2C)
 * - Tampilan pohon tanpa garis (lite mode)
 * - Auto-Spacing + Running Text + Highlight jalur
 *******************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <si5351.h>

/* ==== PINOUT ==== */
#define I2C_SDA   8
#define I2C_SCL   9
#define TFT_CS    7
#define TFT_DC    3
#define TFT_RST   5
#define TFT_SCK   10
#define TFT_MOSI  6
#define PADDLE_DIT 0
#define PADDLE_DAH 1
#define BUZZER_PIN 2
#define LED_PIN    4

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
Si5351 si5351;

/* ==== VARIABEL ==== */
const int SW = 128, SH = 160;
int  WPM = 15;
int  ToneHz = 750;
int  ditTime = 80;
String morseBuffer = "";
String runningText = "";
char decodedChar = ' ';
unsigned long lastKeyTime = 0;
bool ditActive = false, dahActive = false;

/* ==== WARNA ==== */
#define C_BG     ST77XX_BLACK
#define C_TXT    ST77XX_WHITE
#define C_DIT    ST77XX_RED
#define C_DAH    ST77XX_BLUE
#define C_HILITE ST77XX_YELLOW
#define C_GRID   0x39E7

/* ==== LAYOUT ==== */
const int HEADER_H = 16;
const int FOOTER_H = 24;
const int BAR_Y    = SH - FOOTER_H + 4;
const int BAR_X0   = 4;
const int BAR_W    = SW - 8;
const int BAR_H    = 5;

/* ==== UTIL ==== */
void calcDit() { ditTime = 1200 / WPM; }
void beepStart() { tone(BUZZER_PIN, ToneHz); digitalWrite(LED_PIN, HIGH); }
void beepStop()  { noTone(BUZZER_PIN); digitalWrite(LED_PIN, LOW); }

/* ==== HEADER ==== */
void drawHeader() {
  tft.fillRect(0, 0, SW, HEADER_H, C_BG);
  tft.setTextSize(1); tft.setTextColor(C_TXT);
  tft.setCursor(2, 3);  tft.print("WPM:"); tft.print(WPM);
  tft.setCursor(58, 3); tft.print("Hz:");  tft.print(ToneHz);
  tft.setCursor(100, 3); tft.print(decodedChar);
}

/* ==== FOOTER ==== */
void drawFooterBase() {
  tft.fillRect(0, SH - FOOTER_H, SW, FOOTER_H, C_BG);
  tft.drawFastHLine(BAR_X0, BAR_Y, BAR_W, C_GRID);
}

/* ==== TONE BAR ==== */
void drawToneBar(bool isDit) {
  uint16_t c = isDit ? C_DIT : C_DAH;
  int dur = isDit ? ditTime : ditTime * 3;
  int step = max(1, BAR_W / max(1, dur / 10));
  int pos = BAR_X0;
  unsigned long start = millis();
  while (millis() - start < (unsigned long)dur) {
    tft.fillRect(BAR_X0, BAR_Y - 1, BAR_W, BAR_H, C_BG);
    tft.fillRect(pos, BAR_Y - 1, 8, BAR_H, c);
    pos += step;
    delay(8);
  }
  tft.fillRect(BAR_X0, BAR_Y - 1, BAR_W, BAR_H, C_BG);
  tft.drawFastHLine(BAR_X0, BAR_Y, BAR_W, C_GRID);
}

/* ==== DECODE ==== */
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

/* ==== DATA LEVEL POHON ==== */
const char* L1[] = {"E","T"};
const char* L2[] = {"I","A","M","N"};
const char* L3[] = {"S","U","R","W","D","K","G","O"};
const char* L4[] = {"H","V","F","L","P","J","B","X","C","Y","Z","Q"};
const char* L5[] = {"5","4","3","2","1","6","7","8","9","0"};
const uint8_t N1=2,N2=4,N3=8,N4=12,N5=10;

/* ==== DRAW LEVEL ==== */
void drawLevel(const char** arr,uint8_t N,int y,const String& hi){
  int gap = SW/(N+1);
  for(uint8_t i=0;i<N;i++){
    int x=gap*(i+1);
    const char* s=arr[i];
    bool highlight=false;
    for(uint8_t k=0;k<hi.length();k++)
      if(hi[k]==s[0]) highlight=true;
    uint16_t col=(i<N/2)?C_DIT:C_DAH;
    if(highlight)tft.drawRect(x-7,y-7,14,14,C_HILITE);
    tft.setTextSize(1); tft.setTextColor(col);
    tft.setCursor(x-3,y-4); tft.print(s);
  }
}

/* ==== DRAW TREE ==== */
void drawTree(const String& code){
  tft.fillRect(0,HEADER_H+1,SW,SH-HEADER_H-FOOTER_H-2,C_BG);
  String hi="";
  for(uint8_t i=1;i<=code.length();i++){
    char c=decodeMorse(code.substring(0,i));
    if(c!='?')hi+=c;
  }
  int y1=HEADER_H+10;
  int y2=HEADER_H+28;
  int y3=HEADER_H+46;
  int y4=HEADER_H+64;
  int y5=HEADER_H+82;
  drawLevel(L1,N1,y1,hi);
  drawLevel(L2,N2,y2,hi);
  drawLevel(L3,N3,y3,hi);
  drawLevel(L4,N4,y4,hi);
  drawLevel(L5,N5,y5,hi);
}

/* ==== RUNNING TEXT ==== */
void drawRunningText(){
  tft.fillRect(0,SH-11,SW,11,C_BG);
  tft.setTextSize(1); tft.setTextColor(0x07E0);
  if(runningText.length()>26)runningText.remove(0,runningText.length()-26);
  tft.setCursor(2,SH-9); tft.print(runningText);
}

/* ==== STARTUP ==== */
void startup(){
  tft.fillScreen(C_BG);
  tft.setTextSize(2); tft.setTextColor(ST77XX_RED);
  tft.setCursor(20,60); tft.print("YD9CVS");
  tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(26,80); tft.print("MORSE TRAINER");
  delay(1000);
}

/* ==== SETUP ==== */
void setup(){
  pinMode(PADDLE_DIT,INPUT_PULLUP);
  pinMode(PADDLE_DAH,INPUT_PULLUP);
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
  Wire.begin(I2C_SDA,I2C_SCL);
  SPI.begin(TFT_SCK,-1,TFT_MOSI,TFT_CS);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(C_BG);
  si5351.init(SI5351_CRYSTAL_LOAD_8PF,0,0);
  si5351.output_enable(SI5351_CLK0,0);
  startup();
  calcDit();
  drawHeader();
  drawFooterBase();
  drawTree("");
  drawRunningText();
}

/* ==== LOOP ==== */
void loop(){
  bool dit=!digitalRead(PADDLE_DIT);
  bool dah=!digitalRead(PADDLE_DAH);

  if(dit && !ditActive){
    ditActive=true; beepStart(); morseBuffer+="."; drawToneBar(true);
    beepStop(); lastKeyTime=millis(); drawTree(morseBuffer);
  }else if(!dit && ditActive)ditActive=false;

  if(dah && !dahActive){
    dahActive=true; beepStart(); morseBuffer+="-"; drawToneBar(false);
    beepStop(); lastKeyTime=millis(); drawTree(morseBuffer);
  }else if(!dah && dahActive)dahActive=false;

  unsigned long gap=millis()-lastKeyTime;
  if(morseBuffer.length()>0 && gap>=ditTime*3 && gap<ditTime*7){
    decodedChar=decodeMorse(morseBuffer);
    morseBuffer="";
    runningText+=decodedChar=='?'?'#':decodedChar;
    drawHeader(); drawRunningText(); drawTree("");
  }
  if(gap>=ditTime*7 && runningText.length()>0){
    if(runningText[runningText.length()-1]!=' '){
      runningText+=' '; drawRunningText();
    }
  }
}
