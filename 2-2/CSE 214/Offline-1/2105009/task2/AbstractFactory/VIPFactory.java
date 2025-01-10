package AbstractFactory;

import Account.*;
import Loan.*;

public class VIPFactory implements BankServiceFactory {
    @Override
    public Account createAccount(double deposit) {
        return new VIPAccount(deposit);
    }

    @Override
    public Loan createLoan(double deposit) {
        return new VIPLoan(deposit);
    }
}