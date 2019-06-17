/*
MIT License
Copyright (c) 2019 Atlas Scientific
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#ifndef _EZO_UART_H_
#define _EZO_UART_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Ezo_uart{
	public:
	
		//constructors
		Ezo_uart(Stream& Serial_port):Serial_port(Serial_port){}  
		//takes a stream object, which is usually a hardware or software serial port
		
		Ezo_uart(Stream& Serial_port, const char* name):Serial_port(Serial_port), name(name){}
		//takes a stream object, which is usually a hardware or software serial port and a name string
		
		bool send_read();
		//sends the "R" command to the device and parses the response so its
		//accessible as a float with the get_reading() function
		//this function blocks until a reading is received
		
		bool send_read_with_temp_comp(float temperature);
		//sends the "RT" command with the temperature converted to as string
		//and parses the response so its
		//accessible as a float with the get_reading() function
		//this function blocks until a reading is received
		
		void send_cmd_no_resp( const String& cmd);
		//sends a command to the circuit, but doesnt receive the response
		//the reponse can either be later cleared with flush_rx_buffer() or received with
		//receive_cmd(). One can check if theres any data in the response buffer with data_available();
		
		uint8_t send_cmd(const String& cmd, char* sensordata_buffer, const uint8_t buffer_len);
		//sends a command, then blocks until a repsonse. 
		//When the response is received its placed into  char* sensordata_buffer
		
		uint8_t send_cmd_with_num(const char* cmd, char * sensordata_buffer, 
								  const uint8_t buffer_len, float num, uint8_t decimal_amount);
		//sends a command with num appended after as a string, then blocks until a repsonse. 
		//When the response is received its placed into char* sensordata_buffer
								  
		uint8_t receive_cmd(char * sensordata_buffer, const uint8_t buffer_len);
		//waits for a response then returns it in char* sensordata_buffer
		//can be called after send_cmd_no_resp(), or if theres data in the buffer according to data_available()
		//also useful to receive data from a module thats in continuous mode
	
		float get_reading();		
		//returns the last reading the device received as a float

		const char* get_name();		
		//returns a pointer to the name string		
		
		void flush_rx_buffer();
		//discards all the data in the module's serial buffer
		
		uint8_t data_available();
		//a passthrough for the serial port's available() function
		
	private:
		float reading = 0;
		const char* name;
		Stream& Serial_port;
		const static uint8_t bufferlen = 32;
};



#endif
