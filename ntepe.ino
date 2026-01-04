#include <WiFi.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

const char *ssid = ""; // Username wifi
const char *password = ""; // Password wifi

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define led 19

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long prevPrint = 0;
int barX = 14;
int barY = 30;
int barW = 100;
int barH = 10;
int proses = 0;
int persen = 0;

// Logo Rumah
const unsigned char logo_rumah [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x58, 
  0x00, 0x0d, 0xbc, 0x00, 0x0f, 0x7e, 0x00, 0x0e, 
	0xfd, 0x00, 0x0d, 0xfe, 0x80, 0x1f, 0xff, 0xc0, 
  0x3f, 0xff, 0xe0, 0x3f, 0xff, 0xd0, 0x1f, 0xff, 
	0xc0, 0x1f, 0xff, 0xc0, 0x1f, 0xff, 0xc0, 0x1f, 
  0xff, 0xc0, 0x1f, 0xcf, 0xc0, 0x1f, 0x8f, 0xc0, 
	0x1f, 0x8f, 0xc0, 0x1f, 0x8f, 0xc0, 0x0f, 0x87, 
  0x80, 0x00, 0x00, 0x00
};

// Logo Jam
const unsigned char logo_jam [] PROGMEM = {
  0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x07, 0xff, 
  0x00, 0x0f, 0xff, 0x80, 0x1f, 0xff, 0xc0, 0x3f, 
	0xff, 0xe0, 0x3f, 0xff, 0xf0, 0x7f, 0xff, 0xb0, 
  0x7f, 0xfc, 0xf0, 0x7f, 0xc7, 0xf0, 0x7f, 0xdf, 
	0xf0, 0x7f, 0xdf, 0xf0, 0x7f, 0xdf, 0xf0, 0x3f, 
  0xdf, 0xf0, 0x3f, 0xff, 0xc0, 0x1f, 0xff, 0xc0, 
	0x0f, 0xff, 0xc0, 0x07, 0xff, 0x00, 0x01, 0xfc, 
  0x00, 0x00, 0x00, 0x00
};

// Nama Hari
const char* nama_hari[] = {
  "MINGGU", "SENIN", "SELASA", "RABU", 
  "KAMIS", "JUM'AT", "SABTU"
};

void setup() {
  Wire.begin(21, 22);
  pinMode(led, OUTPUT);
  
  display.setTextColor(SSD1306_WHITE);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(22, 12);
    display.print("CONNECTING WIFI");

    bar();
    animasi(proses);

    persen = map(proses, 0, barW -4, 0, 100);
    display.setCursor(55, barY + barH + 4);
    display.print(persen);
    display.print(" %");
    display.display();

    proses++;
    if(proses > barW -4) proses = 0;

  }

  // Tampilan Zona Waktu
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); //  -> ZONA WAKTU WIB
  //configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");   -> ZONA WAKTU WITA
  //configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");   -> ZONA WAKTU WITA

  if(WiFi.status() == WL_CONNECTED){
    display.clearDisplay();
    display.setCursor(25, 30);
    display.print("WIFI CONNECTED");
    delay(700);
    display.display();
  }
}

void loop() {
  struct tm waktu;
  if(!getLocalTime(&waktu)){
    display.setTextSize(1);
    display.setCursor(20, 20);
    display.print("WAKTU GAGAL");
    return;
  }

  int jam = waktu.tm_hour;
  int menit = waktu.tm_min;
  int detik = waktu.tm_sec;
  int tanggal = waktu.tm_mday;
  int bulan = waktu.tm_mon + 1;
  int tahun = waktu.tm_year + 1900;

  if (millis() - prevPrint >= 1000){
    prevPrint = millis();

    display.clearDisplay();
    display.drawBitmap(0, 0, logo_rumah, 21, 20, SSD1306_WHITE);
    display.drawBitmap(106, 0, logo_jam, 21, 20, SSD1306_WHITE);

    // Tampilan Tanggal, Bulan, Tahun
    display.setTextSize(1);
    display.setCursor(35, 0);
    display.printf("%02d-%02d-%02d", tanggal, bulan, tahun);

    // Tampilan Jam, Menit, detik
    display.setTextSize(2);
    display.setCursor(18, 25);
    display.printf("%02d:%02d:%02d", jam, menit, detik);

    // Tampilan Hari
    display.setTextSize(1);
    display.setCursor(50, 50);
    display.print(nama_hari[waktu.tm_wday]);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  } 

  display.display();
}

void bar(){
  display.drawRect(barX, barY, barW, barH, SSD1306_WHITE);
}

void animasi(int proses){
  display.fillRect(barX +2, barY +2, proses, barH -4, SSD1306_WHITE);
}
