package Package;

import Microprocessor.ATMega32;
import Display.LCD;

public class Basic extends Package {
    Basic() {
        super("Basic", new ATMega32(), new LCD());
    }
}
