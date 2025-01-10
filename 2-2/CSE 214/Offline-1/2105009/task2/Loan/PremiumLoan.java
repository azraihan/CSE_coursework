package Loan;

public class PremiumLoan extends Loan {
    public PremiumLoan(double deposit){
        super(deposit,12);
    }
    public void showAccount(){
        System.out.println("This is a Premium Loan.");
    }
}