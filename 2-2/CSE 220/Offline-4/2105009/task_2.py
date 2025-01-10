import numpy as np
import matplotlib.pyplot as plt

import cmath
import time
import os

output_dir="output/task_2"
os.makedirs(output_dir, exist_ok=True)


def generate_random_signal(k=4, a=-5, b=5):
    n = 2 ** k
    return np.random.uniform(a, b, size=n)


def twiddle_factor(N,sign=-1):
    factor = cmath.exp(sign*1j * (2 * np.pi / N)) 
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
    return np.matmul(IDFT_matrix(N), frequency_domain_signal_column_matrix)/N

def FFT(time_domain_signal):
    if(len(time_domain_signal)==1):
        return time_domain_signal
    
    time_domain_signal_at_even_indices = time_domain_signal [::2]
    time_domain_signal_at_odd_indices = time_domain_signal [1::2]

    even_FFT=FFT(time_domain_signal_at_even_indices)
    odd_FFT=FFT(time_domain_signal_at_odd_indices)

    N = len(time_domain_signal)

    fft_result = np.zeros(N, dtype=complex)

    for k in range(N//2):
        tdl_fct = twiddle_factor(N) ** k

        fft_result[k] = even_FFT[k] + tdl_fct * odd_FFT[k]
        fft_result[k+N//2] = even_FFT[k] - tdl_fct * odd_FFT[k] 

    return fft_result


def inverse_fft(frequency_domain_signal):
    N = len(frequency_domain_signal)
    if N == 1:
        return frequency_domain_signal[0]
    
    even_part = frequency_domain_signal[::2]
    odd_part = frequency_domain_signal[1::2]
    
    even_IFFT = inverse_fft(even_part)
    odd_IFFT = inverse_fft(odd_part)
    
    ifft_result = np.zeros(N, dtype=complex)
    
    for k in range(N // 2):
        tdl_fct = twiddle_factor(N,sign=1) ** k
        ifft_result[k] = even_IFFT[k] + tdl_fct * odd_IFFT[k]
        ifft_result[k + N // 2] = even_IFFT[k] - tdl_fct * odd_IFFT[k]
    
    return ifft_result

def IFFT(frequency_domain_signal):
    N = len(frequency_domain_signal)
    # Compute the IFFT
    ifft_result = inverse_fft(frequency_domain_signal)
    # Scale the result by 1/N
    return [x / N for x in ifft_result]


def plot_time_comparison(x, y1, y2, label1, label2, xlabel, ylabel, title, xscale='linear', yscale="linear", y_scale_base=10, x_scale_base=10):
    plt.figure(figsize=(8, 6))
    plt.plot(x, y1, color="b", label=label1, marker="o")
    plt.plot(x, y2, color="r", label=label2, marker="s")
    plt.title(title, fontsize=14)
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)

    if(yscale=="log"): plt.yscale(yscale,base=y_scale_base)
    elif(yscale=="linear"): plt.yscale(yscale)

    if(xscale=="log"):plt.xscale(xscale,base=x_scale_base)
    elif(xscale=="linear"): plt.yscale(xscale)

    plt.grid(alpha=0.5, which="both", linestyle="--")
    plt.legend(fontsize=10)
    plt.tight_layout()
    plt.savefig(f"{output_dir}/{title}")


def test_func(func, k=10, max_iterations=10,test_arr=None):
    time_array=np.zeros(k+1, dtype=np.float64)
    results=[]
    for exp in range(k+1):
        if test_arr==None:
            input=generate_random_signal(exp)
        else:
            input=test_arr[exp]
        start=time.time()
        for i in range(max_iterations):
            result=func(input)
        end=time.time()
        results.append(np.array(result).reshape(-1,1))
        time_array[exp]=(end-start)/10
    return time_array, results


result_fft_time, results_fft=test_func(FFT)
result_dft_time, results_dft=test_func(DFT)

result_ifft_time, results_ifft=test_func(IFFT,test_arr=results_fft)
result_idft_time, results_idft=test_func(IDFT,test_arr=results_dft)


n= np.linspace(0,10,11) # testing from 2^0 to 2^10
x=2**n # x range


plot_time_comparison(x=x, 
                     y1=result_fft_time, y2=result_dft_time, 
                     label1="FFT", label2="DFT", 
                     xlabel="n", ylabel="Time (s)", 
                     title="FFT vs DFT (Linear Scale)")
plot_time_comparison(x=x, 
                     y1=result_fft_time, y2=result_dft_time, 
                     label1="FFT", label2="DFT", 
                     xlabel="n", ylabel="Time (s)", 
                     title="FFT vs DFT (Logarithmic Scale)",
                     xscale="log", yscale="log",
                     y_scale_base=10, x_scale_base=2)


plot_time_comparison(x=x, 
                     y1=result_ifft_time, y2=result_idft_time, 
                     label1="IFFT", label2="IDFT", 
                     xlabel="n", ylabel="Time (s)", 
                     title="IFFT vs IDFT (Linear Scale)")
plot_time_comparison(x=x, 
                     y1=result_ifft_time, y2=result_idft_time, 
                     label1="IFFT", label2="IDFT", 
                     xlabel="n", ylabel="Time (s)", 
                     title="IFFT vs IDFT (Logarithmic Scale)",
                     xscale="log", yscale="log",
                     y_scale_base=10, x_scale_base=2)