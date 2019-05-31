//An example of polling multiple circuits using the Arduin Mega's multiple serial ports
//This code works similarly to the serial port expander sample code in terms of the interface
//but constantly polls all the circuits by default
//To open a channel send the number of the channel followed by a colon and the command (if any) that you want to send. End the string with a carriage return.
//For this example, the channels are determined by the module's index in the Modules[] array
//0:r<CR>
//2:i<CR>
//3:c<CR>

//To open a channel and not send a command just send the channel number followed by a colon.
//1:<CR>
//3:<CR>

#include <Ezo_uart.h>
const uint8_t bufferlen = 32;                         //total buffer size for the response_data array
char response_data[bufferlen];                        //character array to hold the response data from modules
String inputstring = "";                              //a string to hold incoming data from the PC

uint8_t index = 0;

// create objects to represent the Modules you're connecting to
// they can accept hardware or software serial ports, and a name of your choice
Ezo_uart Module1(Serial1, "DO");
Ezo_uart Module2(Serial2, "EC");
Ezo_uart Module3(Serial3, "PH");

// In this code the Modules[] array determines the number assigned to the circuit
const uint8_t module_count = 3;                       //total size fo the Modules array
Ezo_uart Modules[module_count] = {                    //create an array to hold all the modules
  Module1, Module2, Module3
};

void setup() {
  Serial.begin(9600);                                 //Set the hardware serial port to 9600
  Serial1.begin(9600);                                //Set the hardware serial port to 9600
  Serial2.begin(9600);                                //Set the hardware serial port to 9600
  Serial3.begin(9600);                                //Set the hardware serial port to 9600
  inputstring.reserve(20);                            //set aside some bytes for receiving data from the PC

  // in order to use multiple circuits more effectively we need to turn off continuous mode and the *ok response
  for (uint8_t i = 0; i < module_count; i++) {        //loop through the modules
    Modules[i].send_cmd_no_resp("c,0");               //send the command to turn off continuous mode
                                                      //in this case we arent concerned about waiting for the reply
    delay(100);
    Modules[i].send_cmd_no_resp("*ok,0");             //send the command to turn off the *ok response
                                                      //in this case we wont get a reply since its been turned off
    delay(100);
    Modules[i].flush_rx_buffer();                     //clear all the characters that we received from the responses of the above commands
  }
}

void loop() {
  if (Serial.available() > 0) {                       //if we get data from the computer
    inputstring = Serial.readStringUntil(13);         //receive it until the carraige return delimiter
    index = parse_input(inputstring);                  //parse the data to either switch ports or send it to the circuit
    
    if (inputstring != "") {                          //if we have a command for the modules
      Modules[index].send_cmd(inputstring, response_data, bufferlen); // send it to the module of the port we opened
      Serial.print(index);                             //print the modules port
      Serial.print("-");
      Serial.print(Modules[index].get_name());     //print the modules name
      Serial.print(": ");
      Serial.println(response_data);                  //print the modules response
      response_data[0] = 0;                           //clear the modules response
    }
    else {
      Serial.print("Index is set to ");                //if were not sending a command, print the port
      Serial.println(index);
    }
  }

  for (uint8_t i = 0; i < module_count; i++) {        //loop through the modules and take a reading
    print_reading(Modules[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void print_reading(Ezo_uart &Module) {                //takes a reference to a Module
  //send_read() sends the read command to the module then converts the 
  //answer to a float which can be retrieved with get_reading()
  //it returns a bool indicating if the reading was obtained successfully
  if (Module.send_read()) {                           
    Serial.print(Module.get_name());                  //prints the module's name
    Serial.print(": ");
    Serial.print(Module.get_reading());               //prints the reading we obtained
  }
}


uint8_t parse_input(String &inputstring) {                 //this function will decode the string (example 4:cal,1413)
  int colon = inputstring.indexOf(':');                  //find the location of the colon in the string
  if ( colon > 0) {                                       //if we found a colon
    String port_as_string = inputstring.substring(0, colon);  //extract the port number from the string and store it here
    inputstring = inputstring.substring(colon + 1);    //extract the message from the string and store it here
    return port_as_string.toInt();                     //convert the port number from a string to an int
  }
  else {                                              //if theres no colon
    return index;                                      //return the current port and dont modify the input string
  }
}
