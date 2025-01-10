package AbstractFactory;

import Account.*;
import Loan.*;

public class PremiumFactory implements BankServiceFactory {
    @Override
    public Account createAccount(double deposit) {
        return new PremiumAccount(deposit);
    }

    @Override
    public Loan createLoan(double deposit) {
        return new PremiumLoan(deposit);
    }
}