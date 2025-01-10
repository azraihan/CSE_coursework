package Display;

public class OLED extends Display {
    public OLED() {
        super("OLED");
    }

    public void showDisplay() {
        System.out.println(this.getDisplay());
    }
}
