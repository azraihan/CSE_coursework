package Identification;

public class RFIDCard extends Identification {
    public RFIDCard() {
        super("RFIDCard");
    }

    public void showIdentification() {
        System.out.println(this.getIdentification());
    }
}