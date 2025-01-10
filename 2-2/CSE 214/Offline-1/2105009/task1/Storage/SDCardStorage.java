package Storage;

public class SDCardStorage extends Storage {
    public SDCardStorage() {
        super("SDCardStorage");
    }

    public void showStorage() {
        System.out.println(this.getStorage());
    }
}
