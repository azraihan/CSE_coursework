package Display;

public class LED extends Display {
    public LED() {
        super("LED");
    }

    public void showDisplay() {
        System.out.println(this.getDisplay());
    }
}