package Microprocessor;

public abstract class Microprocessor {
    private String microprocessor;

    public Microprocessor(String microprocessor) {
        this.microprocessor = microprocessor;
    }

    public String getMicroprocessor() {
        return microprocessor;
    }

    abstract public void showMicroprocessor();
}
