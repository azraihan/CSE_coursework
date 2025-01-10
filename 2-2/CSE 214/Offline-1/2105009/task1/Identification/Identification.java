package Identification;

public abstract class Identification {
    private String identification;

    public Identification(String identification) {
        this.identification = identification;
    }

    public String getIdentification() {
        return identification;
    }

    abstract public void showIdentification();
}