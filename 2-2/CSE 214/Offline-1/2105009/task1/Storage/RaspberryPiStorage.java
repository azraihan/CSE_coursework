package Storage;

public class RaspberryPiStorage extends OwnStorage {
    public RaspberryPiStorage() {
        super("RaspberryPiStorage");
    }

    public void showStorage() {
        System.out.println(this.getStorage());
    }
}
