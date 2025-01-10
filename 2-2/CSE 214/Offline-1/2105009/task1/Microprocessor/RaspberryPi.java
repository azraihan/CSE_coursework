package Microprocessor;

public class RaspberryPi extends Microprocessor {
    public RaspberryPi() {
        super("RaspberryPi");
    }

    public void showMicroprocessor() {
        System.out.println(this.getMicroprocessor());
    }
}
