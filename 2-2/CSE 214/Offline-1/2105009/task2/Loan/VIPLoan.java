package Loan;

public class VIPLoan extends Loan {
    public VIPLoan(double deposit){
        super(deposit,10);
    }
    public void showAccount(){
        System.out.println("This is a VIP Loan.");
    }
}