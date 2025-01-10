package WebServer;

public class WebServerFactory {
    public WebServer getWebServer(String webServer) {
        if (webServer.equalsIgnoreCase("NodeJS")) {
            return new NodeJS();
        } else if (webServer.equalsIgnoreCase("Django")) {
            return new Django();
        } else if (webServer.equalsIgnoreCase("Ruby")) {
            return new Ruby();
        }
        return null;
    }
}