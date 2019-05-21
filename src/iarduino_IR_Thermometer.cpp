#include "iarduino_IR_Thermometer.h"
/**	пользовательские функции **/
			iarduino_IR_Thermometer::iarduino_IR_Thermometer(uint8_t i){	//	объявление экземпляра объекта с указанием адреса датчика на шине I2C
				if(i){IRT_uint_ADDRESS=i;}									//	сохраняем адрес датчика на шине I2C
			}

//			инициализация датчика
int			iarduino_IR_Thermometer::begin(){
//				выключаем ШИМ на выходе датчика
				pinMode		(SCL, OUTPUT);	delay(100);						//	Переводим вывод SCL  шины I2C в режим выхода и ожидаем звершение переходных процессов в датчике, связанных с подачей питания
				digitalWrite(SCL, HIGH);	delay(1);						//	Устанавливаем 1 на выводе SCL в течении 1мс
				digitalWrite(SCL, LOW);		delay(3);						//	Устанавливаем 0 на выводе SCL в течении 3мс // Выключаем режим ШИМ, если таковой был включён
				digitalWrite(SCL, HIGH);	delay(1);						//	Устанавливаем 1 на выводе SCL в течении 1мс
				I2C_func_BEGIN();											//	Инициируем работу по шине I2C
//				читаем состояние регистра настроек ШИМ датчика
				IRT_data_COM=IRT_code_ROM|IRT_addr_PWM;						//	Собираем команду из кода доступа к ПЗУ и адреса ячейки с указанием настроек ШИМ
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				I2C_func_READ();											//	Повторно отправляем ту же команду, т.к. при самой первой команде, датчик отправляет карявый CRC
				if(!I2C_mass_STATUS[2]){return IRT_NO_REPLY;}				//	Если произошла ошибка при чтении, то возвращаем ошибку IRT_NO_REPLY
				if(IRT_data_CRC!=IRT_func_CRC8(1)){return IRT_CHECKSUM;}	//	Если не CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
//				меняем состояние регистра настроек ШИМ датчика
				if(IRT_data_LSB&0x06){IRT_data_LSB&=0xF9;I2C_func_WRITE();}	//	Если установлены 2 или 1 биты регистра настроек ШИМ (вывод SDA сконфигурирован как двухтактный, или включён режим ШИМ), то сбрасываем их
				if(!I2C_mass_STATUS[2]){return IRT_NO_REPLY;}				//	Если произошла ошибка при записи, то возвращаем ошибку IRT_NO_REPLY
				return IRT_OK;
}

int			iarduino_IR_Thermometer::read(){								//	Читаем показания датчика
//				читаем температуру объекта
				IRT_data_COM=IRT_code_RAM|IRT_addr_To1;						//	Собираем команду из кода доступа к ОЗУ и адреса ячейки с результатом вычисления температуры объекта (1 датчик)
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				if(!I2C_mass_STATUS[2]){return IRT_NO_REPLY;}				//	Если произошла ошибка при чтении, то возвращаем ошибку IRT_NO_REPLY
				if(IRT_data_CRC!=IRT_func_CRC8(1)){return IRT_CHECKSUM;}	//	Если CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
				temp = (double)(((IRT_data_MSB&0x7F)<<8)+IRT_data_LSB);		//	Сохраняем старший и младштй байт данных о температуре объекта
				temp = (temp*0.02)-273.15;									//	Корректируем данные о температуре объекта и переводим из °K в °С
//				читаем температуру окружающей среды
				IRT_data_COM=IRT_code_RAM|IRT_addr_Ta;						//	Собираем команду из кода доступа к ОЗУ и адреса ячейки с результатом вычисления температуры окружающей среды
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				if(!I2C_mass_STATUS[2]){return IRT_NO_REPLY;}				//	Если произошла ошибка при чтении, то возвращаем ошибку IRT_NO_REPLY
				if(IRT_data_CRC!=IRT_func_CRC8(1)){return IRT_CHECKSUM;}	//	Если CRC не совпадает с рассчитанным, то возвращаем ошибку IRT_CHECKSUM
				tempA = (double)(((IRT_data_MSB&0x7F)<<8)+IRT_data_LSB);	//	Сохраняем старший и младштй байт данных о температуре окружающей среды
				tempA = (tempA*0.02)-273.15;								//	Корректируем данные о температуре окружающей среды и переводим из °K в °С
				return IRT_OK;
}

int			iarduino_IR_Thermometer::newID(uint8_t a){						//	Установка нового адреса на шине
//				читаем текущий адрес датчика на шине
				IRT_data_COM=IRT_code_ROM|IRT_addr_ID;						//	Собираем команду из кода доступа к ПЗУ и адреса ячейки с указанием ID адреса датчика на шине I2C
				I2C_func_READ();											//	Отправляем команду и читаем данные из датчика по шине I2C
				IRT_data_MSB=0; IRT_data_LSB=a; I2C_func_WRITE();			//	Отправляем новый адрес датчику
				if(!I2C_mass_STATUS[2]){return IRT_NO_REPLY;}				//	Если произошла ошибка при записи, то возвращаем ошибку IRT_NO_REPLY
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
void		iarduino_IR_Thermometer::I2C_func_START			()												{int I2C_var_I=0;    I2C_mass_STATUS[2]=1; TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA); while(!(TWCR & _BV(TWINT))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} I2C_mass_STATUS[3] = TWSR & 0xF8; if(I2C_mass_STATUS[3]!=0x08){I2C_mass_STATUS[2]=0;}}
void		iarduino_IR_Thermometer::I2C_func_RESTART		()												{int I2C_var_I=0; if(I2C_mass_STATUS[2]){  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA); while(!(TWCR & _BV(TWINT))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} I2C_mass_STATUS[3] = TWSR & 0xF8; if(I2C_mass_STATUS[3]!=0x10){I2C_mass_STATUS[2]=0;}}}
void		iarduino_IR_Thermometer::I2C_func_SEND_ID		(uint8_t I2C_byte_ID, bool I2C_bit_RW)			{int I2C_var_I=0; if(I2C_mass_STATUS[2]){  TWDR = (I2C_byte_ID<<1)+I2C_bit_RW; TWCR = _BV(TWINT) | _BV(TWEN); while(!(TWCR & _BV(TWINT))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} I2C_mass_STATUS[3] = TWSR & 0xF8; if(I2C_bit_RW){ if(I2C_mass_STATUS[3]!=0x40){I2C_mass_STATUS[2]=0;}}else{if(I2C_mass_STATUS[3]!=0x18){I2C_mass_STATUS[2]=0;}}}}
void		iarduino_IR_Thermometer::I2C_func_WRITE_BYTE	(uint8_t I2C_byte_DATA)							{int I2C_var_I=0; if(I2C_mass_STATUS[2]){  TWDR = I2C_byte_DATA; TWCR = _BV(TWINT) | _BV(TWEN); while(!(TWCR & _BV(TWINT))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} I2C_mass_STATUS[3] = TWSR & 0xF8; if(I2C_mass_STATUS[3]!=0x28){I2C_mass_STATUS[2]=0;}}}
uint8_t		iarduino_IR_Thermometer::I2C_func_READ_BYTE		(bool I2C_bit_ACK)								{int I2C_var_I=0; if(I2C_mass_STATUS[2]){  TWCR = _BV(TWINT) | _BV(TWEN) | I2C_bit_ACK<<TWEA; while(!(TWCR & _BV(TWINT))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} I2C_mass_STATUS[3] = TWSR & 0xF8; if(I2C_bit_ACK){if(I2C_mass_STATUS[3]!=0x50){I2C_mass_STATUS[2]=0;}}else{if(I2C_mass_STATUS[3]!=0x58){I2C_mass_STATUS[2]=0;}} return TWDR;}else{return 0xFF;}}
void		iarduino_IR_Thermometer::I2C_func_STOP			()												{int I2C_var_I=0; TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); while(!(TWCR & _BV(TWSTO))){I2C_var_I++; if(I2C_var_I>I2C_mass_STATUS[1]){I2C_mass_STATUS[2]=0; break;}} delayMicroseconds(20);}
void		iarduino_IR_Thermometer::I2C_func_BEGIN			()												{    I2C_mass_STATUS[2]=1; TWBR=((F_CPU/(I2C_mass_STATUS[0]*1000))-16)/2; if(TWBR<10){TWBR=10;} TWSR&=(~(_BV(TWPS1)|_BV(TWPS0)));}

void		iarduino_IR_Thermometer::I2C_func_READ(){						//	Читаем показания датчика
				I2C_func_START();											//	Отправляем команду START
				I2C_func_SEND_ID(IRT_uint_ADDRESS, 0);						//	Отправляем адрес модуля и бит RW=0 (запись данных в датчик)
				I2C_func_WRITE_BYTE(IRT_data_COM);							//	Отправляем данные команды из IRT_data_COM
				I2C_func_RESTART();											//	Отправляем команду RESTART
				I2C_func_SEND_ID(IRT_uint_ADDRESS, 1);						//	Отправляем адрес модуля и бит RW=1 (чтение данных из датчика)
				IRT_data_LSB=I2C_func_READ_BYTE(true);						//	Читаем младший байт из регистра с отправкой бита ACK
				IRT_data_MSB=I2C_func_READ_BYTE(true);						//	Читаем старший байт из регистра с отправкой бита ACK
				IRT_data_CRC=I2C_func_READ_BYTE(false);						//	Читаем байт циклически изб.кода с отправкой бита NACK
				I2C_func_STOP();											//	Отправляем команду STOP
}
void		iarduino_IR_Thermometer::I2C_func_WRITE(){						//	Записываем данные в датчик
				I2C_func_START();											//	Отправляем команду START
				I2C_func_SEND_ID(IRT_uint_ADDRESS, 0);						//	Отправляем адрес модуля и бит RW=0 (запись данных в датчик)
				I2C_func_WRITE_BYTE(IRT_data_COM);							//	Отправляем данные команды из IRT_data_COM
				I2C_func_WRITE_BYTE(IRT_data_LSB);							//	Отправляем младший байт данных
				I2C_func_WRITE_BYTE(IRT_data_MSB);							//	Отправляем старший байт данных
				I2C_func_WRITE_BYTE(IRT_func_CRC8(0));						//	Отправляем циклически избыточный код
				I2C_func_STOP();											//	Отправляем команду STOP
}
