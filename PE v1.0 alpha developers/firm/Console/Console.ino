/*
 * Creator: Samuel Salazar Ramírez      samuel.salazar@besoft.com.mx
 * Company: BeSoftCo
 * Oficial Site: https://www.besoft.com.mx/
 * Last modification date: July 28th, 2017.
 */

#include <string.h>

// Declares serial variables
int SERIAL_BAUDRATE = 9600;
int receivedByte;
bool isReceivingData;
String receivedData;

class ArduinoController 
{
  private:
    int commandIndex;
    int commandLenght;
    String command;
    String subCommand;

    //Declares Arduino numeration ports. This is the Arduino MEGA settings.
    int MIN_DIGITAL_PORT = 22;
    int MAX_DIGITAL_PORT = 53;

    int MIN_ANALOG_PORT = 0;
    int MAX_ANALOG_PORT = 15;

    int MIN_PWM_PORT = 2;
    int MAX_PWM_PORT = 13;
    
  public:
    ArduinoController()
    {
      this->command = "";
      commandIndex = 0;
      commandLenght = 0;
    }
    
    void setCommand(String command)
    { 
      commandLenght = command.length();
      this->command = command;
    }

    //This is the first state to receive data.
    void qPlay()
    {
      commandIndex = 0;
      getSubCommand(true);
      
      if(subCommand == "ABOUT") 
        qAbout();
      else if(subCommand == "FLUSH") 
        qFlush();
      else if(subCommand == "PWM")
        qPWM();
      else if(subCommand == "DIGITAL")
        qDigital();
      else if(subCommand == "ANALOG")
        qAnalog();
      else if(subCommand == "COMMANDLIST")
        qCommandList();
      else if(subCommand == "SETGLOBAL")
        qSetGlobal();
      else
        errUndefined();
    }

    //This is the state to generate "About" screen
    void qAbout(){
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Describes the basic info about the firmware and microcontroller serial port.");
      else
        //This info can be parsed by ArduinoBaseController class
        Serial.println("Arduino MEGA 2560|v1.0 Alpha 1|BeSoftCo|2017|samuel.salazar@besoft.com.mx");
    }

    //This state flushes the serial data buffered in SerialPort
    void qFlush(){
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Cleans the serial buffer.");
      else
        Serial.flush();
    }

    //This state sets a PWM value
    void qPWM()
    {
      getSubCommand(true);
      if(subCommand == "SET")
      {
        getSubCommand(false);
        int pwmPort = subCommand.toInt();
        getSubCommand(false);
        int pwmValue = subCommand.toInt();
        if(pwmValue >= 0 && pwmValue <= 1023)
          analogWrite(pwmPort,pwmValue);
        else 
          errOutOfIndex("PWM",pwmPort,pwmValue);
      }
      else if(subCommand == "HELP") 
      {
        Serial.println("SET, changes the value of an specific port.");
      }
      else
      {
        errUndefined();
      }
    }

    //This is the digital state
    void qDigital(){
      int digitalPort = 0;
      bool value = false;
      getSubCommand(true);
      //Configures pinmode
      if(subCommand == "PINMODE" )
      {
        getSubCommand(false);
        digitalPort = subCommand.toInt();
        getSubCommand(true);
        if(subCommand == "INPUT")
        {
          pinMode(digitalPort,INPUT);
        }
        else if(subCommand == "OUTPUT")
        {
          pinMode(digitalPort,OUTPUT);
        }
        else
        {
          errUndefinedValue();
        }
      }
      //Changes digital port state 
      else if(subCommand == "SET")
      {
        getSubCommand(false);
        digitalPort = subCommand.toInt();
        getSubCommand(true);
        if(subCommand == "HIGH")
        {
          digitalWrite(digitalPort,HIGH);
        }
        else if(subCommand == "LOW")
        {
          digitalWrite(digitalPort,LOW);
        } 
        else
        {
          errUndefinedValue();
        }
      }
      //Gets digital port state
      else if(subCommand == "GET")
      {
        getSubCommand(false);
        digitalPort = subCommand.toInt();
        value = digitalRead(digitalPort);
        if(value) 
          Serial.println(1);
        else 
          Serial.println(0);
      } 
      
      //Gets all the digital port states. ArduinoBaseController class parses this data.
      else if(subCommand == "GETALL")
      {
        String sentData = "";
        for(digitalPort = MIN_DIGITAL_PORT; digitalPort <= MAX_DIGITAL_PORT; digitalPort++)
        {
          value = digitalRead(digitalPort);
          sentData += "D" + (String)digitalPort + ",";
          if(value)
          {
            sentData += "1";
          }
          else 
          {
            sentData += "0";
          }
          if(digitalPort != MAX_DIGITAL_PORT)
          {
            sentData += "|"; 
          }
        }
        Serial.println(sentData);
      } 
      else if(subCommand == "HELP")
      {
        Serial.println("PINMODE, sets a digital port as input or output.|SET, changes the digital port value.|GET, gets a digital port value|GETALL, gets all data ports");
      }
      else 
      {
        errUndefined();
      }
    }
    //This is the analog state
    void qAnalog(){
      getSubCommand(true);
      int analogValue = 0;
      int analogPort = 0;
      //Gets an analog value and print it on serial console
      if(subCommand == "GET")
      {
        getSubCommand(false);
        analogPort = subCommand.toInt();
        analogValue = analogRead(analogPort);
        Serial.println(analogValue);
      }
      //Gets all analog values and print them on serial console
      else if(subCommand == "GETALL")
      {
        String sentData = "";
        for(analogPort = MIN_ANALOG_PORT; analogPort <= MAX_ANALOG_PORT; analogPort++)
        {
          analogValue = analogRead(analogPort);
          sentData += "A" + (String)analogPort + "," + (String)analogValue;
          if(analogPort != MAX_ANALOG_PORT)
          {
            sentData += "|"; 
          }
        }
        Serial.println(sentData);
      } 
      else if(subCommand == "HELP")
      {
        Serial.println("GET, gets an analog port value.|GETALL, gets all the analog ports values.");
      }
      else
      {
        errUndefined();
      }
    }
    //This is the command list state
    void qCommandList()
    {
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Returns the list of all the instructions available for this microcontroller");
      else
        Serial.println("ABOUT|FLUSH|PWM|DIGITAL|ANALOG");
    }

    //This state sets the limits port values to digital, analog and PWM
    void qSetGlobal(){
      getSubCommand(true);
      int globalValue = 0;
      
      if(subCommand == "MIN_DIGITAL_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MIN_DIGITAL_PORT = globalValue;
        } 
        else 
        {
          errUndefinedValue();
        }
      }
      else if(subCommand == "MAX_DIGITAL_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MAX_DIGITAL_PORT = globalValue;
        }
        else
        {
          errUndefinedValue();
        }
      }
      else if(subCommand == "MIN_ANALOG_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MIN_ANALOG_PORT = globalValue;
        } 
        else
        {
          errUndefinedValue();
        }
      }
      else if(subCommand == "MAX_ANALOG_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MAX_ANALOG_PORT = globalValue;
        }
        else
        {
          errUndefinedValue();
        }
      }
      else if(subCommand == "MIN_PWM_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MIN_PWM_PORT = globalValue;
        }
        else 
        {
          errUndefinedValue();
        }
      }
      else if(subCommand == "MAX_PWM_PORT")
      {
        getSubCommand(false);

        globalValue = subCommand.toInt();
        if(globalValue >= 0)
        {
          MAX_PWM_PORT = globalValue;
        }
        else 
        {
          errUndefinedValue();
        }
      }
      else 
      {
        errGlobalUndefinedVariable();
      }
    }

    //This method gets the next substring and put it on subCommand property
    void getSubCommand(bool isCommand){
      subCommand = "";
      for(; commandIndex < commandLenght; commandIndex++)
      {
        if(command[commandIndex] == ' ') 
        { 
          commandIndex++; break;
        }
        else if(command[commandIndex] != '\n')
          subCommand += command[commandIndex];
      }
      if(isCommand)
        subCommand.toUpperCase();
    }

    //This is the errors definitions
    void errUndefined(){
      if(this->subCommand == "")
        Serial.println("It was waiting for an instruction");
      else 
        Serial.println("Instruction not defined by microcontroller: '" + this->subCommand + "'");
    }

    void errOutOfIndex(String function, int port, int value){
      Serial.println("Value out of index in function '"+ function +"', port '" + port + "', value '"+ value +"'");
    }

    void errUndefinedValue(){
      Serial.println("The value '" + this->subCommand + "' is not defined on this context");
    }

    void errGlobalUndefinedVariable(){
      if(this->subCommand == "")
        Serial.println("It was waiting a variable");
      else 
        Serial.println("Variable not defined by microcontroller: '" + this->subCommand + "'");
    }
};
//Declares ArduinoController object
ArduinoController *_arduinoController;

void setup() 
{
  //Creates ArduinoController object
  _arduinoController = new ArduinoController();

  //Starts serial baudrate
  Serial.begin(SERIAL_BAUDRATE);

  //Starts serial receiving data false (Arduino is still setting up)
  isReceivingData = false;
}

void loop() 
{
    //We do not use while because the loop does while-like.
    if(Serial.available() > 0)
    {
      receivedByte = Serial.read();
      if((char)receivedByte == '\r') // End of line defined by SerialPort class
      {
          readReceivedData(); // It goes to ArduinoController and play it.
          receivedData = ""; // It Clears received data (our buffer), now the serial port is ready to receive more bytes.
      }
      else
        receivedData += (char)receivedByte; // It concats the last received byte with the string buffered in Arduino.
    }
}

void readReceivedData()
{
  _arduinoController->setCommand(receivedData);
  _arduinoController->qPlay();
}
