package Controller;

public abstract class Controller {
    private String controller;

    public Controller(String controller) {
        this.controller = controller;
    }

    public String getController() {
        return controller;
    }

    abstract public void showController();
}
