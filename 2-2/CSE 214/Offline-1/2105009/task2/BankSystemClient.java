import Loan.*;
import AbstractFactory.*;
import Account.*;

// Client code
public class BankSystemClient {
    public static void main(String[] args) {
        // Test the system with different customer types
        BankServiceFactory factory;

        double deposit=2000;

        // Example for Regular Customer
        factory = new RegularFactory();
        Account regularAccount = factory.createAccount(deposit);
        Loan regularLoan = factory.createLoan(deposit);

        System.out.println("Regular Account Interest Rate: " + regularAccount.getInterestRate() + "%");
        System.out.println("Regular Account Total Interest: " + regularAccount.caluclateInterest(2) + "%");
        System.out.println("Regular Total Loan Interest: " + regularLoan.caluclateInterest(2) + "%");
        System.out.println("Regular Loan Interest Rate: " + regularLoan.getInterestRate() + "%");
        System.out.println("Regular Loan Total Interest: " + regularLoan.caluclateInterest(2) + "%");

        // Example for Premium Customer
        factory = new PremiumFactory();
        Account premiumAccount = factory.createAccount(deposit);
        Loan premiumLoan = factory.createLoan(deposit);

        System.out.println("Premium Account Interest Rate: " + premiumAccount.getInterestRate() + "%");
        System.out.println("Regular Account Total Interest: " + premiumAccount.caluclateInterest(2) + "%");
        System.out.println("Premium Loan Interest Rate: " + premiumLoan.getInterestRate() + "%");
        System.out.println("Premium Loan Total Interest: " + premiumLoan.caluclateInterest(2) + "%");

        // Example for VIP Customer
        factory = new VIPFactory();
        Account vipAccount = factory.createAccount(deposit);
        Loan vipLoan = factory.createLoan(deposit);

        System.out.println("VIP Account Interest Rate: " + vipAccount.getInterestRate() + "%");
        System.out.println("Regular Account Total Interest: " + vipAccount.caluclateInterest(2) + "%");
        System.out.println("VIP Loan Interest Rate: " + vipLoan.getInterestRate() + "%");
        System.out.println("VIP Loan Total Interest: " + premiumLoan.caluclateInterest(2) + "%");
    }
}
