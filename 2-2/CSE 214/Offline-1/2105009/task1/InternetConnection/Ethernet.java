package InternetConnection;

public class Ethernet extends InternetConnection {
    public Ethernet() {
        super("Ethernet");
    }

    public void showInternetConnection() {
        System.out.println(this.getInternetConnection());
    }
}
