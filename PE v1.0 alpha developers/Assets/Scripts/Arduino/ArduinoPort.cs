using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ArduinoPort {

    public string PortName { get; set; }
    public int PortValue { get; set; }

    public ArduinoPort()
    {
        this.PortName = "";
        this.PortValue = 0;
    }

    public ArduinoPort(string pinName)
    {
        this.PortName = pinName;
        this.PortValue = 0;
    }

    public ArduinoPort(string portName,int portValue)
    {
        this.PortName = portName;
        this.PortValue = portValue;
    }
}
