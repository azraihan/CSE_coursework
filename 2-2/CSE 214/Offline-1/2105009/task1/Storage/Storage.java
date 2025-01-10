package Storage;

public abstract class Storage {
    private String storage;

    public Storage(String storage) {
        this.storage = storage;
    }

    public String getStorage() {
        return storage;
    }

    abstract public void showStorage();
}