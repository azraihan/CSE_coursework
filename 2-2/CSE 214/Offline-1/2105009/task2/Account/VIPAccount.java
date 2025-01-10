package Account;

public class VIPAccount extends Account {
    public VIPAccount(double deposit){
        super(deposit,5);
    }
    public void showAccount(){
        System.out.println("This is a VIP Account.");
    }
}