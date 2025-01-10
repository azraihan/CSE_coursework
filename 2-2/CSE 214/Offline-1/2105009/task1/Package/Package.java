package Package;

import Microprocessor.Microprocessor;
import Display.Display;

public abstract class Package {
    String PackageType;
    Microprocessor microprocessor;
    Display display;

    public Package(String PackageType, Microprocessor microprocessor, Display display) {
        this.PackageType = PackageType;
        this.microprocessor = microprocessor;
        this.display = display;
    }

    public String getPackageType() {
        return PackageType;
    }

    public String getPackageMicroprocessor() {
        return microprocessor.getMicroprocessor();
    }

    public String getPackageDisplay() {
        return display.getDisplay();
    }
}
