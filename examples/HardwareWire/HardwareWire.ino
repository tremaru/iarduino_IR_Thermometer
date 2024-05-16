// ПРИМЕР ВЫВОДА ТЕМПЕРАТУРЫ:                                                   //
                                                                                //
// ИК датчик температуры I2C:                                                   // https://iarduino.ru/shop/
// Информация о подключении модулей к шине I2C:                                 // https://wiki.iarduino.ru/page/i2c_connection/
// Информация о модуле и описание библиотеки:                                   // https://wiki.iarduino.ru/
                                                                                //
#include <Wire.h>                                                               // Подключаем библиотеку для работы с аппаратной шиной I2C, до подключения библиотеки iarduino_IR_Thermometer.
#include <iarduino_IR_Thermometer.h>                                            // Подключаем библиотеку для работы с ИК датчиком температуры
iarduino_IR_Thermometer sensor;                                                 // Объявляем объект sensor для работы с ИК датчиком температуры. Можно указать адрес датчика на шине I2C: iarduino_IR_Thermometer sensor(0x5A);
                                                                                //
void setup(){                                                                   //
     Serial.begin(9600);                                                        // Инициализируем передачу данных по UART на скорости 9600 бит/сек
     sensor.begin(&Wire); // &Wire1, &Wire2 ...                                 // Инициируем работу с ИК датчиком температуры, указав ссылку на объект для работы с шиной I2C на которой находится датчик.
}                                                                               // Объект основной аппаратной шины I2C (Wire) можно не указывать: sensor.begin();
                                                                                //
void loop(){                                                                    //
     switch(sensor.read()){                                                     // Читаем показания датчика:
         case IRT_OK:       Serial.print  ("CEHCOP: ");                         //
                            Serial.print  ((String) "To="+sensor.temp +"*C, "); // Выводим температуру объектов.
                            Serial.print  ((String) "Ta="+sensor.tempA+"*C"  ); // Выводим температуру окружающей среды.
                            Serial.println();                   break;          //
         case IRT_CHECKSUM: Serial.println("Не равенство CRC"); break;          //
         case IRT_NO_REPLY: Serial.println("Нет ответа");       break;          //
         default:           Serial.println("Error");            break;          //
     }                                                                          //
     delay(200);                                                                //
}                                                                               //