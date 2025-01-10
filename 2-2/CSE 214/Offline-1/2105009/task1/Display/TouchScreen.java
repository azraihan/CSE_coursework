package Display;

public class TouchScreen extends Display {
    public TouchScreen() {
        super("TouchScreen");
    }

    public void showDisplay() {
        System.out.println(this.getDisplay());
    }
}
