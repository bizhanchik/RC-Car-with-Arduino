#include <IBusBM.h> // Подключение библиотеки IBus для работы с приемником

// Пины для управления левым двигателем
#define pwmA 3
#define in1A 5
#define in2A 4

// Пины для управления правым двигателем
#define pwmB 9
#define in1B 7
#define in2B 8

// Пин для управления реле (оружие)
const int relay1 = 11;

// Объект IBus для работы с приемником
IBusBM IBus;

// Переменные для значений каналов
int CH1 = 0; // Влево/вправо
int CH2 = 0; // Вперед/назад
int CH5 = 0; // Управление оружием

// Переменные для управления моторами
int MotorSpeedA = 0;
int MotorSpeedB = 0;
int MotorDirA = 1;
int MotorDirB = 1;

// Функция для управления левым двигателем
void mControlA(int speed, int direction) {
  if (direction == 0) {
    digitalWrite(in1A, LOW);
    digitalWrite(in2A, HIGH);  // Назад
  } else {
    digitalWrite(in1A, HIGH);
    digitalWrite(in2A, LOW);   // Вперед
  }
  analogWrite(pwmA, speed);     // Устанавливаем скорость
}

// Функция для управления правым двигателем
void mControlB(int speed, int direction) {
  if (direction == 0) {
    digitalWrite(in1B, LOW);
    digitalWrite(in2B, HIGH);  // Назад
  } else {
    digitalWrite(in1B, HIGH);
    digitalWrite(in2B, LOW);   // Вперед
  }
  analogWrite(pwmB, speed);    // Устанавливаем скорость
}

// Чтение значения канала и преобразование в диапазон от min до max
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t CH = IBus.readChannel(channelInput);
  if (CH < 100) return defaultValue;  // Если канал неактивен, вернуть значение по умолчанию
  return map(CH, 1000, 2000, minLimit, maxLimit);  // Преобразование сигнала
}

void setup() {
  Serial.begin(115200);
  IBus.begin(Serial);

  // Настраиваем пины для двигателей и реле
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(in1A, OUTPUT);
  pinMode(in2A, OUTPUT);
  pinMode(in1B, OUTPUT);
  pinMode(in2B, OUTPUT);
  pinMode(relay1, OUTPUT);
}

void loop() {
  // Чтение значений каналов с приемника Flysky
  CH1 = readChannel(0, -100, 100, 0);  // Управление влево/вправо
  CH2 = readChannel(1, -255, 255, 0);  // Управление вперед/назад
  CH5 = readChannel(4, 0, 1, 0);       // Управление оружием (CH5)

  // Управление реле (оружием)
  if (CH5 == 1) {
    digitalWrite(relay1, HIGH);  // Включить оружие
  } else {
    digitalWrite(relay1, LOW);   // Выключить оружие
  }

  // Определение направления движения (вперед/назад)
  if (CH2 >= 0) {
    MotorDirA = 1;  // Вперед
    MotorDirB = 1;
  } else {
    MotorDirA = 0;  // Назад
    MotorDirB = 0;
  }

  // Управление скоростью двигателей
  MotorSpeedA = abs(CH2);  // Скорость зависит от положения джойстика вперед/назад
  MotorSpeedB = abs(CH2);

  // Управление поворотами (влево/вправо)
  // Для танковой системы, левый и правый мотор управляются независимо
  MotorSpeedA -= CH1;
  MotorSpeedB += CH1; 

  MotorSpeedA = constrain(MotorSpeedA, 0, 255);
  MotorSpeedB = constrain(MotorSpeedB, 0, 255);

  // Управление двигателями
  mControlA(MotorSpeedA, MotorDirA);
  mControlB(MotorSpeedB, MotorDirB);

  delay(50);
}