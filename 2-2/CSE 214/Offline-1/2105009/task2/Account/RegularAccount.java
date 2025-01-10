package Account;

public class RegularAccount extends Account {
    public RegularAccount(double deposit){
        super(deposit,2.5);
    }

    public void showAccount(){
        System.out.println("This is a Regular Account.");
    }
}