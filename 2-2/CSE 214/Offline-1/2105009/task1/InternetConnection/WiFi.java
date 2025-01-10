package InternetConnection;

public class WiFi extends InternetConnection {
    public WiFi() {
        super("WiFi");
    }

    public void showInternetConnection() {
        System.out.println(this.getInternetConnection());
    }
}
