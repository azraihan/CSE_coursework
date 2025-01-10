package WebServer;

public class Django extends WebServer {
    public Django() {
        super("Django");
    }

    public void showWebServer() {
        System.out.println(this.getWebServer());
    }
}