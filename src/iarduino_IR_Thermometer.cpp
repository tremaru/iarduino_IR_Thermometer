#include "iarduino_IR_Thermometer.h"
/**	пользовательские функции **/

//			инициализация датчика
int			iarduino_IR_Thermometer::IRT_begin(int scl){					//
//				Выключаем ШИМ на выходе датчика								//
				if( scl ){													//
					pinMode		(scl, OUTPUT);	delay(100);					//	Переводим вывод scl  шины I2C в режим выхода и ожидаем звершение переходных процессов в датчике, связанных с подачей питания
					digitalWrite(scl, HIGH);	delay(1);					//	Устанавливаем 1 на выводе scl в течении 1мс
					digitalWrite(scl, LOW);		delay(3);					//	Устанавливаем 0 на выводе scl в течении 3мс // Выключаем режим ШИМ, если таковой был включён
					digitalWrite(scl, HIGH);	delay(1);					//	Устанавливаем 1 на выводе scl в течении 1мс
//					Переинициируем шину I2C									//
					selI2C->begin();										//	Переинициируем шину I2C.
				}															//
//				читаем состояние регистра настроек ШИМ датчика
				IRT_data_COM=IRT_code_ROM|IRT_addr_PWM;						//	Собираем команду из кода доступа к ПЗУ и адреса ячейки с указанием настроек ШИМ
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				if( !I2C_func_READ()){return IRT_NO_REPLY;}					//	Если произошла ошибка при чтении, то возвращаем ошибку IRT_NO_REPLY
				if( IRT_data_CRC!=IRT_func_CRC8(1) ){return IRT_CHECKSUM;}	//	Если не CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
//				меняем состояние регистра настроек ШИМ датчика				//
				if(IRT_data_LSB&0x06){										//	Если установлены 2 или 1 биты регистра настроек ШИМ, то сбрасываем их
					IRT_data_LSB&=0xF9;										//
					if( !I2C_func_WRITE() ){return IRT_NO_REPLY;}			//	Если произошла ошибка при записи, то возвращаем ошибку IRT_NO_REPLY
				}															//
				return IRT_OK;												//
}

int			iarduino_IR_Thermometer::read(){								//	Читаем показания датчика
//				читаем температуру объекта
				IRT_data_COM=IRT_code_RAM|IRT_addr_To1;						//	Собираем команду из кода доступа к ОЗУ и адреса ячейки с результатом вычисления температуры объекта (1 датчик)
				if( !I2C_func_READ()){return IRT_NO_REPLY;}					//	Отправляем команду и читаем данные из датчика по шине I2C
				if(IRT_data_CRC!=IRT_func_CRC8(1)){return IRT_CHECKSUM;}	//	Если CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
				temp = (double)(((IRT_data_MSB&0x7F)<<8)+IRT_data_LSB);		//	Сохраняем старший и младштй байт данных о температуре объекта
				temp = (temp*0.02)-273.15;									//	Корректируем данные о температуре объекта и переводим из °K в °С
//				читаем температуру окружающей среды
				IRT_data_COM=IRT_code_RAM|IRT_addr_Ta;						//	Собираем команду из кода доступа к ОЗУ и адреса ячейки с результатом вычисления температуры окружающей среды
				if( !I2C_func_READ()){return IRT_NO_REPLY;}					//	Отправляем команду и читаем данные из датчика по шине I2C
				if(IRT_data_CRC!=IRT_func_CRC8(1)){return IRT_CHECKSUM;}	//	Если CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
				tempA = (double)(((IRT_data_MSB&0x7F)<<8)+IRT_data_LSB);	//	Сохраняем старший и младштй байт данных о температуре окружающей среды
				tempA = (tempA*0.02)-273.15;								//	Корректируем данные о температуре окружающей среды и переводим из °K в °С
				return IRT_OK;
}

int			iarduino_IR_Thermometer::newID(uint8_t a){						//	Установка нового адреса на шине
//				читаем текущий адрес датчика на шине
				IRT_data_COM=IRT_code_ROM|IRT_addr_ID;						//	Собираем команду из кода доступа к ПЗУ и адреса ячейки с указанием ID адреса датчика на шине I2C
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				IRT_data_MSB=0; IRT_data_LSB=a;								//
				if( !I2C_func_WRITE() ){return IRT_NO_REPLY;}				//	Отправляем новый адрес датчику
				IRT_uint_ADDRESS=a;											//	Сохраняем новый адрес для общения с датчиком
				return IRT_OK;
}

uint16_t	iarduino_IR_Thermometer::serial(uint8_t a){						//	Получение сериного номера датчика
				switch(a){													//	Указываем команду доступа к ячейке с указанием части серийного номера датчика
					case 0: IRT_data_COM = IRT_addr_SERIAL0; break;
					case 1: IRT_data_COM = IRT_addr_SERIAL1; break;
					case 2: IRT_data_COM = IRT_addr_SERIAL2; break;
					case 3: IRT_data_COM = IRT_addr_SERIAL3; break;
				}			IRT_data_COM|= IRT_code_ROM; I2C_func_READ();	//	Добавляем к команде кода доступа к ПЗУ, отправляем её и читаем данные из датчика по шине I2C
				return (int)((IRT_data_MSB<<8)+IRT_data_LSB);				//	Возвращаем часть серийного номера датчика
}

/** внутренняя функция для определения CRC8 (X8+X2+X1+1) **/
uint8_t		iarduino_IR_Thermometer::IRT_func_CRC8(bool a){					//	a=0-запись / a=1-чтение
			uint8_t data[5];												//	создаем массив данных для которых сформируется CRC
					data[0]=	IRT_uint_ADDRESS<<1;						//	
					data[1]=	IRT_data_COM;								//	
					data[2]=a?((IRT_uint_ADDRESS<<1)+1)	:IRT_data_LSB;		//	
					data[3]=a?	IRT_data_LSB			:IRT_data_MSB;		//	
					data[4]=a?	IRT_data_MSB			:0;					//	
			uint8_t crc=0x00;												//	присваиваем CRC8 начальное значение				(для CRC8 X8+X2+X1+1 - значение 0x00)
			for(uint8_t i=0; i<(a?5:4); i++){								//	проходим по всем байтам данных из массива data	(для CRC8 X8+X2+X1+1 - MSB)
				crc^=(data[i]);												//  выполняем операцию XOR между CRC8 и очередным байтом массива data
				for(uint8_t j=0; j<8; j++){									//  проходим по всем битам байта CRC8
					if(crc & 0x80){crc<<=1; crc^=0x07;}else{crc<<=1;}		//	сдвигаем CRC8 на 1 бит (для CRC8 X8+X2+X1+1 - сдвигаем влево) и если вытесненный бит был равен 1, то выполняем операцию XOR между CRC8 и полиномом (для CRC8 X8+X2+X1+1 - полином 0x07)
				}
			}	return crc;
}

/** внутренние функции для работы с шиной I2C **/

bool		iarduino_IR_Thermometer::I2C_func_READ(void){					//	Читаем показания датчика
				bool	result=false;										//	Определяем флаг       для хранения результата чтения.
				uint8_t	sumtry=10;											//	Определяем переменную для подсчёта количества оставшихся попыток чтения.
				uint8_t	data[3];											//	Объявляем  массив     для хранения получаемых данных.
			do{	result = selI2C->readBytes(IRT_uint_ADDRESS, IRT_data_COM, data, 3);	//	Отправляем модулю IRT_uint_ADDRESS комаду(адрес регистра) IRT_data_COM и считываем 3 байта в массив data.
				sumtry--;	if(!result){delay(1);}							//	Уменьшаем количество попыток чтения и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);							//	Повторяем чтение если оно завершилось неудачей, но не более sumtry попыток.
				IRT_data_LSB=data[0];										//	Сохраняем младший байт.
				IRT_data_MSB=data[1];										//	Сохраняем старший байт.
				IRT_data_CRC=data[2];										//	Сохраняем байт CRC.
				delayMicroseconds(500);										//	Между пакетами необходимо выдерживать паузу.
			return result;													//	Возвращаем результат чтения (true/false).
}																			//

bool	iarduino_IR_Thermometer::I2C_func_WRITE(void){						//	Записываем данные в датчик
			bool	result=false;											//	Определяем флаг       для хранения результата записи.
			uint8_t	sumtry=10;												//	Определяем переменную для подсчёта количества оставшихся попыток записи.
			uint8_t	data[3]={IRT_data_LSB,IRT_data_MSB,IRT_func_CRC8(0)};	//	Объявляем  массив для передачи данных.
			do{	result = selI2C->writeBytes(IRT_uint_ADDRESS, IRT_data_COM, data, 3);	//	Записываем в модуль valAddr начиная с регистра reg, sum байи из массива data начиная с элемента num.
				sumtry--;	if(!result){delay(1);}							//	Уменьшаем количество попыток записи и устанавливаем задержку при неудаче.
			}	while		(!result && sumtry>0);							//	Повторяем запись если она завершилась неудачей, но не более sumtry попыток.
			delay(10);														//	Ждём применения модулем записанных данных.
			return result;													//	Возвращаем результат записи (true/false).
}																			//
