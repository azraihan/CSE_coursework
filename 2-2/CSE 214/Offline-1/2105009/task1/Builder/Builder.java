package Builder;

import Identification.Identification;
import PaymentTerminal.PaymentTerminalClass;
import Storage.Storage;
import Controller.Controller;
import Product.Product;

public interface Builder {
    void addPackage(String packageType);

    void addInternetConnection(String internetConnection);

    void addWebServer(String frameowrk);

    void addIdentification(Identification identification);

    void addPaymentTerminal(PaymentTerminalClass paymentTerminalClass);

    void addStorage(Storage storage);

    void addController(Controller controller);

    Product getProduct();
}
