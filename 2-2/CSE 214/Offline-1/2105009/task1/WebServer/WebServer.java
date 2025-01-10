package WebServer;

public abstract class WebServer {
    private String webServer;

    public WebServer(String webServer) {
        this.webServer = webServer;
    }

    public String getWebServer() {
        return webServer;
    }

    abstract public void showWebServer();
}