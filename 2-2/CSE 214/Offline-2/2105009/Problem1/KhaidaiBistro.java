import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.stream.Collectors;
import java.util.stream.Stream;

interface MenuItem {
    String getName();
    int getPrice();
}

class FoodItem implements MenuItem {
    private String name;
    private int price;

    public FoodItem(String name, int price) {
        this.name = name;
        this.price = price;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getPrice() {
        return price;
    }
}

class Combo implements MenuItem {
    private String name;
    private List<MenuItem> items;
    private List<MenuItem> freeItems;
    private int discount;

    public Combo(String name) {
        this.name = name;
        this.items = new ArrayList<>();
        this.freeItems = new ArrayList<>();
        this.discount = 0; // No discount by default
    }

    public void addItem(MenuItem item) {
        items.add(item);
    }

    public void removeItem(MenuItem item) {
        items.remove(item);
        freeItems.remove(item);
    }

    public void setDiscount(int discountPercentage) {
        this.discount = discountPercentage;
    }
    public int getDiscount() {
        return discount;
    }

    public void addFreeItem(MenuItem item) {
        freeItems.add(item); // Add the item and mark it free
    }

    @Override
    public String getName() {
        return name;
    }

    public String getItems(){
        String details="";
        List<MenuItem> allItems = Stream.concat(items.stream(), freeItems.stream())
                                          .collect(Collectors.toList());
        for (int i=0; i<allItems.size();i++) {
            if(i==items.size()-1) details=details+items.get(i).getName();
            else details=details+items.get(i).getName()+" + ";
        }
        return details;
    }

    @Override
    public int getPrice() {
        int total = 0;
        for (MenuItem item : items) {
            total += item.getPrice();
        }
        return total - (total * discount / 100); // Apply discount
    }
    public int getActualPrice() {
        int total = 0;
        for (MenuItem item : items) {
            total += item.getPrice();
        }
        return total ;
    }

    public void printComboDetails() {
        System.out.println(name);
        for (MenuItem item : items) {
            if(item.getClass().getName()=="FoodItem") System.out.println("\t- " + item.getName());
            if(item.getClass().getName()=="Combo") System.out.println("\t- " + item.getName()+ " ("+((Combo)item).getItems()+")");
        }
        for (MenuItem item : freeItems) {
            System.out.println("\t- " + item.getName()+ " (Free!!)");
        }
        System.out.println();
        System.out.println("Total - " + getActualPrice());
        System.out.println("Discount - "+ getDiscount());
        System.out.println("Discounted Price - " + getPrice());
    }
}

class Menu {
    private Map<String, MenuItem> items;

    public Menu() {
        items = new HashMap<>();
        initializeMenu();
    }

    private void initializeMenu() {
        // Add individual items to the menu
        items.put("Burger", new FoodItem("Burger", 300));
        items.put("Fries", new FoodItem("Fries", 100));
        items.put("Wedges", new FoodItem("Wedges", 150));
        items.put("Shawarma", new FoodItem("Shawarma", 200));
        items.put("Drink", new FoodItem("Drink", 25));

        // Add predefined combos
        Combo combo1 = new Combo("Combo1");
        combo1.addItem(items.get("Burger"));
        combo1.addItem(items.get("Fries"));
        combo1.addItem(items.get("Drink"));
        items.put("Combo1", combo1);

        Combo combo2 = new Combo("Combo2");
        combo2.addItem(items.get("Shawarma"));
        combo2.addItem(items.get("Drink"));
        items.put("Combo2", combo2);
    }

    public void addCombo(String name, Combo combo) {
        items.put(name, combo);
    }

    public MenuItem getItem(String name) {
        return items.get(name);
    }

    public void viewMenu() {
        System.out.println("Menu:");
        for (MenuItem item : items.values()) {
            if(item.getClass().getName().equals("FoodItem")) System.out.println(item.getName() + " - " + item.getPrice() + "tk");
            else if(item.getClass().getName().equals("Combo")) System.out.println(item.getName()+ " ("+((Combo)item).getItems()+")" + " - " + item.getPrice() + "tk");
            
        }
    }
}

public class KhaidaiBistro {
    public static void main(String[] args) {
        Menu menu = new Menu();
        menu.viewMenu();

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\nPress 1 to create a combo, 2 to view menu, and 0 to exit\n");
            int choice = scanner.nextInt();
            scanner.nextLine(); // Consume newline

            if (choice == 0) {
                break;
            } else if (choice == 1) {
                System.out.print("\nEnter the name of the combo: ");
                String comboName = scanner.nextLine();
                Combo customCombo = new Combo(comboName);

                System.out.println("\nAvailable commands:");
                System.out.println("Add [item], Remove [item], Free [item], Discount [percentage], Done");

                while (true) {
                    System.out.print("> ");
                    String[] command = scanner.nextLine().split(" ");

                    if (command[0].equalsIgnoreCase("Done")) {
                        break;
                    } else if (command[0].equalsIgnoreCase("Add")) {
                        MenuItem item = menu.getItem(command[1]);
                        if (item != null) {
                            customCombo.addItem(item);
                        }
                    } else if (command[0].equalsIgnoreCase("Remove")) {
                        MenuItem item = menu.getItem(command[1]);
                        if (item != null) {
                            customCombo.removeItem(item);
                        }
                    } else if (command[0].equalsIgnoreCase("Free")) {
                        MenuItem item = menu.getItem(command[1]);
                        if (item != null) {
                            customCombo.addFreeItem(item);
                        }
                    } else if (command[0].equalsIgnoreCase("Discount")) {
                        int discount = Integer.parseInt(command[1]);
                        customCombo.setDiscount(discount);
                    }
                }

                menu.addCombo(comboName, customCombo);
                customCombo.printComboDetails();

                System.out.println("\n");
            } else if (choice == 2) {
                menu.viewMenu();
            }
        }

        scanner.close();
    }
}
