package AbstractFactory;

import Account.*;
import Loan.*;

public class RegularFactory implements BankServiceFactory {
    @Override
    public Account createAccount(double deposit) {
        return new RegularAccount(deposit);
    }

    @Override
    public Loan createLoan(double deposit) {
        return new RegularLoan(deposit);
    }
}
