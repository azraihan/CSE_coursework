import numpy as np
import matplotlib.pyplot as plt

##
import os


# class for signal pulse implementation
class WavePulse:
    def __init__(self, x_range=(-10, 10), x_pulse=(-2,2), num_points=1000):
        """
        Initialize the WaveFunctions class with x range and number of points.
        
        Parameters:
        x_range (tuple): Range of x values (min, max).
        x_pulse (tuple): Range of pulse (min, max).
        num_points (int): Number of points in the x range.
        """
        self.x_values = np.linspace(x_range[0], x_range[1], num_points)
        self.x_range = x_range
        self.x_pulse = x_pulse
    
    def parabolic(self):
        """
        Generate a parabolic function: y = x^2 within [-2, 2], and 0 elsewhere.
        """
        pulse_lower_limit = self.x_pulse[0]
        pulse_upper_limit = self.x_pulse[1]
        y_values = np.where((self.x_values >= pulse_lower_limit) & (self.x_values <= pulse_upper_limit),
                            self.x_values**2, 0)
        return y_values
    
    def triangular(self):
        """
        Generate a triangular wave within [-2, 2] with height 1 and 0 elsewhere.
        """
        pulse_lower_limit = self.x_pulse[0]
        pulse_upper_limit = self.x_pulse[1]
        y_values = np.where(
            (self.x_values >= pulse_lower_limit) & (self.x_values <= 0),
            (self.x_values - pulse_lower_limit) / abs(pulse_lower_limit),  # Slope +1
            np.where(
                (self.x_values > 0) & (self.x_values <= pulse_upper_limit),
                (pulse_upper_limit - self.x_values) / pulse_upper_limit,  # Slope -1
                0
            )
        )
        return y_values
    
    def sawtooth(self):
        """
        Generate a sawtooth wave within [-2, 2] with a slope of 1 and 0 elsewhere.
        """
        pulse_lower_limit = self.x_pulse[0]
        pulse_upper_limit = self.x_pulse[1]
        y_values = np.where(
            (self.x_values >= pulse_lower_limit) & (self.x_values <= pulse_upper_limit),
            (self.x_values - pulse_lower_limit),  # Slope of 1
            0
        )
        return y_values

    
    def rectangular(self):
        """
        Generate a rectangular pulse within [-2, 2] with height 1 and 0 elsewhere.
        """
        pulse_lower_limit = self.x_pulse[0]
        pulse_upper_limit = self.x_pulse[1]
        y_values = np.where(
            (self.x_values >= pulse_lower_limit) & (self.x_values <= pulse_upper_limit),
            1,
            0
        )
        return y_values
    
    def get_wave_pulse(self, wave_type):
        """
        Return the specified wave pulse function based on the input string.
        
        Parameters:
        wave_type (str): The type of wave pulse ('parabolic', 'triangular', 'sawtooth', 'rectangular').
        
        Returns:
        np.array: The y-values of the specified wave function.
        """
        if wave_type == 'parabolic':
            return self.parabolic()
        elif wave_type == 'triangular':
            return self.triangular()
        elif wave_type == 'sawtooth':
            return self.sawtooth()
        elif wave_type == 'rectangular':
            return self.rectangular()
        else:
            raise ValueError(f"Unknown wave type: {wave_type}")


def fourier_transform(signal, frequencies, sampled_times):
    omega_t = 2 * np.pi * np.outer(frequencies, sampled_times)
    exp_matrix = np.exp(-1j * omega_t)
    integrand = exp_matrix * signal
    # print(integrand.shape)
    # Compute integral over time axis (axis=1) using trapezoidal rule
    integral = np.trapz(integrand, sampled_times, axis=1)
    # Return real and imaginary parts
    ft_real = integral.real
    ft_imag = integral.imag
    return np.array([ft_real, ft_imag])


# Inverse Fourier Transform implementation
def inverse_fourier_transform(ft_signal, frequencies, sampled_times):
    df = frequencies[1] - frequencies[0]  # assuming uniform spacing
    ft_complex = ft_signal[0] + 1j * ft_signal[1]
    # Create a matrix of frequency * time
    omega_t = 2 * np.pi * np.outer(frequencies, sampled_times)
    exp_matrix = np.exp(1j * omega_t)
    # Multiply ft_signal with exp_matrix and integrate over frequency
    integrand = exp_matrix.T * ft_complex
    # Compute integral over frequency axis (axis=1)
    integral = np.trapz(integrand, frequencies, axis=1)
    reconstructed_signal = integral.real
    return reconstructed_signal



wave_pulses=WavePulse()

output_dir='output/ft_basic'
os.makedirs(output_dir, exist_ok=True)

for pulse_type in ['parabolic', 'triangular', 'sawtooth', 'rectangular']:
    #make a dirrectory to save the graphs
    graph_output_dir=f'{output_dir}/{pulse_type}'
    os.makedirs(graph_output_dir, exist_ok=True)


    # Define the interval and parabolic function
    x_values = wave_pulses.x_values # Larger range for better visualization
    y_values = wave_pulses.get_wave_pulse(pulse_type) # Parabolic function y = x^2 within [-2, 2]

    # Plot the original parabolic function
    plt.figure(figsize=(12, 4))
    plt.plot(x_values, y_values, label=f"Original ({pulse_type})")
    plt.title(f"Original Function ({pulse_type})")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.legend()
    # plt.show()

    #saving
    plt.savefig(f'{graph_output_dir}/{pulse_type}_original.png')

    # Define the sampled times and frequencies
    sampled_times = x_values  # Ensure sampled_times matches x_values
    frequency_ranges=[(-1,1),(-2,2),(-5,5)]

    for frequency_range in frequency_ranges:
        frequencies = np.linspace(frequency_range[0], frequency_range[1], 1000)  # Frequency range for the Fourier Transform
        # Apply Fourier Transform to the sampled data
        ft_data = fourier_transform(y_values, frequencies, sampled_times)

        # # Plot the frequency spectrum
        # plt.figure(figsize=(12, 6))
        # magnitude = np.sqrt(ft_data[0]**2 + ft_data[1]**2)  # Magnitude of Fourier Transform
        # plt.plot(frequencies, magnitude, label="Magnitude of FT")
        # plt.title(f"Frequency Spectrum of {pulse_type} function")
        # plt.xlabel("Frequency (Hz)")
        # plt.ylabel("Magnitude")
        # plt.legend()
        # plt.show()

        # # Reconstruct the signal from the Fourier Transform data
        # reconstructed_y_values = inverse_fourier_transform(ft_data, frequencies, sampled_times)

        # # Plot the original and reconstructed functions for comparison
        # plt.figure(figsize=(12, 4))
        # plt.plot(x_values, y_values, label=f"Original ({pulse_type})", color="blue")
        # plt.plot(sampled_times, reconstructed_y_values, label=f"Reconstructed ({pulse_type}) function", color="red", linestyle="--")
        # plt.title(f"Original vs Reconstructed Function ({pulse_type})")
        # plt.xlabel("x")
        # plt.ylabel("y")
        # plt.legend()
        # plt.show()

        fig, axs = plt.subplots(nrows=2, figsize=(12, 10))

            # Plot the frequency spectrum
        magnitude = np.sqrt(ft_data[0]**2 + ft_data[1]**2)  # Magnitude of Fourier Transform
        axs[0].plot(frequencies, magnitude, label="Magnitude of FT")
        axs[0].set_title(f"Frequency Spectrum of {pulse_type} function")
        axs[0].set_xlabel("Frequency (Hz)")
        axs[0].set_ylabel("Magnitude")
        axs[0].legend()

        # Reconstruct the signal from the Fourier Transform data
        reconstructed_y_values = inverse_fourier_transform(ft_data, frequencies, sampled_times)

        # Plot the original and reconstructed functions for comparison
        axs[1].plot(x_values, y_values, label=f"Original ({pulse_type})", color="blue")
        axs[1].plot(sampled_times, reconstructed_y_values, label=f"Reconstructed ({pulse_type}) function", color="red", linestyle="--")
        axs[1].set_title(f"Original vs Reconstructed Function ({pulse_type})")
        axs[1].set_xlabel("x")
        axs[1].set_ylabel("y")
        axs[1].legend()

        # Adjust layout and display
        # Add an overall figure title
        fig.suptitle(f"Frequency Range {frequency_range[0]} to {frequency_range[1]}", fontsize=16)

        # Adjust layout and display
        plt.tight_layout(rect=[0, 0.03, 1, 0.95])  # Leave space for the suptitle and caption
        # plt.show()

        #saving
        plt.savefig(f'{graph_output_dir}/{pulse_type}_range_{frequency_range[0]}_to_{frequency_range[1]}.png')
