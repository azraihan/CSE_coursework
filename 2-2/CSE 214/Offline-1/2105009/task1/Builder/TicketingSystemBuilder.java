package Builder;

import Product.Product;
import Package.PackageFactory;
import InternetConnection.InternetConnectionFactory;
import WebServer.WebServerFactory;
import Identification.Identification;
import PaymentTerminal.PaymentTerminalClass;
import Storage.Storage;
import Controller.Controller;

public class TicketingSystemBuilder implements Builder {
    private Product product;

    public void addPackage(String packageType) {
        PackageFactory packageFactory = new PackageFactory();
        product.addPackage(packageFactory.getPackage(packageType));
    }

    public void addInternetConnection(String internetConnection) {
        InternetConnectionFactory internetConnectionFactory = new InternetConnectionFactory();
        product.addInternetConnection(internetConnectionFactory.getInternetConnection(internetConnection));
    }

    public void addWebServer(String frameowrk) {
        WebServerFactory webServerFactory = new WebServerFactory();
        product.addWebServer(webServerFactory.getWebServer(frameowrk));
    }

    public void addIdentification(Identification identification) {
        product.addIdentification(identification);
    }

    public void addPaymentTerminal(PaymentTerminalClass paymentTerminalClass) {
        product.addPaymentTerminal(paymentTerminalClass);
    }

    public void addStorage(Storage storage) {
        product.addStorage(storage);
    }

    public void addController(Controller controller) {
        product.addController(controller);
    }

    public Product getProduct() {
        return product;
    }
}