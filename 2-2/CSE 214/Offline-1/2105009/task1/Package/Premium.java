package Package;

import Microprocessor.RaspberryPi;
import Display.TouchScreen;

public class Premium extends Package {
    Premium() {
        super("Premium", new RaspberryPi(), new TouchScreen());
    }
}
