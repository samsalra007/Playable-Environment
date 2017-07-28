/* Creator and developer: Samuel Salazar Ramírez  samuel.salazar@besoft.com.mx
 * Company: BeSoftCo
 * Oficial Site: https://www.besoft.com.mx/
 */
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ArduinoController : ArduinoBaseController {

    public override void Setup()
    {
        //On this section you can use pinmode
        DigitalPinMode(53, Pinmodes.Input);
        base.Setup();
    }

    void Update()
    {
        //TODO the same thing in loop() function from Arduino IDE
    }
}
