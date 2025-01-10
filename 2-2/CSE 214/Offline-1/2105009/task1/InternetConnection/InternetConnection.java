package InternetConnection;

public abstract class InternetConnection {
    private String internetConnection;

    public InternetConnection(String internetConnection) {
        this.internetConnection = internetConnection;
    }

    public String getInternetConnection() {
        return internetConnection;
    }

    abstract public void showInternetConnection();
}