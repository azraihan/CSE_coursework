package Product;

import Package.Package;
import InternetConnection.InternetConnection;
import WebServer.WebServer;
import Identification.Identification;
import PaymentTerminal.PaymentTerminalClass;
import Storage.Storage;
import Controller.Controller;


public class Product {
    Package packag;
    InternetConnection internetConnection;
    WebServer webServer;
    Identification identification;
    PaymentTerminalClass paymentTerminalClass;
    Storage storage;
    Controller controller;

    Product product;

    // Product(Package packag, InternetConnection internetConnection, WebServer
    // webServer, Identification identification, PaymentTerminalClass
    // paymentTerminalClass, Storage storage, Controller controller){
    // this.packag=packag;
    // this.internetConnection=internetConnection;
    // this.webServer=webServer;
    // this.identification=identification;
    // this.paymentTerminalClass=paymentTerminalClass;
    // this.storage=storage;
    // this.controller=controller;
    // }

    public void show() {
        System.out.println("System Details:");
        System.out.println("Package:");
        System.out.println("Webserver:");
        System.out.println("Identification:");
        System.out.println("Payment Terminal");
        System.out.println("Storage:");
        System.out.println("Controller:");
    }

    public void add(Product product) {
        this.product = product;
    }

    public void addPackage(Package packag) {
        this.packag = packag;
    }

    public void addInternetConnection(InternetConnection internetConnection) {
        this.internetConnection = internetConnection;
    }

    public void addWebServer(WebServer webServer) {
        this.webServer = webServer;
    }

    public void addIdentification(Identification identification) {
        this.identification = identification;
    }

    public void addPaymentTerminal(PaymentTerminalClass paymentTerminalClass) {
        this.paymentTerminalClass = paymentTerminalClass;
    }

    public void addStorage(Storage storage) {
        this.storage = storage;
    }

    public void addController(Controller controller) {
        this.controller = controller;
    }
}