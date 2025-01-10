package Display;

public class LCD extends Display {
    public LCD() {
        super("LCD");
    }

    public void showDisplay() {
        System.out.println(this.getDisplay());
    }
}