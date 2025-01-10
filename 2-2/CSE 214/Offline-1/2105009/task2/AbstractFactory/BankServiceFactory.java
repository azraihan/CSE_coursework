package AbstractFactory;

import Account.Account;
import Loan.Loan;

public interface BankServiceFactory {
    Account createAccount(double deposit);
    Loan createLoan(double deposit);
}
