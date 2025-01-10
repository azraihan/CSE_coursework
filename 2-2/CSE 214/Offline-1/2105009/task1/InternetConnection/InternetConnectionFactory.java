package InternetConnection;

public class InternetConnectionFactory {
    public InternetConnection getInternetConnection(String internetConnection) {
        if (internetConnection.equalsIgnoreCase("WiFi")) {
            return new WiFi();
        } else if (internetConnection.equalsIgnoreCase("GSM")) {
            return new GSM();
        } else if (internetConnection.equalsIgnoreCase("Ethernet")) {
            return new Ethernet();
        }
        return null;
    }
}
