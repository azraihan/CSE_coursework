import numpy as np
import scipy.io.wavfile as wavfile
import matplotlib.pyplot as plt

##
import os


#making output directory
output_dir='output/denoising'
os.makedirs(output_dir, exist_ok=True)


# Step 1: Load the audio file
sample_rate, data = wavfile.read('buzzjc.wav')
data = data / np.max(np.abs(data))  # Normalize to -1 to 1

# If stereo, convert to mono by averaging channels
if len(data.shape) > 1:
    data = data.mean(axis=1)

# Step 1.1: Plot the original audio signal in the time domain
plt.figure(figsize=(12, 4))
time = np.linspace(0, len(data) / sample_rate, num=len(data))
plt.plot(time, data)
plt.title("Original Audio Signal (Time Domain)")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
# plt.show()
plt.savefig(f'{output_dir}/original.png')

# Set parameters for interval sampling and FT
interval_step = 1  # Adjust for sampling every 'interval_step' data points  
data_sampled = data[::interval_step]
max_time = len(data_sampled) / (sample_rate / interval_step)
sampled_times = np.linspace(0, max_time, num=len(data_sampled))


max_freq = sample_rate / (2 * interval_step)
num_freqs = len(data_sampled)
frequencies = np.linspace(0, max_freq, num=num_freqs)

# Step 2: Apply Fourier Transform using trapezoidal integration
def fourier_transform(signal, frequencies, sampled_times):
    omega_t = 2 * np.pi * np.outer(frequencies, sampled_times)
    exp_matrix = np.exp(-1j * omega_t)
    integrand = exp_matrix * signal
    print(integrand.shape)
    # Compute integral over time axis (axis=1) using trapezoidal rule
    integral = np.trapz(integrand, sampled_times, axis=1)
    # Return real and imaginary parts
    ft_real = integral.real
    ft_imag = integral.imag
    return np.array([ft_real, ft_imag])
    

# Apply FT with trapezoidal integration
ft_data = fourier_transform(data_sampled, frequencies, sampled_times)

# Step 2.1: Visualize the frequency spectrum
plt.figure(figsize=(12, 6))
plt.plot(frequencies, np.sqrt(ft_data[0]**2 + ft_data[1]**2))
plt.title("Frequency Spectrum of the Audio Signal (Custom FT with Trapezoidal Integration)")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude")
# plt.show()
plt.savefig(f'{output_dir}/frequency_spectrum.png')

# Step 3: Filter out unwanted noise frequencies
filtered_ft_data= np.zeros((2, num_freqs))
filtered_ft_data[0] = ft_data[0].copy()
filtered_ft_data[1] = ft_data[1].copy()

# Try to filter out the frequencies for which you get the best result.
# Experiment with different ideas like make the values for low frequencies zero, or make high frequencies zero, or make a range of frequencies zero. 
magnitude=np.sqrt(ft_data[0]**2 + ft_data[1]**2)

# # Set the threshold
# threshold = 0.1 * np.max(magnitude)  # Adjust threshold as needed

# # Manually find peaks in the magnitude spectrum
# peaks_indices = []

# # Loop through the magnitude array to find peaks
# for i in range(1, len(magnitude) - 1):
#     if (magnitude[i] > magnitude[i - 1]) and (magnitude[i] > magnitude[i + 1]) and (magnitude[i] > threshold):
#         peaks_indices.append(i)

# peaks_indices = np.array(peaks_indices)
# peak_frequencies = frequencies[peaks_indices]
# peak_magnitudes = magnitude[peaks_indices]

# # Sort peaks by magnitude
# sorted_peak_frequencies = np.sort(peak_frequencies)[::-1]
# sorted_peak_magnitudes = np.sort(peak_magnitudes)[::-1]

# # print("Top peak frequencies:")
# # for freq, mag in zip(sorted_peak_frequencies[:10], sorted_peak_magnitudes[:10]):
# #     print(f"Frequency: {freq:.2f} Hz, Magnitude: {mag:.4f}")

# # Filter out the top peaks (noise frequencies)
# num_peaks_to_filter = len(peaks_indices)  # Adjust as needed based on observed peaks
# freq_bandwidth = 150  # Hz

# for i in range(num_peaks_to_filter):
#     nf = sorted_peak_frequencies[i]
#     indices = np.where((frequencies >= nf - freq_bandwidth) & (frequencies <= nf + freq_bandwidth))[0]
#     filtered_ft_data[0][indices] = 0
#     filtered_ft_data[1][indices] = 0
#     print(f"Filtering frequency around: {nf:.2f} Hz")

filter_mask = np.where(frequencies <= 1000)

# Set corresponding values in filtered_ft_data to zero
filtered_ft_data[0][filter_mask] = 0
filtered_ft_data[1][filter_mask] = 0


# Step 3.1: Visualize the filtered frequency spectrum
plt.figure(figsize=(12, 6))
plt.plot(frequencies, np.sqrt(filtered_ft_data[0]**2 + filtered_ft_data[1]**2))
plt.title("Filtered Frequency Spectrum (Unwanted Frequencies Removed)")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude")
# plt.show()
plt.savefig(f'{output_dir}/frequency_spectrum_filtered.png')

# Step 4: Apply Inverse Fourier Transform using trapezoidal integration
def inverse_fourier_transform(ft_signal, frequencies, sampled_times):
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

# Step 4.1: Reconstruct the signal using IFT
filtered_data = inverse_fourier_transform(filtered_ft_data, frequencies, sampled_times)

# Step 4.2: Plot the reconstructed audio signal
plt.figure(figsize=(12, 4))
plt.plot(sampled_times, filtered_data)
plt.title("Reconstructed (Denoised) Audio Signal (Time Domain)")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
# plt.show()
plt.savefig(f'{output_dir}/reconstructed.png')

# Step 5: Normalize and save the denoised audio
filtered_data = np.int16(filtered_data / np.max(np.abs(filtered_data)) * 32767)  # Convert to int16 format for WAV
wavfile.write(f'{output_dir}/denoised_audio.wav', sample_rate, filtered_data)

print("Denoised audio saved as 'denoised_audio.wav'")


