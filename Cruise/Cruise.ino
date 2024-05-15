#include <Wire.h> 
#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>


Servo drosel;  // Дросель
int potpin = 3;  // Первый температурный датчик
int val;    // variable to read the value from the analog pin
int speed_now = 0;    // variable to read the value from the analog pin
int speed_set = 0;
int cruise_set = 0;
const float BETA = 3950;
LiquidCrystal_I2C lcd(0x27,16,2);

int btn_click = 0;
#define btn_up 13 //Вверх или +
#define btn_down 12 //Вниз или -
#define btn_left 11 //Влево
#define btn_right 10 //Вправо
#define BTN_ENTER 9 // Ок или Активация круиза
#define btn_break 8 // Сигнал тормоза
int default_serv = 0; // Серво на 0
bool cruise = false; // Статус круиза
bool brake = false; // Статус тормоза

void setup() {
  drosel.write(-180);// По умолчанию 0 
  lcd.begin(16, 2);
  drosel.attach(3); // инициализация на пин 7 серево привода заслонки
  pinMode(A3, INPUT);
  Serial.begin(9600); 
  lcd.backlight();
  lcd.init();
  lcd.begin(16, 2);
  lcd.print("Hello World!");
  lcd.clear();
  EEPROM.put(0, default_serv);
  EEPROM.put(1, 0);
  EEPROM.put(010, 0); //menu = 1
  pinMode(BTN_ENTER, INPUT_PULLUP);
  pinMode(btn_up, INPUT_PULLUP);
  pinMode(btn_down, INPUT_PULLUP);
  pinMode(btn_left, INPUT_PULLUP);
  pinMode(btn_right, INPUT_PULLUP);
  pinMode(btn_break, INPUT_PULLUP);

}


void loop() {
int engine_drosel = analogRead(A3);
speed_set = speed_btn(digitalRead(btn_up),digitalRead(btn_down), speed_set);
cruise = option_set(digitalRead(BTN_ENTER),digitalRead(btn_break));
//Serial.println(speed_now);
lcd.setCursor(0, 0);
lcd.print("Cruise: ");
speed_now = map(engine_drosel, 0, 1023, 0, 180); 
Serial.println(speed_now);
if(cruise == true && digitalRead(btn_break) == HIGH){
  lcd.print("ON ");
  int buffer = speed_now - speed_set;
 
  if(speed_now != speed_set && abs(buffer) > 2){
      if(speed_now > speed_set){
        if(buffer > 5){ cruise_set = cruise_set - 5;}else{cruise_set--;}
      }else{
        cruise_set++;
      }
  }
  lcd.setCursor(0, 1);
  lcd.print("Speed: ");
    if(strlen(speed_set) < 3){
      lcd.print(speed_set);
      lcd.print(" km/h");
      lcd.print("  ");
    }else{
      lcd.print(speed_set);
      lcd.print(" km/h");
      lcd.print(" ");
    }
  Serial.println(speed_set);
  drosel.write(cruise_set);
  
}else{
  drosel.write(-180);
  lcd.print("OFF");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  speed_set = speed_now;
  cruise_set = speed_now;
}

  delay(100);
}

int option_set(int enter,int left){
    if(left == LOW){
      cruise = false;
    }

    if(enter == LOW || cruise == true){
      cruise = true;
    }
    return(cruise);
}

int speed_btn(int up,int down, int speed_set){
    if(up == LOW){
      speed_set++;
    }

    if(down == LOW){
      speed_set--;
    }
   
    return(speed_set);
}