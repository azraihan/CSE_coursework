import java.util.Scanner;

public class Main {
    static boolean isPrime(int n) {
        if (n == 1) return true; // Incorrect
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int T = sc.nextInt();
        while (T-- > 0) {
            int n = sc.nextInt();
            if (isPrime(n))
                System.out.println("Prime");
            else
                System.out.println("Not Prime");
        }
        sc.close();
    }
}
