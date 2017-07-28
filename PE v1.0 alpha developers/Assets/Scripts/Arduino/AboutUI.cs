/* Creator and developer: Samuel Salazar Ramírez  samuel.salazar@besoft.com.mx
 * Company: BeSoftCo
 * Oficial Site: https://www.besoft.com.mx/
 */
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class AboutUI : MonoBehaviour {

    private ArduinoBaseController arduino;
    private List<string> commandListGeneric;
    public GameObject microcontroller;
    public GameObject version;
    public GameObject vendor;
    public GameObject lastSupportYear;
    public GameObject mailContact;
    public GameObject availableCommands;
    public GameObject commandInfo;

    public GameObject analogicValues;
    public GameObject digitalValues;

    private Text txtAnalogicValues, txtDigitalValues;
    private ArduinoPort[] analogPortList, digitalPortList;

    public void Start()
    {
        arduino = gameObject.GetComponent<ArduinoController>();
        txtAnalogicValues = analogicValues.GetComponent<Text>();
        txtDigitalValues = digitalValues.GetComponent<Text>();
    }

    public void Update()
    {
        if (arduino != null)
        {
            if (arduino.IsOpen())
            {
                analogPortList = arduino.AnalogPortList;
                txtAnalogicValues.text = "";
                for (int i = 0; i < analogPortList.Length && analogPortList[i] != null; i++)
                {
                    txtAnalogicValues.text += analogPortList[i].PortName + " = " + analogPortList[i].PortValue + "\n";
                }

                digitalPortList = arduino.DigitalPortList;
                txtDigitalValues.text = "";
                for (int i = 0; i < digitalPortList.Length && digitalPortList[i] != null; i++)
                {
                    if (digitalPortList[i].PortValue == 1)
                        txtDigitalValues.text += digitalPortList[i].PortName + " = HIGH\n";
                    else
                        txtDigitalValues.text += digitalPortList[i].PortName + " = LOW\n";
                }
            }
        }
    }

    public void ShowAboutWindow()
    {
        microcontroller.GetComponent<Text>().text   = "Controller: " + arduino.Controller;
        version.GetComponent<Text>().text           = "Firmware Version: " + arduino.FirmwareVersion;
        vendor.GetComponent<Text>().text            = "Firmware Vendor: " + arduino.FirmwareVendor;
        lastSupportYear.GetComponent<Text>().text   = "Year Support: " + arduino.YearSupport;
        mailContact.GetComponent<Text>().text       = "E-mail Contact: " + arduino.EmailContact;

        string[] commandList = arduino.AvailableCommands;

        commandListGeneric = new List<string>();
        for (int i = 0; i < commandList.Length; i++)
        {
                commandListGeneric.Add(commandList[i]);
        }
        availableCommands.GetComponent<Dropdown>().ClearOptions();
        availableCommands.GetComponent<Dropdown>().AddOptions(commandListGeneric);
    }

    public void ShowCommandHelp()
    {
        int selectedValue = availableCommands.GetComponent<Dropdown>().value;
        string help = arduino.AvailableCommandsHelp[selectedValue];
        commandInfo.GetComponent<Text>().text = help;
    }
}