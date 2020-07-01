//automatically polls the listed circuits and sends them temperature compensation

#include <Ezo_uart.h>
#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 2                                          //define what pin rx is going to be
#define tx 3                                          //define what pin tx is going to be
SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

int s1 = 6;                                           //Arduino pin 6 to control pin S1
int s2 = 5;                                           //Arduino pin 5 to control pin S2
int s3 = 4;                                           //Arduino pin 4 to control pin S3
int port = 1;                                         //what port to open

const uint8_t bufferlen = 32;                         //total buffer size for the response_data array
char response_data[bufferlen];                        //character array to hold the response data from modules
String inputstring = "";                              //a string to hold incoming data from the PC

// create objects to represent the Modules you're connecting to
// they can accept hardware or software serial ports, and a name of your choice
Ezo_uart Module1_DO(myserial, "DO");
Ezo_uart Module2_EC(myserial, "EC");
Ezo_uart Module3_PH(myserial, "PH");
Ezo_uart Module4_RTD(myserial, "RTD");

// the modules are ordered in an array according to their position in the serial port expander
// so Modules[0] holds the module in port1, Modules[1] holds the module in port 2, etc
const uint8_t module_count = 4;                       //total size fo the Modules array
Ezo_uart Modules[module_count] = {                    //create an array to hold all the modules
  Module1_DO, Module2_EC, Module3_PH, Module4_RTD
};

void setup() {
  Serial.begin(9600);                                 //Set the hardware serial port to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(20);                            //set aside some bytes for receiving data from the PC
  pinMode(s1, OUTPUT);                                //Set the digital pin as output
  pinMode(s2, OUTPUT);                                //Set the digital pin as output
  pinMode(s3, OUTPUT);                                //Set the digital pin as output

  // in order to use multiple modules more effectively we need to turn off continuous mode and the *ok response
  for (uint8_t i = 0; i < module_count; i++) {        // loop through the modules
    open_port(i + 1);                                 // open the port
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

  float last_temp = 25;                             //variable that holds our temperature, defaults to 25C

  open_port(4);                                     //open the 4th port, which in this case is RTD
  print_reading(Module4_RTD);                       //acquire the reading from the RTD circuit first
  if (Module4_RTD.get_reading() < -1020) {          //if theres no probe attached
    last_temp = 25;                                 //default it to 25C
  } else {
    last_temp = Module4_RTD.get_reading();          //otherwise its the reading from the RTD module
  }

  //open the ports of the modules and send the temperature and get their reading one by one
  Serial.print(" ");
  open_port(1);
  print_reading(Module1_DO, last_temp);
  Serial.print(" ");
  open_port(2);
  print_reading(Module2_EC, last_temp);
  Serial.print(" ");
  open_port(3);
  print_reading(Module3_PH, last_temp);

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

void print_reading(Ezo_uart &Module, float tempcomp) {                //takes a reference to a Module
  //send_read_wth_temp_comp() sends the "RT" command with a temperature to the module
  //then converts the answer to a float which can be retrieved with get_reading()
  //it returns a bool indicating if the reading was obtained successfully
  if (Module.send_read_with_temp_comp(tempcomp)) {
    Serial.print(Module.get_name());                  //prints the module's name
    Serial.print(": ");
    Serial.print(Module.get_reading());               //prints the reading we obtained
  }
}

void open_port(uint8_t _port) {                                  //this function controls what port is opened on the serial port expander

  if (_port < 1 || _port > 8)_port = 1;                //if the value of the port is within range (1-8) then open that port. If it’s not in range set it to port 1
  uint8_t port_bits = _port - 1;

  digitalWrite(s1, bitRead(port_bits, 0));               //Here we have two commands combined into one.
  digitalWrite(s2, bitRead(port_bits, 1));               //The digitalWrite command sets a pin to 1/0 (high or low)
  digitalWrite(s3, bitRead(port_bits, 2));               //The bitRead command tells us what the bit value is for a specific bit location of a number
  delay(2);                                         //this is needed to make sure the channel switching event has completed
}
