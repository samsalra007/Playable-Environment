/*
 * Creator: Samuel Salazar Ramírez      samuel.salazar@besoft.com.mx
 * Company: BeSoftCo
 * Oficial Site: https://www.besoft.com.mx/
 */

#include <string.h>

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

    int MIN_DIGITAL_PORT = 0;
    int MAX_DIGITAL_PORT = 0;

    int MIN_ANALOG_PORT = 0;
    int MAX_ANALOG_PORT = 0;

    int MIN_PWM_PORT = 0;
    int MAX_PWM_PORT = 0;
    
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

    void qAbout(){
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Describes the basic info about the firmware and microcontroller serial port.");
      else 
        Serial.println("Arduino MEGA 2560|v1.0 Alpha 1|BeSoftCo|2017|samuel.salazar@besoft.com.mx");
    }

    void qFlush(){
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Cleans the serial buffer.");
      else
        Serial.flush();
    }

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

    void qDigital(){
      int digitalPort = 0;
      bool value = false;
      getSubCommand(true);
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
    
    void qAnalog(){
      getSubCommand(true);
      int analogValue = 0;
      int analogPort = 0;
      if(subCommand == "GET")
      {
        getSubCommand(false);
        analogPort = subCommand.toInt();
        analogValue = analogRead(analogPort);
        Serial.println(analogValue);
      }
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

    void qCommandList()
    {
      getSubCommand(true);
      if(subCommand == "HELP")
        Serial.println("Returns the list of all the instructions available for this microcontroller");
      else
        Serial.println("ABOUT|FLUSH|PWM|DIGITAL|ANALOG");
    }
    
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

ArduinoController *_arduinoController;
void setup() 
{
  _arduinoController = new ArduinoController();
  Serial.begin(SERIAL_BAUDRATE);
  isReceivingData = false;
}

void loop() 
{
    if(Serial.available() > 0)
    {
      receivedByte = Serial.read();
      if((char)receivedByte == '\r')
      {
          readReceivedData();
          receivedData = "";
      }
      else
        receivedData += (char)receivedByte;
    }
}

void readReceivedData()
{
  _arduinoController->setCommand(receivedData);
  _arduinoController->qPlay();
}
