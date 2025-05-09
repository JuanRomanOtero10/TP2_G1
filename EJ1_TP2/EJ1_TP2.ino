// Otero - Fraccaro - Ponczyk - Mofsovich
#include <U8g2lib.h>
#include "DHT.h"
#include <ESP32Time.h>
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
#define HORAS 4
#define MINUTOS 5
int estado = RST;
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temp;
int millis_valor;
int millis_actual;
int hora = 9;
int minuto = 59;


void setup() {
  pinMode(BOTON1, INPUT_PULLUP);
  pinMode(BOTON2, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  u8g2.begin();
  dht.begin();
  rtc.setTime(0, minuto, hora, 25, 4, 2025);
  Serial.println(rtc.getTime());
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
            estado = HORAS;
          }
          if (digitalRead(BOTON2) == LOW) {
            estado = MINUTOS;
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

      case HORAS:
        {

          if (digitalRead(BOTON1) == HIGH) {
            hora = hora + 1;
            if (hora == 24) {
              hora = 0;
            }
            rtc.setTime(0, minuto, hora, 25, 4, 2025);
            estado = P2;
          }

        }
        break;

      case MINUTOS:
        {
          if (digitalRead(BOTON2) == HIGH) {
            minuto = minuto + 1;
            if (minuto == 60) {
              minuto = 0;
            }
            rtc.setTime(0, minuto, hora, 25, 4, 2025);
            estado = P2;
          }

        }
        break;
    }

  }
}




void printBMP_OLED(void) {
  char stringU[5];
  char stringtemp[10];
  char horaStr[10];  
  int horaActual = rtc.getHour(true);  
  int minutoActual = rtc.getMinute();  
  u8g2.clearBuffer();          
  u8g2.setFont(u8g2_font_t0_11b_tr); // choose a suitable font
  sprintf (stringtemp, "%.2f" , temp); 
  sprintf(horaStr, "%02d:%02d", horaActual, minutoActual);
  u8g2.drawStr(0, 35, "Hora: ");
  u8g2.drawStr(40, 35, horaStr);  
  u8g2.drawStr(0, 50, "T. Actual:");
  u8g2.drawStr(60, 50, stringtemp);
  u8g2.drawStr(90, 50, "°C");
  u8g2.sendBuffer();          
}

void printBMP_OLED2(void) {
  char horaStr[3];
  char minStr[3];
  int horaActual = rtc.getHour(true);  
  int minutoActual = rtc.getMinute();  
  sprintf(horaStr, "%02d", horaActual);
  sprintf(minStr, "%02d", minutoActual);
  u8g2.clearBuffer();         
  u8g2.setFont(u8g2_font_t0_11b_tr); // elegir una fuente adecuada
  u8g2.drawStr(0, 50, horaStr);
  u8g2.drawStr(25, 50, ":");
  u8g2.drawStr(35, 50, minStr);
  u8g2.sendBuffer();          
}
