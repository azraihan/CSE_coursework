import numpy as np
import matplotlib.pyplot as plt
import os

class FourierSeries:
    def __init__(self, func, L, terms=10):
        """
        Initialize the FourierSeries class with a target function, period L, and number of terms.
        
        Parameters:
        - func: The target function to approximate.
        - L: Half the period of the target function.
        - terms: Number of terms to use in the Fourier series expansion.
        """
        self.func=func
        self.L=L
        self.terms=terms

    def calculate_a0(self, N=1000):
        """
        Step 1: Compute the a0 coefficient, which is the average (DC component) of the function over one period.
        
        You need to integrate the target function over one period from -L to L.
        For that, you can use the trapezoidal rule with a set of points (N points here) for numerical integration.
        
        Parameters:
        - N: Number of points to use for integration (more points = more accuracy).
        
        Returns:
        - a0: The computed a0 coefficient.
        """
        a = -self.L # lower limit
        b = self.L # upper limit

        x = np.linspace(a, b, N + 1)
        y = self.func(x)
        dx = (b-a) / N

        a_0 = np.trapezoid(y, x, dx)/(2*self.L)

        return a_0


    def calculate_an(self, n, N=1000):
        """
        Step 2: Compute the an coefficient for the nth cosine term in the Fourier series.
        
        You need to integrate the target function times cos(n * pi * x / L) over one period.
        This captures how much of the nth cosine harmonic is present in the function.
        
        Parameters:
        - n: Harmonic number to calculate the nth cosine coefficient.
        - N: Number of points to use for numerical integration.
        
        Returns:
        - an: The computed an coefficient.
        """
        a = -self.L # lower limit
        b = self.L # upper limit

        x = np.linspace(a, b, N + 1)
        y = self.func(x)
        dx = (b-a) / N

        return np.trapezoid(y*np.cos((n*np.pi*x)/self.L),x,dx)/self.L

    def calculate_bn(self, n, N=1000):
        """
        Step 3: Compute the bn coefficient for the nth sine term in the Fourier series.
        
        You need to integrate the target function times sin(n * pi * x / L) over one period.
        This determines the contribution of the nth sine harmonic in the function.
        
        Parameters:
        - n: Harmonic number to calculate the nth sine coefficient.
        - N: Number of points to use for numerical integration.
        
        Returns:
        - bn: The computed bn coefficient.
        """

        a = -self.L # lower limit
        b = self.L # upper limit

        x = np.linspace(a, b, N + 1)
        y = self.func(x)
        dx = (b-a) / N

        return np.trapezoid(y*np.sin((n*np.pi*x)/self.L),x,dx)/self.L

    def approximate(self, x):
        """
        Step 4: Use the calculated coefficients to build the Fourier series approximation.

        For each term up to the specified number of terms, you need to calculate the sum of:
        a0 (the constant term) + cosine terms (an * cos(n * pi * x / L)) + sine terms (bn * sin(n * pi * x / L)).

        Parameters:
        - x: Points at which to evaluate the Fourier series.

        Returns:
        - The Fourier series approximation evaluated at each point in x.
        """
        # Compute a0 term
        a0 = self.calculate_a0()

        # Initialize the series with the a0 term
        # series = a0 / 2 * np.ones_like(x)
        series = a0 / 2

        # Compute each harmonic up to the specified number of terms
        for n in range(1, self.terms + 1):
            an = self.calculate_an(n)
            bn = self.calculate_bn(n)
            series += an * np.cos(n * np.pi * x / self.L)
            series += bn * np.sin(n * np.pi * x / self.L)

        return series





    def plot(self,title):
        """
        Step 5: Plot the original function and its Fourier series approximation.

        You need to calculate the Fourier series approximation over a set of points (x values) from -L to L.
        Then plot both the original function and the Fourier series to visually compare them.
        """
        # Generate points over one period
        x = np.linspace(-self.L, self.L, 1000)   ###??
        
        # Compute original function values
        original = self.func(x)
        
        # Compute Fourier series approximation
        approximation = self.approximate(x)

        # Plotting
        plt.figure(figsize=(10, 6))
        plt.plot(x, original, label="Original Function", color="blue")
        plt.plot(x, approximation, label=f"Fourier Series Approximation (N={self.terms})", color="red", linestyle="--")
        plt.xlabel("x")
        plt.ylabel("f(x)")
        plt.legend()
        plt.title(f"Fourier Series Approximation ({title})")
        plt.grid(True)

        # Ensure the 'Graphs' directory exists
        os.makedirs("Graphs", exist_ok=True)
        
        # Save the figure to the 'Graphs' directory with the title as the filename
        plt.savefig(f"Graphs/{title}.jpg")


        plt.show()



def target_function(x, function_type="square"):
    """
    Defines various periodic target functions that can be used for Fourier series approximation.
    
    Parameters:
    - x: Array of x-values for the function.
    - function_type: Type of function to use ("square", "sawtooth", "triangle", "sine", "cosine").
    
    Returns:
    - The values of the specified target function at each point in x.
    """
    if function_type == "square":
        # Square wave: +1 when sin(x) >= 0, -1 otherwise
        return np.where(np.sin(x) >= 0, 1, -1)
    
    elif function_type == "sawtooth":
        # Sawtooth wave: linearly increasing from -1 to 1 over the period
        # Normalize x to the range [-L, L]
        L = np.pi  # Assuming half-period is pi
        # return (x % (2 * L)) / L - 1  # Scaled to go from -1 to 1 over one period
        return 2*(x/(2*L)-np.floor(1/2+x/(2*L)))
    
    elif function_type == "triangle":
        # Triangle wave: periodic line with slope +1 and -1 alternately
        # Normalize x to the range [-L, L]
        # L = np.max(x)
        # return 2 * np.abs(x % (2*L) - L) - L
        # Triangle wave with peaks at (-pi, -pi/2), (0, pi/2), (pi, -pi/2)
        # x = np.mod(x + np.pi, 2 * np.pi) - np.pi  # Map x to [-pi, pi]
        L = np.max(x)
        a = L / 2
        p = 2 * L
        return 4*a/p * abs((((x - p/4) % p) + p) % p - p/2) - a
    
    elif function_type == "sine":
        # Pure sine wave
        return np.sin(x)
    
    elif function_type == "cosine":
        # Pure cosine wave
        return np.cos(x)
    
    else:
        raise ValueError("Invalid function_type. Choose from 'square', 'sawtooth', 'triangle', 'sine', or 'cosine'.")

# Example of using these functions in the FourierSeries class
if __name__ == "__main__":
    L = np.pi  # Half-period for all functions
    terms = 10  # Number of terms in Fourier series

    # Test each type of target function
    for function_type in ["square", "sawtooth", "triangle", "sine", "cosine"]:
        print(f"Plotting Fourier series for {function_type} wave:")
        
        # Define the target function dynamically
        fourier_series = FourierSeries(lambda x: target_function(x, function_type=function_type), L, terms)
        
        # Plot the Fourier series approximation
        fourier_series.plot(function_type)