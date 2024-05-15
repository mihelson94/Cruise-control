#include <Wire.h> 
#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
//Инициализация переменных
Servo servo_dr;  //Серво привод
int speed_imp = 0; //Спидометр, импульсы от датчика скорости
int speed_set = 0; //Установленная скорость
int pos_servo = -180; //Положение серво по умочанию -180, то есть 0
char set_funcp[] = {""}; //Выбранное меню
int pos_accelerator = 0;
bool cruise_flag = false;
bool stop = false;
LiquidCrystal_I2C lcd(0x27,16,2);
//Тексты
const char text0[] ="                ";
const char text1[] ="Cruise: ---";
const char text2[] ="Cruise: ";
const char text3[] =" km/h";
const char text4[] =">";
const char text5[] ="Temp:";
const char text6[] ="|";
const char text7[] =" V";
const char* const msg[] = {text0,text1,text2,text3,text4,text5,text6,text7};
byte celsiy[] = {
  0x1C,
  0x14,
  0x1C,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
//Кнопки
#define btn_up 13 //Вверх или +
#define btn_down 12 //Вниз или -
#define btn_left 11 //Влево
#define btn_right 10 //Вправо
#define BTN_ENTER 9 // Ок или Активация меню
#define btn_brake 8 // Сигнал тормоза или сцепление
int pos_engine_drosel = 0;// Получаем данные он положения газа

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  servo_dr.attach(3);//Инициализируем серво на выходе 3 амнк
  servo_dr.write(pos_servo);//Выравниваем в 0
  lcd.init();
  lcd.backlight();
  lcd.print("Loading...");
  lcd.createChar(0, celsiy);
  lcd.clear();
  pinMode(BTN_ENTER, INPUT_PULLUP);
  pinMode(btn_up, INPUT_PULLUP);
  pinMode(btn_down, INPUT_PULLUP);
  pinMode(btn_left, INPUT_PULLUP);//Включение круиз-контроля
  pinMode(btn_right, INPUT_PULLUP);
  pinMode(btn_brake, INPUT_PULLUP);
  
}

void loop() {
  pos_engine_drosel = analogRead(A3);
  speed_imp = map(analogRead(A2), 0, 1023, 0, 180);
    if(digitalRead(btn_left) == LOW){
          cruise_flag = true;
          speed_set = speed_imp;
          pos_servo = map(pos_engine_drosel, 0, 1023, 0, 180);
    }
    if(digitalRead(btn_brake) == LOW && cruise_flag == true){
          cruise_flag = false;
          reset();
    }
  if(cruise_flag){
    
    
    if(digitalRead(btn_up) == LOW){
          speed_set++;
    }
    if(digitalRead(btn_down) == LOW){
          speed_set--;
    }
    
    int buffer = speed_imp - speed_set;
    buffer = abs(buffer);
    if(buffer > 5){
      if(speed_set > speed_imp ){
        if(pos_servo < 177 && pos_servo > 0){
            pos_servo = pos_servo + 3;
        }  
        }else{
          if(pos_servo > 0 && pos_servo < 177){
            pos_servo = pos_servo - 3;
          }
        }
    }

     Serial.println(pos_servo);

 
    //
    lcd.setCursor(0, 0);
    lcd.print(msg[2]);
    lcd.print(speed_set);
    lcd.print(msg[3]);
    lcd.print(msg[0]);
    
    servo_dr.write(pos_servo);
  }else{
    lcd.setCursor(0, 0);
    lcd.print(msg[1]);
  }

  //temperatura
  lcd.setCursor(0, 1);
  lcd.print(msg[5]);
  lcd.print("+25");
  lcd.write(0);
  lcd.print(msg[6]);
  lcd.print("14.2");
  lcd.print(msg[7]);
  lcd.print(msg[0]);
  delay (100);
}

bool cruise(int status){
    if(status == LOW){
      return true;
    }else{
      return false; 
    }

}

void reset(void){
    servo_dr.write(-180);
    lcd.setCursor(0, 0);
    lcd.print(msg[0]);
    lcd.print(msg[1]);
}