//Otero - Fraccaro - Ponczyk - Mofsovich
#include <U8g2lib.h>
#include "DHT.h"
#include <ESP32Time.h>
#include <WiFi.h>
#include <time.h>
ESP32Time rtc;
/* ----------------------------------------------------------------------------------------------------- */

// Definicion de constructores y variables globales

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/* ----------------------------------------------------------------------------------------------------- */
void printBMP_OLED(void );
void printBMP_OLED2(void) ;
#define BOTON1 34
#define BOTON2 35
#define P1 0
#define P2 1
#define RST 20
#define ESPERA1 2
#define ESPERA2 3
#define SUMAR 4
#define RESTAR 5
int estado = RST;
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temp;
int millis_valor;
int millis_actual;
int gmt = 0;
struct tm tiempo;

const char* ssid = "ORT-IoT";
const char* password = "NuevaIOT$25";
const char* ntpServer = "pool.ntp.org";

void setup() {
  pinMode(BOTON1, INPUT_PULLUP);
  pinMode(BOTON2, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  u8g2.begin();
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi!");
  TiempoGMT();
}

void loop() {
  millis_actual = millis();
  if (millis_actual - millis_valor >= 2000) {
    temp = dht.readTemperature();
    if (isnan(temp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }


    switch (estado) {
      case RST:
        {
          millis_valor = millis();
          estado = P1;
        }
        break;
      case P1:
        {
          printBMP_OLED();
          if (digitalRead(BOTON1) == LOW && digitalRead(BOTON2) == LOW) {
            estado = ESPERA1;
          }
        }
        break;
      case ESPERA1:
        {
          if (digitalRead(BOTON1) == HIGH && digitalRead(BOTON2) == HIGH) {
            estado = P2;
          }

        }
        break;
      case P2:
        {
          printBMP_OLED2();
          if (digitalRead(BOTON1) == LOW) {
            estado = SUMAR;
          }
          if (digitalRead(BOTON2) == LOW) {
            estado = RESTAR;
          }
          if (digitalRead(BOTON1) == LOW && digitalRead(BOTON2) == LOW) {
            estado = ESPERA2;
          }
        }
        break;
      case ESPERA2:
        {
          if (digitalRead(BOTON1) == HIGH && digitalRead(BOTON2) == HIGH) {
            estado = P1;
          }
        }
        break;

      case SUMAR:
        {
          if (digitalRead(BOTON1) == HIGH) {
            gmt++;
              if (gmt > 12) {  
                gmt=-12;
                TiempoGMT();
            }
            estado = P2;
          }
        }
        break;

      case RESTAR:
        {
          if (digitalRead(BOTON2) == HIGH) {
            gmt--;
            if (gmt < -12) {
              gmt=12;
              TiempoGMT();
            }
            estado = P2;
          }
        }
        break;
    }

  }
}

void TiempoGMT() {
  configTime(gmt * 3600, 0, ntpServer); // gmt en segundos
  if (getLocalTime(&tiempo)) {
    rtc.setTimeStruct(tiempo); // Copia la hora NTP al RTC interno
    Serial.println("Hora sincronizada con NTP:");
    Serial.print(tiempo.tm_hour);
    Serial.print(":");
    Serial.println(tiempo.tm_min);
  } else {
    Serial.println("Error al obtener hora");
  }
}

void printBMP_OLED(void) {
  char stringU[5];
  char stringtemp[10];
  char horaStr[10];
  char minStr[10];
  int horaActual = tiempo.tm_hour;
  int minutoActual = tiempo.tm_min;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11b_tr); // choose a suitable font
  sprintf (stringtemp, "%.2f" , temp);
  sprintf(horaStr, "%02d", horaActual);   
  sprintf(minStr, "%02d", minutoActual);  
  u8g2.drawStr(0, 35, "Hora: ");
  u8g2.drawStr(40, 35, horaStr);
  u8g2.drawStr(60, 35, ":");
  u8g2.drawStr(70, 35, minStr);
  u8g2.drawStr(0, 50, "T. Actual:");
  u8g2.drawStr(60, 50, stringtemp);
  u8g2.drawStr(90, 50, "°C");
  u8g2.sendBuffer();
}

void printBMP_OLED2(void) {
  char gmtStr[10];
  sprintf(gmtStr, "%+d", gmt);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11b_tr);
  u8g2.drawStr(0, 50, "GMT:");
  u8g2.drawStr(35, 50, gmtStr);
  u8g2.sendBuffer();
}
