package Display;

public abstract class Display {
    private String display;

    public Display(String display) {
        this.display = display;
    }

    public String getDisplay() {
        return display;
    }

    abstract public void showDisplay();
}
