import java.util.Scanner;

import Builder.TicketingSystemBuilder;
import Director.TicketingSystemDirector;

public class TicketingSystem {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        String packageType;
        String internetConnection;
        String framework;

        System.out.println("Enter the packageType");

        packageType = scanner.nextLine();

        System.out.println("Enter the internetconnection:");

        internetConnection = scanner.nextLine();

        if (packageType.equalsIgnoreCase("Basic") || packageType.equalsIgnoreCase("Standard")) {
            while (!(internetConnection.equalsIgnoreCase("WiFi") || internetConnection.equalsIgnoreCase("GSM"))) {
                System.out.println("ATMega32 and ArduinoMega only supports WiFi or GSM!\n");
                System.out.println("Enter the internetconnection:");

                internetConnection = scanner.nextLine();
            }
        }
        framework = scanner.nextLine();

        TicketingSystemBuilder ticketingSystemBuilder = new TicketingSystemBuilder();

        TicketingSystemDirector ticketingSystemDirector = new TicketingSystemDirector();
        ticketingSystemDirector.buildTicketingSystem(ticketingSystemBuilder, packageType, internetConnection,
                framework);

        ticketingSystemBuilder.getProduct().show();

        scanner.close();
    }
}
