// Библиотека iarduino_IR_Thermometer для работы с ИК датчиком температуры http://iarduino.ru
// Считывание данных осуществляется с ИК датчика температуры               http://iarduino.ru

#include <iarduino_IR_Thermometer.h>                  // подключаем библиотеку для работы с ИК датчиком температуры
iarduino_IR_Thermometer  sensor;                      // объявляем объект sensor для работы с ИК датчиком температуры (можно указать адрес датчика на шине I2C)

void setup(){
  Serial.begin(9600);                                 // инициализируем передачу данных по UART на скорости 9600 бит/сек
  sensor.begin();                                     // инициализируем ИК датчик температуры
}

void loop(){
  switch(sensor.read()){                              // читаем показания датчика
    case IRT_OK:         Serial.println((String) "CEHCOP: To=" + sensor.temp + "*C, Ta=" + sensor.tempA + "*C");  break;
    case IRT_CHECKSUM:   Serial.println(         "CEHCOP: HE PABEHCTBO CRC");                                     break;
    case IRT_NO_REPLY:   Serial.println(         "CEHCOP: HET OTBETA");                                           break;
    default:             Serial.println(         "CEHCOP: ERROR");                                                break;
  }
  delay(200);
}

/* 
 *  sensor.temp  - хранит температуру объектов
 *  sensor.tempA - хранит температуру огружающей среды
 *  
 */
