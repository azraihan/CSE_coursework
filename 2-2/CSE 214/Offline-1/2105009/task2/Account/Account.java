package Account;

// Abstract Product: Account
public abstract class Account {
    private double deposit;
    private double interest;

    public Account(double deposit, double interest){
        this.deposit=deposit;
        this.interest=interest;
    }

    abstract void showAccount();

    public double getInterestRate(){
        return interest;
    }
    public double getDeposit(){
        return deposit;
    }
    public double caluclateInterest(int time){
        return deposit * interest * time;
    } //uses simple interest formula
}