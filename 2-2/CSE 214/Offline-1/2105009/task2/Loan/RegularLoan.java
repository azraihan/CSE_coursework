package Loan;

public class RegularLoan extends Loan {
    public RegularLoan(double deposit){
        super(deposit,14);
    }

    public void showAccount(){
        System.out.println("This is a Regular Loan.");
    }
}