#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Ezo_uart.h>   


const char* Ezo_uart::get_name(){
	return this->name;
}

void Ezo_uart::flush_rx_buffer(){
	while(this->Serial_port.available() > 0){
		this->Serial_port.read();
	}
}

uint8_t Ezo_uart::data_available(){
	return this->Serial_port.available();
}

bool Ezo_uart::send_read(){
	char _sensordata[this->bufferlen];
	if(send_cmd("r", _sensordata, this->bufferlen)){
		if(strcmp(_sensordata, "*ER") != 0){
			this->reading = atof(_sensordata);
			return true;
		}
	}
	return false;
}

bool Ezo_uart::send_read_with_temp_comp(float temperature){
	//send_cmd_with_num("rt,", temperature, 3);
	char _sensordata[this->bufferlen];
	if(send_cmd_with_num("rt,", _sensordata, this->bufferlen, temperature, 3)){
		if(strcmp(_sensordata, "*ER") != 0){
			this->reading = atof(_sensordata);
			return true;
		}
	}
	return false;
}

float Ezo_uart::get_reading(){
	return this->reading;
}

void Ezo_uart::send_cmd_no_resp( const String& cmd){
	this->Serial_port.print(cmd + "\r");
}

uint8_t Ezo_uart::send_cmd(const String& cmd, char * sensordata_buffer, uint8_t buffer_len) {
  this->Serial_port.print(cmd + "\r");
  for(uint16_t i = 0; i < 100; i++){
    if(this->Serial_port.available() > 0){
      return receive_cmd(sensordata_buffer, buffer_len);
    }
    delay(10);
  }
  return 0;
}

uint8_t  Ezo_uart::send_cmd_with_num(const char* cmd, char * sensordata_buffer, 
										const uint8_t buffer_len, float num, uint8_t decimal_amount){
	String temp = String(cmd )+ String(num, (unsigned int)decimal_amount);
	const char* pointer = temp.c_str();
	return send_cmd(pointer,sensordata_buffer,buffer_len);
}

uint8_t Ezo_uart::receive_cmd(char * sensordata_buffer, const uint8_t buffer_len) {
	if (buffer_len == 0) return 0;
	
	uint8_t num_received =  this->Serial_port.readBytesUntil('\r', sensordata_buffer, buffer_len-1);
	sensordata_buffer[num_received] = 0; //null terminate the string!
	return num_received;
}
