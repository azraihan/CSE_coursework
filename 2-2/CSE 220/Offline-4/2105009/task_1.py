import numpy as np
import matplotlib.pyplot as plt

##
import cmath
import os
import pandas as pd


n=50
samples = np.arange(n) 
sampling_rate=100
wave_velocity=8000

##
output_dir="output/task_1"
os.makedirs(output_dir, exist_ok=True)


#use this function to generate signal_A and signal_B with a random shift
def generate_signals(frequency=5, noise_freqs_a=[15, 30, 45], amplitudes_a = [0.5, 0.3, 0.1], noise_freqs_b = [10, 20, 40], amplitudes_b = [0.3, 0.2, 0.1]):

    noise_freqs = noise_freqs_a # Default noise frequencies in Hz

    amplitudes = amplitudes_a  # Default noise amplitudes
    noise_freqs2 = noise_freqs_b
    amplitudes2 = amplitudes_b
    
     # Discrete sample indices
    dt = 1 / sampling_rate  # Sampling interval in seconds
    time = samples * dt  # Time points corresponding to each sample

    # Original clean signal (sinusoidal)
    original_signal = np.sin(2 * np.pi * frequency * time)

    # Adding noise
    noise_for_sigal_A = sum(amplitude * np.sin(2 * np.pi * noise_freq * time)
                for noise_freq, amplitude in zip(noise_freqs, amplitudes))
    noise_for_sigal_B = sum(amplitude * np.sin(2 * np.pi * noise_freq * time)
                for noise_freq, amplitude in zip(noise_freqs2, amplitudes2))
    signal_A = original_signal + noise_for_sigal_A 
    noisy_signal_B = signal_A + noise_for_sigal_B

    # Applying random shift
    shift_samples = np.random.randint(-n // 2, n // 2)  # Random shift
    signal_B = np.roll(noisy_signal_B, shift_samples)
    
    return signal_A, signal_B, shift_samples

#implement other functions and logic

def stem_plot( y, title, x_label, y_label, color="b", x=None):
    if(x is None): x = np.arange(len(y))
    plt.figure(figsize=(8, 7))
    plt.stem(x, y, linefmt=f'{color}-', markerfmt=f'{color}o', basefmt=" ")
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.savefig(f"{output_dir}/{title}")


def twiddle_factor(N):
    factor = cmath.exp(-1j * (2 * np.pi / N)) 
    return complex(round(factor.real, 15), round(factor.imag, 15)) # handling precision

def DFT_matrix(N):
    DFT_mat=np.zeros((N,N),dtype=complex)

    for row in range(N):
        for col in range(N):
            DFT_mat[row][col]=twiddle_factor(N)**(row*col)

    return DFT_mat

def IDFT_matrix(N):
    DFT_mat = DFT_matrix(N)
    return np.conj(DFT_mat)

def form_signal_column_matrix(signal):
    return np.reshape(signal, (-1,1))

def DFT(time_domain_signal):
    N=len(time_domain_signal)
    return np.dot(DFT_matrix(N), form_signal_column_matrix(time_domain_signal))

def IDFT(frequency_domain_signal_column_matrix):
    N=len(frequency_domain_signal_column_matrix)
    return np.dot(IDFT_matrix(N), frequency_domain_signal_column_matrix)/N

def calculate_cross_correlation(signal_a, signal_b):
    DFT_A=DFT(signal_a)
    DFT_B=DFT(signal_b)
    cross_correlation= IDFT(DFT_A* np.conj(DFT_B))

    x_range= np.linspace(0,len(cross_correlation)-1)
    x_range[x_range> len(x_range)/2]-= len(x_range)
    x_range=-x_range

    return cross_correlation, x_range

def sample_lag_detection(cross_correlation, x_range):
    return int(x_range[np.argmax(np.abs(cross_correlation.real))])

def calculate_distance(sample_lag, sampling_rate, wave_velocity):
    return np.abs(sample_lag)/sampling_rate * wave_velocity

def denoise_signal(signal, sampling_rate, cutoff_frequency):
    N = len(signal)
    DFT_signal = DFT(signal).flatten()

    # Frequency bins
    k = np.arange(N)
    
    freqs = k * sampling_rate / N

    freqs[k >= N//2] -= sampling_rate
    # low-pass filter
    DFT_signal_filtered = DFT_signal.copy()
    DFT_signal_filtered[np.abs(freqs) > cutoff_frequency] = 0

    # IDFT
    denoised_signal = IDFT(DFT_signal_filtered.reshape(-1,1))
    return denoised_signal.real.flatten()




signal_a, signal_b, shift_samples = generate_signals()


stem_plot(signal_a, title="Signal A (Station A)", x_label="Sample Index", y_label="Amplitude", color="b")
DFT_A=DFT(signal_a)
stem_plot(np.abs(DFT_A), title="Frequency Spectrum of Signal A", x_label="Frequency", y_label="Magnitude", color="b")


stem_plot(signal_b, title="Signal B (Station B)", x_label="Sample Index", y_label="Amplitude", color="r")
DFT_B=DFT(signal_b)
stem_plot(np.abs(DFT_B), title="Frequency Spectrum of Signal B", x_label="Frequency", y_label="Magnitude", color="r")


cross_correlation, x_range = calculate_cross_correlation(signal_a, signal_b)
stem_plot(cross_correlation.real, title="DFT Based Cross-Correlation", x_label="Lag", y_label="Correlation", color="g", x=x_range)


sample_lag=sample_lag_detection(cross_correlation, x_range)
print(f"Expected Sample Shift: {shift_samples}\nCalculated Sample Lag: {sample_lag}")
distance= calculate_distance(sample_lag, sampling_rate, wave_velocity)
print(f"Distance Between Stations: {distance}")


### TESTING AFTER DENOISING

print("\n\n-------------\nTESTING AFTER DENOISING\n-------------\n\n")

denoised_signal_a = denoise_signal(signal_a, sampling_rate, cutoff_frequency=10)
denoised_signal_b = denoise_signal(signal_b, sampling_rate, cutoff_frequency=8)
# Plot the denoised signals
stem_plot(denoised_signal_a, title="Denoised Signal A (Station A)", x_label="Sample Index", y_label="Amplitude", color="b")
stem_plot(denoised_signal_b, title="Denoised Signal B (Station B)", x_label="Sample Index", y_label="Amplitude", color="r")


dft_denoised_signal_a = DFT(denoised_signal_a)
dft_denoised_signal_b = DFT(denoised_signal_b)

# Plot the frequency spectrum of denoised signals
stem_plot(np.abs(dft_denoised_signal_a), title="Frequency Spectrum of Denoised Signal A", x_label="Frequency", y_label="Magnitude", color="b")

stem_plot(np.abs(dft_denoised_signal_b), title="Frequency Spectrum of Denoised Signal A", x_label="Frequency", y_label="Magnitude", color="r")


cross_correlation_denoised, x_range_denoised = calculate_cross_correlation(denoised_signal_a, denoised_signal_b)
stem_plot(cross_correlation_denoised.real, title="DFT Based Cross-Correlation of Denoised Signal", x_label="Lag", y_label="Correlation", color="g", x=x_range_denoised)

sample_lag_denoised=sample_lag_detection(cross_correlation_denoised, x_range_denoised)
print(f"Expected Sample Shift: {shift_samples}\nCalculated Sample Lag: {sample_lag_denoised}")
distance_denoised= calculate_distance(sample_lag_denoised, sampling_rate, wave_velocity)
print(f"Distance Between Stations: {distance_denoised}")


### EXTENSIVE TESTING
print("\n\n-------------\nEXTENSIVE TESTING\n-------------\n\n")

def test(noise_freqs_list_a, amplitudes_list_a, noise_freqs_list_b, amplitudes_list_b):
    if(not(len(noise_freqs_list_a)==len(amplitudes_list_a)==len(noise_freqs_list_b)==len(amplitudes_list_b))):
        return
    df=pd.DataFrame(columns=["Noise A", "Noise Amplitude A", "Noise B", "Noise Amplitude B", "Expected Lag", "Lag (Noisy Signal)", 
                             "Distance (Noisy Signal)", "Lag (Filtered Signal)", "Distance (Filtered Signal)"])
    for noise_a, amp_a, noise_b, amp_b in zip(noise_freqs_list_a, amplitudes_list_a, noise_freqs_list_b, amplitudes_list_b):
        if(len(noise_a)==len(amp_a) and len(noise_b)==len(amp_a)):
            sig_a, sig_b, shift=generate_signals(frequency=5,
                                                noise_freqs_a=noise_a,
                                                amplitudes_a=amp_a,
                                                noise_freqs_b=noise_b,
                                                amplitudes_b=amp_b)
            
            cross_correlation, x_range = calculate_cross_correlation(sig_a, sig_b)
            # stem_plot(cross_correlation.real, title="DFT Based Cross-Correlation", x_label="Lag", y_label="Correlation", color="g", x=x_range)
            sample_lag=sample_lag_detection(cross_correlation, x_range)
            distance= calculate_distance(sample_lag, sampling_rate, wave_velocity)


            denoised_signal_a = denoise_signal(sig_a, sampling_rate, cutoff_frequency=10)
            denoised_signal_b = denoise_signal(sig_b, sampling_rate, cutoff_frequency=8)
            cross_correlation_denoised, x_range_denoised = calculate_cross_correlation(denoised_signal_a, denoised_signal_b)
            # stem_plot(cross_correlation.real, title="DFT Based Cross-Correlation", x_label="Lag", y_label="Correlation", color="g", x=x_range)
            sample_lag_denoised=sample_lag_detection(cross_correlation_denoised, x_range_denoised)
            distance_denoised= calculate_distance(sample_lag_denoised, sampling_rate, wave_velocity)

            new_row = pd.DataFrame([{
                "Noise A": noise_a,
                "Noise Amplitude A": amp_a,
                "Noise B": noise_b,
                "Noise Amplitude B": amp_b,
                "Expected Lag": shift,
                "Lag (Noisy Signal)": sample_lag,
                "Distance (Noisy Signal)": distance,
                "Lag (Filtered Signal)": sample_lag_denoised,
                "Distance (Filtered Signal)": distance_denoised
                }])
            df = pd.concat([df, new_row], ignore_index=True)

    return df


np.random.seed(52)
def generate_random_noise_freq_and_amp(k=3, no_of_samples=3, freq_lower_bound=10, freq_upper_bound=50, amp_lower_bound=0, amp_upper_bound=3):
    noise_freqs_list_a = []
    amplitudes_list_a = []
    noise_freqs_list_b = []
    amplitudes_list_b = []
    for _ in range(k):
        noise_freqs_list_a.append(np.random.randint(freq_lower_bound, freq_upper_bound, no_of_samples))
        amplitudes_list_a.append(np.round(np.random.uniform(amp_lower_bound, amp_upper_bound, no_of_samples),1))
        noise_freqs_list_b.append(np.random.randint(freq_lower_bound, freq_upper_bound, no_of_samples))
        amplitudes_list_b.append(np.round(np.random.uniform(amp_lower_bound, amp_upper_bound, no_of_samples),1))

    return noise_freqs_list_a, amplitudes_list_a, noise_freqs_list_b, amplitudes_list_b

noise_freqs_list_a, amplitudes_list_a, noise_freqs_list_b, amplitudes_list_b = generate_random_noise_freq_and_amp(k=10)

df = test(noise_freqs_list_a=noise_freqs_list_a,
          amplitudes_list_a=amplitudes_list_a,
          noise_freqs_list_b=noise_freqs_list_b,
          amplitudes_list_b=amplitudes_list_b)

pd.set_option('display.max_columns', None)  # Show all columns
pd.set_option('display.width', 1000)        # Set the width of the display
pd.set_option('display.max_colwidth', 50)  # Limit the column width for better readability

print(df)