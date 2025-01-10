package Account;

public class PremiumAccount extends Account {
    public PremiumAccount(double deposit){
        super(deposit,3.5);
    }
    public void showAccount(){
        System.out.println("This is a Premium Account.");
    }
}