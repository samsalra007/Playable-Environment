/* Creator and developer: Samuel Salazar Ramírez  samuel.salazar@besoft.com.mx
 * Company: BeSoftCo
 * Oficial Site: https://www.besoft.com.mx/
 */

using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using UnityEngine;

public class ArduinoBaseController : MonoBehaviour
{
    public enum Ports
    {
        COM1 = 1,
        COM2 = 2,
        COM3 = 3,
        COM4 = 4,
        COM5 = 5,
        COM6 = 6,
        COM7 = 7,
        COM8 = 8,
        COM9 = 9,
        COM10 = 10
    };

    public enum Pinmodes
    {
        Input,
        Output
    };

    protected SerialPort serialPort;

    protected ArduinoPort[] digitalPortList;
    public ArduinoPort[] DigitalPortList{
        get { return digitalPortList; }
    }

    protected ArduinoPort[] analogPortList;
    public ArduinoPort[] AnalogPortList
    {
        get { return analogPortList; }
    }

    protected ArduinoPort[] pwmPortList;
    public ArduinoPort[] PWMPortList
    {
        get { return pwmPortList; }
    }

    private Queue<string> queryQueue;

    public Ports port = Ports.COM3;
    public int baudRate = 9600;
    public bool connectOnStart = true;
    public bool setupOnStart = true;

    protected string controller;
    public string Controller
    {
        get { return controller; }
    }

    protected string firmwareVersion;
    public string FirmwareVersion
    {
        get { return firmwareVersion; }
    }

    protected string firmwareVendor;
    public string FirmwareVendor
    {
        get { return firmwareVendor; }
    }

    protected string yearSupport;
    public string YearSupport
    {
        get { return yearSupport; }
    }

    protected string emailContact;
    public string EmailContact
    {
        get { return emailContact; }
    }

    protected string[] availableCommands;
    public string[] AvailableCommands
    {
        get { return availableCommands; }
        set { availableCommands = value; }
    }

    protected string[] availableCommandsHelp;
    public string[] AvailableCommandsHelp
    {
        get { return availableCommandsHelp; }
    }

    public int MinDigitalPort = 22;
    public int MaxDigitalPort = 53;

    public int MinAnalogPort = 0;
    public int MaxAnalogPort = 15;

    public int MinPWMPort = 2;
    public int MaxPWMPort = 13;

    public virtual void Setup()
    {

    }

    void Start()
    {
        Debug.Log("Creating Serial Port Controller");
        serialPort = new SerialPort(port.ToString(), baudRate);
        queryQueue = new Queue<string>();

        if (connectOnStart)
        {
            serialPort.Open();
            if (setupOnStart)
            {
                Debug.Log("Setting Up Device ");
                Setup();
            }

            Debug.Log("Setting Up Ports Limits ");
            digitalPortList = new ArduinoPort[MaxDigitalPort - MinDigitalPort + 1];
            analogPortList = new ArduinoPort[MaxAnalogPort - MinAnalogPort + 1];
            pwmPortList = new ArduinoPort[MaxPWMPort - MinPWMPort + 1];

            Debug.Log("Setting Up Global Variables ");
            SetGlobalVariables();

            Debug.Log("Loading Basic Information ");
            LoadAbout();
            LoadSerialCommandList();
            LoadHelpInfo();

            Debug.Log("Creating Ardunio Thread to Get Data '" + controller + "'");
            ThreadStart threadStart = new ThreadStart(GetPortListData);
            Thread thread = new Thread(threadStart);

            thread.Priority = System.Threading.ThreadPriority.Highest;
            
            Debug.Log("Starting Thread '" + controller + "'");
            thread.Start();
        }
    }

    void OnApplicationQuit()
    {
        serialPort.BaseStream.Flush();
        CloseConnection();
    }

    private void OpenConnection()
    {
        Debug.Log("Connecting Device '" + controller + "'");
        if (serialPort == null)
            Debug.LogError("SerialPort is not created yet");
        else if (IsOpen())
            Debug.LogWarning("SerialPort is already open");
        else
        {
            serialPort.Open();
            Debug.Log("Successfully Connected '" + controller + "'");
        }
    }

    public bool IsOpen()
    {
        return serialPort.IsOpen;
    }

    private void CloseConnection()
    {
        Debug.Log("Closing Connection '" + controller + "'");
        if (serialPort == null)
            Debug.LogError("SerialPort is not defined yet");
        else if (IsOpen())
            serialPort.Close();
        else
            Debug.LogWarning("SerialPort is already closed");
    }

    private void SetGlobalVariables()
    {
        serialPort.WriteLine("SETGLOBAL MIN_DIGITAL_PORT " + MinDigitalPort.ToString());
        serialPort.WriteLine("SETGLOBAL MAX_DIGITAL_PORT " + MaxDigitalPort.ToString());

        serialPort.WriteLine("SETGLOBAL MIN_ANALOG_PORT " + MinAnalogPort.ToString());
        serialPort.WriteLine("SETGLOBAL MAX_ANALOG_PORT " + MaxAnalogPort.ToString());

        serialPort.WriteLine("SETGLOBAL MIN_PWM_PORT " + MinPWMPort.ToString());
        serialPort.WriteLine("SETGLOBAL MAX_PWM_PORT " + MaxPWMPort.ToString());

        serialPort.BaseStream.Flush();
    }

    public void SetPWM(int port, int value)
    {
        string query = "";
        if (value >= 0 && value <= 1023)
        {
            query = "PWM SET " + port.ToString() + " " + value.ToString();
            queryQueue.Enqueue(query);
        }
        else
        {
            Debug.LogError("The value '" + value.ToString() + "' in port '" + port.ToString() + "' is not valid. ");
        }
    }
    
    protected void DigitalPinMode(int port, Pinmodes pinmode)
    {
        string query = "";
        query = "DIGITAL PINMODE " + port.ToString() + " " + pinmode.ToString();
        queryQueue.Enqueue(query);
    }

    public void DigitalWrite(int port, bool value)
    {
        string query = "";
        if (value)
            query = "DIGITAL SET " + port.ToString() + " HIGH";
        else
            query = "DIGITAL SET " + port.ToString() + " LOW";
        queryQueue.Enqueue(query);
    }

    public bool DigitalRead(int port)
    {
        return digitalPortList[port - MinDigitalPort].PortValue == 1;
    }

    public float AnalagorRead(int port)
    {
        return analogPortList[port - MinAnalogPort].PortValue;
    }

    private void LoadAbout()
    {
        string[] dataReceived = new string[1];
        if (serialPort.IsOpen)
        {
            serialPort.WriteLine("ABOUT");
            dataReceived = serialPort.ReadLine().Split('|');
            controller = dataReceived[0];
            firmwareVersion = dataReceived[1];
            firmwareVendor = dataReceived[2];
            yearSupport = dataReceived[3];
            emailContact = dataReceived[4];
        }
        else
        {
            Debug.LogError("SerialPort is not defined");
        }
    }

    private void LoadSerialCommandList()
    {
        string[] lstCommands = new string[1];
        serialPort.WriteLine("COMMANDLIST");
        availableCommands = serialPort.ReadLine().Split('|');
    }

    private void LoadHelpInfo()
    {
        string[] helpCommand = new string[1];
        availableCommandsHelp = new string[availableCommands.Length];
        for(int i = 0; i<availableCommands.Length; i++)
        {
            availableCommandsHelp[i] = "";
            serialPort.WriteLine(availableCommands[i] + " HELP");
            helpCommand = serialPort.ReadLine().Split('|');

            for(int j = 0; j<helpCommand.Length; j++)
                availableCommandsHelp[i] += helpCommand[j] + "\n";
        }
    }

    private void FlushSerial()
    {
        if (serialPort.IsOpen)
        {
            serialPort.WriteLine("FLUSH");
            serialPort.BaseStream.Flush();
        }
    }

    private void GetPortListData()
    {
        Debug.Log("Thread Created '" + controller + "'");

        string[] dataReceived = new string[1];
        string[] portInfo = new string[1];
        string portName = "";
        int portValue = 0;

        Debug.Log("Getting serial data from '" + controller + "'");
        while (serialPort.IsOpen)
        {
            try
            {
                serialPort.WriteLine("DIGITAL GETALL");
                dataReceived = serialPort.ReadLine().Split('|');
                for (int i = 0; i < dataReceived.Length; i++)
                {
                    portInfo = dataReceived[i].Split(',');
                    portName = portInfo[0];
                    int.TryParse(portInfo[1], out portValue);
                    digitalPortList[i] = new ArduinoPort(portName, portValue);
                }

                serialPort.WriteLine("ANALOG GETALL");
                dataReceived = serialPort.ReadLine().Split('|');
                for (int i = 0; i < dataReceived.Length; i++)
                {
                    portInfo = dataReceived[i].Split(',');
                    portName = portInfo[0];
                    int.TryParse(portInfo[1], out portValue);
                    analogPortList[i] = new ArduinoPort(portName, portValue);
                }

                while (queryQueue.Count > 0)
                {
                    serialPort.WriteLine(queryQueue.Dequeue());
                }
            }
            catch (NullReferenceException nrEx)
            {

            }
        }
        Debug.Log("Ending serial data from '" + controller + "'");
    }

}