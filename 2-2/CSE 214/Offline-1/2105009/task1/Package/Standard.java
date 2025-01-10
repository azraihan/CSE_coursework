package Package;

import Microprocessor.ArduinoMega;
import Display.LED;

public class Standard extends Package {
    Standard() {
        super("Standard", new ArduinoMega(), new LED());
    }
}
