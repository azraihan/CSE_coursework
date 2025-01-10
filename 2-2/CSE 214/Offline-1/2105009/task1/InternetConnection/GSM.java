package InternetConnection;

public class GSM extends InternetConnection {
    public GSM() {
        super("GSM");
    }

    public void showInternetConnection() {
        System.out.println(this.getInternetConnection());
    }
}
