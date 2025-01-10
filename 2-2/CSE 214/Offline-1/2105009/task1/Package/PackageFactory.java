package Package;

public class PackageFactory {
    public Package getPackage(String packageType) {
        if (packageType.equalsIgnoreCase("Basic")) {
            return new Basic();
        } else if (packageType.equalsIgnoreCase("Standard")) {
            return new Standard();
        } else if (packageType.equalsIgnoreCase("Advanced")) {
            return new Advanced();
        } else if (packageType.equalsIgnoreCase("Premium")) {
            return new Premium();
        }
        return null;
    }
}
