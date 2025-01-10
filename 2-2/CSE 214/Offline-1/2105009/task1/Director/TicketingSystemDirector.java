package Director;

import Builder.TicketingSystemBuilder;
import PaymentTerminal.PaymentTerminal;
import Storage.*;
import Identification.*;
import Controller.*;

public class TicketingSystemDirector {
    TicketingSystemBuilder ticketingSystemBuilder;

    public void buildTicketingSystem(TicketingSystemBuilder ticketingSystemBuilder, String packageType,
            String internetConnection, String framework) {
        this.ticketingSystemBuilder = ticketingSystemBuilder;

        ticketingSystemBuilder.addPackage(packageType);
        ticketingSystemBuilder.addInternetConnection(internetConnection);
        ticketingSystemBuilder.addWebServer(framework);
        ticketingSystemBuilder.addPaymentTerminal(new PaymentTerminal());

        if(packageType.equalsIgnoreCase("Basic") || packageType.equalsIgnoreCase("Standard")){
            ticketingSystemBuilder.addIdentification(new RFIDCard());
            ticketingSystemBuilder.addStorage(new SDCardStorage());

            if(packageType.equalsIgnoreCase("Basic")) ticketingSystemBuilder.addController(new ATMega32Controller());
            else ticketingSystemBuilder.addController(new ArduinoController());
        }

        else {
            ticketingSystemBuilder.addIdentification(new NFCCard());
            ticketingSystemBuilder.addStorage(new RaspberryPiStorage());   
            ticketingSystemBuilder.addController(new TouchScreenController());
        }
    }
}