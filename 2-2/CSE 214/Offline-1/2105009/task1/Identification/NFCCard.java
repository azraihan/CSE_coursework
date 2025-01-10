package Identification;

public class NFCCard extends Identification {
    public NFCCard() {
        super("RFIDCard");
    }

    public void showIdentification() {
        System.out.println(this.getIdentification());
    }
}
