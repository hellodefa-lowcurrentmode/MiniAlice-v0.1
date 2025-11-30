#define BLYNK_TEMPLATE_ID "TMPL6wczGmyxn"
#define BLYNK_TEMPLATE_NAME "MiniRobo"
#define BLYNK_AUTH_TOKEN "W_N0QCcE3j0elUli90VSAF-nKp8e9sJO"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>


/* #include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial */
char auth[] = BLYNK_AUTH_TOKEN;
 
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "BARASANJI V1";
char pass[] = "Valverde123@";

/* char ssid[] = "HAUS COFFEA";
char pass[] = "hauskumi"; */


//SDA 21, SCL 22;
/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TOUCH_PIN  27       // ubah sesuai pin TTP223
#define LONG_PRESS_TIME 1000   // 1000 ms = 1 detik
#define MESSAGE_DISPLAY_TIME 2000

RoboEyes<Adafruit_SH1106G> roboEyes(display); 


// -------------------------------------------------
// STATE MANAGEMENT
// -------------------------------------------------
enum SystemState {
  IDLE,
  SHORT_PRESS_ACTIVE,
  LONG_PRESS_ACTIVE
};

SystemState state = IDLE;

// -------------------------------------------------
// TIMER VARIABLES
// -------------------------------------------------
unsigned long splashStart = 0;
unsigned long pressStart = 0;
unsigned long moodChangeTime = 0;
unsigned long textStartTime = 0;
unsigned long returnToDefaultTime = 0;

bool lastButton = false;
bool textShownOnce = false;

const unsigned long splashDuration = 2000;    // 2 detik
const unsigned long longPressTime = 1000;     // 1 detik
const unsigned long showTextAfter = 1000;     // 1 detik setelah mood berubah
const unsigned long textDuration = 2000;      // durasi teks 2 detik
const unsigned long frameDuration = 4000;   
const unsigned long delayReturnMood = 2000;   // kembali default setelah 2 detik


// ==== TEXT MANAGEMENT ====
String incomingText = "";
bool textDisplaying = false;


/* BLYNK_WRITE(V3)
{
  incomingText = param.asStr();
  Serial.printf("The Incoming Text : %s \n", incomingText);
} */

// V1 = Button Submit
BLYNK_WRITE(V1)
{

  int btn = param.asInt();

  /* Serial.printf("Button Contained With Value %u \n", btn);
  Serial.printf("text is : %s \n",incomingText); */
  //Serial.println(incomingText);

  if (btn == 1)   // tombol ditekan
  {
    showCustomText("Hello Defa");
    /* if (incomingText.length() > 0)
    {
      showCustomText(incomingText);
    } */
  }
}

// =====================================================
// TAMPILKAN TEKS DI OLED (NON-BLOCKING)
// =====================================================
void showCustomText(String txt)
{
  // Matikan animasi sementara
  //roboEyes.setMood(OFF);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 15);
  display.println("Alice says");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 25);
  // Display static text
  display.println("HelloDefa!");
  display.display();

  /* display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.print(txt);
  display.display();

  textStartTime = millis();
  textDisplaying = true; */

  delay(2000);

  

  Serial.printf("The Text Was Printed : %s ", txt);
}

/* void drawCentreString(const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor(x - w / 2, y);
    display.print(buf);
} */

// -------------------------------------------------
// SETUP
// -------------------------------------------------
void setup() {


  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  
  pinMode(TOUCH_PIN, INPUT);

  delay(250); // wait for the OLED to power up

  display.begin(i2c_Address, true); // Address 0x3C default
  
  //BOOT DISPLAY  
 /*  display.clearDisplay();
  //display.setCursor(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2);
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.print("HELLODEFA");
  display.display(); */
  //display.setTextColor(SH110X_WHITE);
  //drawCentreString("HELLODEFA", 13, 12);

  


  

  //splashStart = millis();

/* 
  if (millis() - splashStart >= splashDuration) {
    display.clearDisplay();
    roboEyes.setMood(DEFAULT);
    
  } */
      
  state = IDLE;
  
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
    // Define some automated eyes behaviour
  roboEyes.setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
  roboEyes.setIdleMode(ON, 2, 2); 

  roboEyes.setSweat(false);
  roboEyes.setMood(DEFAULT);
  roboEyes.setCuriosity(ON);
}

// -------------------------------------------------
// LOOP (NO DELAY EVER)
// -------------------------------------------------

//bool splashing = true;
void loop() {

  /* if (splashing && ((millis() - textStartTime) >= frameDuration))
  {
     splashing=false;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("HelloDefa V0.1 Meet : ");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 25);
  // Display static text
  display.println("MiniAlice");
  display.display();
  delay(4000);
  display.clearDisplay();
 

  } */

  roboEyes.update(); 

  
 

  //Blynk.virtualWrite(V0,weight_In_g);
  Blynk.run();

  // VERY IMPORTANT (FPS)

  bool button = digitalRead(TOUCH_PIN);

  // ============================================================
  // 1. BOOT SPLASH STATE
  // ============================================================
  // Hapus teks setelah 2 detik → kembali ke RoboEyes
  if (textDisplaying && ((millis() - textStartTime) >= textDuration))
  {
    display.clearDisplay();
    roboEyes.setSweat(false);
    roboEyes.setMood(DEFAULT);
    textDisplaying = false;
    delay(2000);
  }

  // ============================================================
  // 2. IDLE STATE
  // ============================================================
  else if (state == IDLE) {

    if (button && !lastButton) {
      // start press timer
      pressStart = millis();
      textShownOnce = false;
    }

    if (button) {
      // cek LONG PRESS
      if (millis() - pressStart >= longPressTime) {
        roboEyes.setMood(HAPPY);
        roboEyes.anim_laugh();
        moodChangeTime = millis();
        state = LONG_PRESS_ACTIVE;
      }
    }

    if (!button && lastButton) {
      // SHORT PRESS DETECTED
      //roboEyes.setSweat(true);
      roboEyes.setMood(ANGRY);
      moodChangeTime = millis();
      state = SHORT_PRESS_ACTIVE;
    }
  }

  // ============================================================
  // 3. SHORT PRESS STATE
  // ============================================================
  else if (state == SHORT_PRESS_ACTIVE) {

    // tampilkan teks setelah 1 detik
    if (!textShownOnce && millis() - moodChangeTime >= showTextAfter) {
      touchNoticeText(1);
      textStartTime = millis();
      textShownOnce = true;
    }

    // hapus teks setelah 2 detik
    if (textShownOnce && millis() - textStartTime >= textDuration) {
      display.clearDisplay();
      returnToDefaultTime = millis();
    }

    // kembalikan mood 2 detik setelah teks hilang
    if (textShownOnce &&
      millis() - textStartTime >= textDuration + delayReturnMood) {
        roboEyes.setSweat(false);
      roboEyes.setMood(DEFAULT);
      state = IDLE;
    }
  }

  // ============================================================
  // 4. LONG PRESS STATE
  // ============================================================
  else if (state == LONG_PRESS_ACTIVE) {
    // tampilkan teks sekali saja setelah 1 detik
    if (!textShownOnce && millis() - moodChangeTime >= showTextAfter) {
       touchNoticeText(2);
      textStartTime = millis();
      textShownOnce = true;
    }

    // hapus teks setelah 2 detik
    if (textShownOnce && millis() - textStartTime >= textDuration) {
      display.clearDisplay();
    }

    // saat tombol dilepas → set timer kembali ke default
    if (!button && lastButton) {
      returnToDefaultTime = millis();
    }

    // 2 detik setelah dilepas → kembali mood DEFAULT
    if (!button &&
        millis() - returnToDefaultTime >= delayReturnMood) {
      roboEyes.setSweat(false);
      roboEyes.setMood(DEFAULT);
      state = IDLE;
    }
  }

  lastButton = button;
}

void touchNoticeText(int type){

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);

    if(type==1){
      display.print("i'm angry now, No fkin touch !");
    }else{
      display.print("Yeah, keep going \n with that puk-puk thing");
    }

    display.display();
    delay(2000);

    Serial.println("Message Sent !");

 }


 
