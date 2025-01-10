package Controller;

public class ArduinoController extends Controller {
    public ArduinoController() {
        super("ArduinoController");
    }

    public void showController() {
        System.out.println(this.getController());
    }
}