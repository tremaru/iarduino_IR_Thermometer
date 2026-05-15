// ПРИМЕР ВЫВОДА ТЕМПЕРАТУРЫ:                                                   //
// Требуется установить библиотеку <iarduino_I2C_Software.h>                    // https://iarduino.ru/file/627.html
                                                                                //
// ИК датчик температуры I2C:                                                   // https://iarduino.ru/shop/
// Информация о подключении модулей к шине I2C:                                 // https://wiki.iarduino.ru/page/i2c_connection/
// Информация о модуле и описание библиотеки:                                   // https://wiki.iarduino.ru/
                                                                                //
#include <iarduino_I2C_Software.h>                                              // Подключаем библиотеку для работы с программной шиной I2C, до подключения библиотеки iarduino_IR_Thermometer.
SoftTwoWire sWire(3,4);                                                         // Создаём объект программной шины I2C указав выводы которым будет назначена роль линий: SDA, SCL.
                                                                                //
#include <iarduino_IR_Thermometer.h>                                            // Подключаем библиотеку для работы с ИК датчиком температуры
iarduino_IR_Thermometer sensor;                                                 // Создаём объект sensor для работы с ИК датчиком температуры. Можно указать адрес датчика на шине I2C: iarduino_IR_Thermometer sensor(0x5A);
                                                                                //
void setup(){                                                                   //
     Serial.begin(9600);                                                        // Инициализируем передачу данных по UART на скорости 9600 бит/сек
     sensor.begin(&sWire);                                                      // Инициируем работу с ИК датчиком температуры, указав ссылку на объект для работы с шиной I2C на которой находится датчик (по умолчанию &Wire).
}                                                                               //
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