package WebServer;

public class NodeJS extends WebServer {
    public NodeJS() {
        super("NodeJS");
    }

    public void showWebServer() {
        System.out.println(this.getWebServer());
    }
}