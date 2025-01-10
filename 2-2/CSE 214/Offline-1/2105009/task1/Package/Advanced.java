package Package;

import Microprocessor.RaspberryPi;
import Display.OLED;

public class Advanced extends Package {
    Advanced() {
        super("Advanced", new RaspberryPi(), new OLED());
    }
}
