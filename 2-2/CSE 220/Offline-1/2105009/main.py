import numpy as np 
import matplotlib.pyplot as plt 
import os  

class DiscreteSignal:
    """
    Class representing a discrete signal with finite support from -INF to INF.
    """

    def __init__(self, INF):
        """
        Initializes the DiscreteSignal instance.

        Parameters:
        INF (int): The maximum absolute time index. The signal is defined from -INF to INF.
        """
        self.INF = INF
        # Initialize the signal values to zeros; array size is 2*INF + 1 to include zero index
        self.values = np.zeros(2 * INF + 1)

    def set_value_at_defined_time(self, time, value):
        """
        Sets the signal value at a specific time index.

        Parameters:
        time (int): The time index at which to set the value (from -INF to INF).
        value (float): The value to set at the given time index.
        """
        index = time + self.INF  # Adjust index for zero-based indexing
        self.values[index] = value  # Set the value at the adjusted index

    def get_value_at_defined_time(self, time):
        """
        Retrieves the signal value at a specific time index.

        Parameters:
        time (int): The time index from which to get the value (from -INF to INF).

        Returns:
        float: The value at the given time index.
        """
        index = time + self.INF  # Adjust index for zero-based indexing
        return self.values[index]  # Return the value at the adjusted index

    def shift_signal(self, shift):
        """
        Shifts the signal by a specified number of time units.

        Parameters:
        shift (int): The number of units to shift the signal. Positive for right shift, negative for left shift.

        Returns:
        DiscreteSignal: A new DiscreteSignal instance representing the shifted signal.
        """
        newSignal = DiscreteSignal(self.INF)
        newSignal.values = np.copy(self.values)  # Copy the original signal values

        if shift > 0:
            # Right shift: prepend zeros and trim from the end
            newSignal.values = np.append(np.zeros(shift), newSignal.values)  # Add zeros to the left
            newSignal.values = newSignal.values[:len(newSignal.values)-shift]  # Trim the end
        elif shift < 0:
            # Left shift: append zeros and trim from the start
            newSignal.values = np.append(newSignal.values, np.zeros(-shift))  # Add zeros to the right
            newSignal.values = newSignal.values[-shift:]  # Trim the start
        # If shift == 0, do nothing
        return newSignal  # Return the shifted signal

    def add(self, other):
        """
        Adds another DiscreteSignal to this signal.

        Parameters:
        other (DiscreteSignal): The other signal to add.

        Returns:
        DiscreteSignal: A new DiscreteSignal instance representing the sum of the two signals.
        """
        newSignal = DiscreteSignal(self.INF)
        newSignal.values = self.values + other.values  # Element-wise addition of signal values
        return newSignal

    def multiply(self, other):
        """
        Multiplies this signal with another DiscreteSignal element-wise.

        Parameters:
        other (DiscreteSignal): The other signal to multiply with.

        Returns:
        DiscreteSignal: A new DiscreteSignal instance representing the product of the two signals.
        """
        newSignal = DiscreteSignal(self.INF)
        newSignal.values = self.values * other.values  # Element-wise multiplication
        return newSignal

    def multiply_signal_by_const_factor(self, scalar):
        """
        Multiplies the signal by a scalar constant.

        Parameters:
        scalar (float): The scalar value to multiply with.

        Returns:
        DiscreteSignal: A new DiscreteSignal instance representing the scaled signal.
        """
        newSignal = DiscreteSignal(self.INF)
        newSignal.values = self.values * scalar  # Multiply each value by the scalar
        return newSignal

    def plot(self):
        """
        Plots the discrete signal using a stem plot.

        The x-axis represents the time indices from -INF to INF.
        """
        # Generate time indices from -INF to INF
        time_indices = np.arange(-self.INF, self.INF + 1, 1)
        # Create a stem plot of the signal values
        plt.stem(time_indices, self.values)
        # Set x-ticks to show each integer time index
        plt.xticks(time_indices)
        # Set y-axis limits and ticks
        plt.ylim(-1, 4)
        plt.yticks(np.arange(-1, 5, 1))
        # Label the axes
        plt.xlabel('n (Time Index)')
        plt.ylabel('x[n]')
        # Enable grid for better readability
        plt.grid(True)

class ContinuousSignal:
    """
    Class representing a continuous-time signal over a finite interval.

    Attributes:
    PRECISION (int): Number of points to sample the function when plotting.
    INF (float): The signal is considered from -INF to INF.
    """

    PRECISION = 2000  # Number of points for plotting
    INF = 3  # Time range from -INF to INF

    def __init__(self, func):
        """
        Initializes the ContinuousSignal instance.

        Parameters:
        func (function): A function representing the signal, defined for real numbers.
        """
        self.func = func  # Store the function representing the signal

    def shift(self, shift):
        """
        Shifts the signal by a specified amount in time.

        Parameters:
        shift (float): The amount of time to shift the signal. Positive shifts the signal to the right.

        Returns:
        ContinuousSignal: A new ContinuousSignal instance representing the shifted signal.
        """
        # Define the shifted function
        def shifted_func(t):
            return self.func(t - shift)
        # Create a new ContinuousSignal with the shifted function
        newSignal = ContinuousSignal(shifted_func)
        return newSignal

    def add(self, other):
        """
        Adds another ContinuousSignal to this signal.

        Parameters:
        other (ContinuousSignal): The other signal to add.

        Returns:
        ContinuousSignal: A new ContinuousSignal instance representing the sum of the two signals.
        """
        # Define a new function representing the sum of the two signals
        def added_func(t):
            return self.func(t) + other.func(t)
        # Create a new ContinuousSignal with the added function
        newSignal = ContinuousSignal(added_func)
        return newSignal

    def multiply(self, other):
        """
        Multiplies this signal with another ContinuousSignal.

        Parameters:
        other (ContinuousSignal): The other signal to multiply with.

        Returns:
        ContinuousSignal: A new ContinuousSignal instance representing the product of the two signals.
        """
        # Define a new function representing the product of the two signals
        def multiplied_func(t):
            return self.func(t) * other.func(t)
        # Create a new ContinuousSignal with the multiplied function
        newSignal = ContinuousSignal(multiplied_func)
        return newSignal

    def multiply_signal_by_const_factor(self, scalar):
        """
        Multiplies the signal by a scalar constant.

        Parameters:
        scalar (float): The scalar value to multiply with.

        Returns:
        ContinuousSignal: A new ContinuousSignal instance representing the scaled signal.
        """
        # Define a new function representing the scaled signal
        def multiplied_func(t):
            return self.func(t) * scalar
        # Create a new ContinuousSignal with the multiplied function
        newSignal = ContinuousSignal(multiplied_func)
        return newSignal

    def plot(self):
        """
        Plots the continuous signal over the range from -INF to INF.

        The signal is sampled at 'PRECISION' number of points within the range.
        """
        # Generate time values over the specified range
        x_axis_values = np.linspace(-self.INF, self.INF, self.PRECISION)
        # Evaluate the function at each time value
        y_axis_values = np.array([self.func(x) for x in x_axis_values])
        # Plot the signal
        plt.plot(x_axis_values, y_axis_values)
        # Label the axes
        plt.xlabel('t (Time)')
        plt.ylabel('x(t)')
        # Set x-ticks at integer values
        plt.xticks(np.arange(-self.INF, self.INF + 1, 1))
        # Enable grid for better readability
        plt.grid(True)

class DiscreteLinearTimeInvariantSystem:
    """
    Class representing a discrete Linear Time-Invariant (LTI) system characterized by its impulse response.
    """

    def __init__(self, impulse_response):
        """
        Initializes the LTI system with a given impulse response.

        Parameters:
        impulse_response (DiscreteSignal): The impulse response h[n] of the system.
        """
        self.impulse_response = impulse_response

    def linear_combination_of_impulses(self, input_signal):
        """
        Decomposes the input signal into a linear combination of scaled and shifted unit impulses.

        Parameters:
        input_signal (DiscreteSignal): The input signal x[n].

        Returns:
        unit_impulses (list of DiscreteSignal): A list of unit impulse signals δ[n - n0].
        coefficients (list of float): Corresponding coefficients x[n0] for each unit impulse.
        """
        unit_impulses = []  # List to store unit impulses (DiscreteSignal instances)
        coefficients = []    # List to store the corresponding coefficients

        # Iterate over the time indices of the input signal
        for i in range(-input_signal.INF, input_signal.INF + 1):
            # Get the value of the input signal at time i
            value = input_signal.get_value_at_defined_time(i)
            if value != 0:
                # If the value is non-zero, create a unit impulse at time i
                unit_impulse = DiscreteSignal(input_signal.INF)
                unit_impulse.set_value_at_defined_time(i, 1)  # Set δ[n - i] = 1 at time n = i
                unit_impulses.append(unit_impulse)
                coefficients.append(value)  # The coefficient is x[i]

        return unit_impulses, coefficients

    def output(self, input_signal):
        """
        Computes the output of the LTI system for a given input signal using convolution.

        Parameters:
        input_signal (DiscreteSignal): The input signal x[n].

        Returns:
        output_signal (DiscreteSignal): The output signal y[n].
        impulse_responses (list of DiscreteSignal): The shifted impulse responses corresponding to each unit impulse.
        coefficients (list of float): The coefficients corresponding to each impulse response.
        """
        # Decompose the input signal into unit impulses and coefficients
        unit_impulses, coefficients = self.linear_combination_of_impulses(input_signal)

        output_signal = DiscreteSignal(input_signal.INF)  # Initialize output signal with zeros
        impulse_responses = []  # List to store shifted impulse responses

        # For each unit impulse, compute the shifted impulse response
        for i in range(len(unit_impulses)):
            # Get the current unit impulse
            curr_unit_impulse = unit_impulses[i]
            # Find the index where the unit impulse is non-zero
            for j in range(len(curr_unit_impulse.values)):
                if curr_unit_impulse.values[j] != 0:
                    index = j
                    break
            # Calculate the shift amount (time index of impulse minus INF)
            shift_amount = index - curr_unit_impulse.INF
            # Shift the impulse response by the shift amount
            curr_impulse_response = self.impulse_response.shift_signal(shift_amount)
            # Store the shifted impulse response
            impulse_responses.append(curr_impulse_response)

        # Compute the output signal by summing the scaled shifted impulse responses
        for i in range(len(impulse_responses)):
            # Multiply the shifted impulse response by its coefficient
            scaled_impulse_response = impulse_responses[i].multiply_signal_by_const_factor(coefficients[i])
            # Add it to the output signal
            output_signal = output_signal.add(scaled_impulse_response)

        return output_signal, impulse_responses, coefficients

class ContinuousLinearTimeInvariantSystem:
    """
    Class representing a continuous-time Linear Time-Invariant (LTI) system characterized by its impulse response.
    """

    def __init__(self, impulse_response):
        """
        Initializes the LTI system with a given impulse response.

        Parameters:
        impulse_response (ContinuousSignal): The impulse response h(t) of the system.
        """
        self.impulse_response = impulse_response

    def linear_combination_of_impulses(self, input_signal, delta):
        """
        Approximates the input signal as a linear combination of scaled and shifted approximate unit impulses.

        Parameters:
        input_signal (ContinuousSignal): The input signal x(t).
        delta (float): The width of the approximate unit impulse (sampling interval).

        Returns:
        unit_impulses (list of ContinuousSignal): A list of approximate unit impulse signals δΔ(t - t0).
        coefficients (list of float): Corresponding coefficients x(t0) * Δ for each unit impulse.
        """
        # Define an approximate unit impulse function with width delta
        def unit_impulse_function(t):
            if 0 <= t < delta:
                return 1 / delta  # The area under the impulse is 1
            else:
                return 0

        unit_impulses = []  # List to store unit impulses
        coefficients = []   # List to store corresponding coefficients

        # Generate time values from -INF to INF with step size delta
        t_values = np.arange(-ContinuousSignal.INF, ContinuousSignal.INF, delta)

        # For each time value, create a shifted unit impulse
        for t in t_values:
            t = round(t, 2)  # Round to two decimal places to avoid floating point issues
            # Create a unit impulse function
            unit_impulse = ContinuousSignal(unit_impulse_function)
            # Shift the unit impulse to time t
            unit_impulse = unit_impulse.shift(t)
            # Add the shifted unit impulse to the list
            unit_impulses.append(unit_impulse)
            # Compute the coefficient x(t) * delta
            coefficient = input_signal.func(t) * delta
            coefficients.append(coefficient)

        return unit_impulses, coefficients

    def signal_output_approximation(self, input_signal, delta):
        """
        Approximates the output of the LTI system for a given input signal using convolution integral approximation.

        Parameters:
        input_signal (ContinuousSignal): The input signal x(t).
        delta (float): The width of the approximate unit impulse (sampling interval).

        Returns:
        output_signal (ContinuousSignal): The approximate output signal y(t).
        impulse_responses (list of ContinuousSignal): The shifted impulse responses corresponding to each unit impulse.
        coefficients (list of float): The coefficients corresponding to each impulse response.
        """
        # Decompose the input signal into approximate unit impulses and coefficients
        unit_impulses, coefficients = self.linear_combination_of_impulses(input_signal, delta)

        impulse_responses = []  # List to store shifted impulse responses
        # Initialize the output signal to zero
        def init_signal_with_zeroes(t):
            return 0
        output_signal = ContinuousSignal(init_signal_with_zeroes)

        # For each unit impulse, compute the shifted impulse response and accumulate the output
        for i in range(len(unit_impulses)):
            curr_unit_impulse = unit_impulses[i]
            # Generate time values to evaluate the unit impulse function
            t_values = np.arange(-ContinuousSignal.INF, ContinuousSignal.INF, delta)

            # Find the time at which the unit impulse is non-zero
            for t in t_values:
                t = round(t, 2)
                if curr_unit_impulse.func(t) != 0:
                    first_index = t
                    break

            # Shift the impulse response by the time where the unit impulse is located
            curr_impulse_response = self.impulse_response.shift(first_index)
            # Store the shifted impulse response
            impulse_responses.append(curr_impulse_response)
            # Multiply the impulse response by the coefficient and add to the output signal
            scaled_impulse_response = curr_impulse_response.multiply_signal_by_const_factor(coefficients[i])
            output_signal = output_signal.add(scaled_impulse_response)

        return output_signal, impulse_responses, coefficients

def main_discrete_portion(INF):
    # #################### DISCRETE PORTION OF MAIN #####################
    

    # Creating a discrete LTI system with an arbitrary impulse response (from Figure 3 of spec)
    impulse_response = DiscreteSignal(INF)
    impulse_response.set_value_at_defined_time(0, 1)
    impulse_response.set_value_at_defined_time(1, 1)
    impulse_response.set_value_at_defined_time(2, 1)

    # Instantiate the Discrete LTI system with the defined impulse response
    DiscreteLTIInstance = DiscreteLinearTimeInvariantSystem(impulse_response)

    # Creating an arbitrary input signal (from Figure 1 of spec)
    input_signal = DiscreteSignal(INF)
    input_signal.set_value_at_defined_time(0, 0.5)
    input_signal.set_value_at_defined_time(1, 2)

    # Decompose the input signal into unit impulses and coefficients
    unit_impulses, coefficients = DiscreteLTIInstance.linear_combination_of_impulses(input_signal)

    return input_signal, unit_impulses, impulse_response, DiscreteLTIInstance, coefficients

def main_discrete_portion_plot(INF, input_signal, unit_impulses, impulse_response, DiscreteLTIInstance, coefficients, PLOT_SIZE):
    ############### PLOTTING THE INPUT SIGNAL PART ####################
    # Create directory to save discrete signal plots
    directory = 'Discrete_Signals'
    if not os.path.exists(directory):
        os.makedirs(directory)
    # Figure 1: Input Discrete Signal x[n]
    plt.title('x[n]')
    input_signal.plot()
    plt.savefig(os.path.join(directory, 'Figure_1_Input_Discrete_Signal.png'))
    plt.show()

    # Figure 2: Plotting the unit impulses multiplied by their respective coefficients
    
    fig, axs = plt.subplots(4, 3, figsize=PLOT_SIZE)  # Create a grid of subplots
    fig.tight_layout()  # Adjust the layout

    # Initialize a signal to accumulate the sum of the scaled unit impulses
    sum_of_plots = DiscreteSignal(INF)
    currentIndex = 0  # Index to keep track of the current non-zero unit impulse

    # Iterate over the time indices to plot each scaled unit impulse
    for i in range(11):  # Since INF = 5, indices from 0 to 10
        ax = plt.subplot(4, 3, i+1)  # Select the subplot
        if(i % 3 != 0):
            ax.set_yticklabels([])  # Hide y-axis labels for better readability
        if (i < 9):
            ax.set_xticklabels([])  # Hide x-axis labels except for the last row
        # Check if there is a unit impulse at this index
        if currentIndex < len(unit_impulses) and i == np.where(unit_impulses[currentIndex].values != 0)[0][0]:
            # Multiply the unit impulse by its coefficient
            currentSig = unit_impulses[currentIndex].multiply_signal_by_const_factor(coefficients[currentIndex])
            # Add to the sum of plots
            sum_of_plots = sum_of_plots.add(currentSig)
            currentIndex += 1  # Move to the next unit impulse
        else:
            # If no unit impulse at this index, plot a zero signal
            currentSig = DiscreteSignal(INF)
        # Set the title of the subplot
        plt.title(f'δ[n - ({i - INF})]x[{i - INF}]')
        # Plot the current signal
        currentSig.plot()

    # Plot the sum of the scaled unit impulses in the last subplot
    plt.subplot(4, 3, 12).set_yticklabels([])
    plt.title('Sum')
    sum_of_plots.plot()
    plt.suptitle('Impulses multiplied by coefficients')
    # Adjust subplot spacing
    plt.subplots_adjust(hspace=0.6, wspace=0.2, top=0.92)

    # Save and show the figure
    plt.savefig(os.path.join(directory,'Figure_2_Returned_Impulses_multiplied_by_respective_coefficients.png'))
    plt.show()

    # ################## PLOTTING THE OUTPUT SIGNAL PART ##################
    # Compute the output signal by passing the input signal through the LTI system
    output_signal, impulse_responses, coefficients = DiscreteLTIInstance.output(input_signal)

    # Figure 3: Plotting the impulse response h[n] of the system
    plt.title('h[n]')
    impulse_response.plot()
    plt.savefig(os.path.join(directory, 'Figure_3_Impulse_Response_Signal.png'))
    plt.show()

    # Figure 4: Plotting the scaled and shifted impulse responses and the output signal
    #PLOT_SIZE = (12, 9)
    fig, axs = plt.subplots(4, 3, figsize=PLOT_SIZE)
    fig.tight_layout()

    # Initialize a signal to accumulate the sum of the scaled impulse responses (which will be the output)
    output_sum = DiscreteSignal(INF)
    currentIndex = 0  # Index to keep track of the current non-zero impulse response

    # Iterate over the time indices to plot each scaled and shifted impulse response
    for i in range(11):  # Since INF = 5, indices from 0 to 10
        ax = plt.subplot(4, 3, i+1)
        if(i % 3 != 0):
            ax.set_yticklabels([])
        if (i < 9):
            ax.set_xticklabels([])
        # Check if there is an impulse response at this index
        if currentIndex < len(impulse_responses) and i == np.where(impulse_responses[currentIndex].values != 0)[0][0]:
            # Multiply the impulse response by its coefficient
            currentSig = impulse_responses[currentIndex].multiply_signal_by_const_factor(coefficients[currentIndex])
            # Add to the output sum
            output_sum = output_sum.add(currentSig)
            currentIndex += 1  # Move to the next impulse response
        else:
            # If no impulse response at this index, plot a zero signal
            currentSig = DiscreteSignal(INF)
        # Set the title of the subplot
        plt.title(f'h[n - ({i - INF})] * x[{i - INF}]')
        # Plot the current signal
        currentSig.plot()

    # Plot the output signal (sum of scaled impulse responses) in the last subplot
    plt.subplot(4, 3, 12).set_yticklabels([])
    plt.title('Output = Sum')
    output_sum.plot()
    plt.suptitle('Response of Input Signal')
    # Adjust subplot spacing
    plt.subplots_adjust(hspace=0.6, wspace=0.2, top=0.92)

    # Save and show the figure
    plt.savefig(os.path.join(directory,'Figure_4_Output.png'))
    plt.show()

def main_continuous_portion(DELTA):
    #################### CONTINUOUS PORTION OF MAIN #####################

    # Define the impulse response h(t) = u(t), the unit step function
    def unit_step_function(t):
        if t < 0:
            return 0
        else:
            return 1

    impulse_response = ContinuousSignal(unit_step_function)

    # Instantiate the Continuous LTI system with the impulse response h(t)
    ContinuousLTIInstance = ContinuousLinearTimeInvariantSystem(impulse_response)

    # Define the input signal x(t)
    def my_exponential_function(t):
        if t < 0:
            return 0
        else:
            return np.exp(-t)
    input_signal = ContinuousSignal(my_exponential_function)

    # DELTA = 0.5  # Sampling interval (width of approximate unit impulses)

    # Decompose the input signal into approximate unit impulses and coefficients
    unit_impulses, coefficients = ContinuousLTIInstance.linear_combination_of_impulses(input_signal, DELTA)

    return input_signal, unit_impulses, coefficients, ContinuousLTIInstance

def main_continuous_portion_plot(input_signal, unit_impulses, coefficients, ContinuousLTIInstance,DELTA, PLOT_SIZE):
    # Create directory to save continuous signal plots
    directory = 'Continuous_Signals'
    if not os.path.exists(directory):
        os.makedirs(directory)

    # Figure 5: Plotting the input continuous signal x(t)
    plt.title('x(t),INF = 3')
    input_signal.plot()
    plt.savefig(os.path.join(directory, 'Figure_5_Input_Continuous_Signal.png'))
    plt.show()

    # Figure 6: Plotting the scaled unit impulses and the reconstructed signal
    fig, axs = plt.subplots(5, 3, figsize=PLOT_SIZE)
    fig.tight_layout()

    # Initialize a signal to accumulate the reconstructed signal
    def init_signal_with_zeroes(t):
        return 0
    reconstructed_signal = ContinuousSignal(init_signal_with_zeroes)

    for i in range(len(unit_impulses)):
        ax = plt.subplot(5, 3, i+1)
        if (i % 3 != 0):
            ax.set_yticklabels([])
        if (i < 12):
            ax.set_xticklabels([])
        ax.set_ylim(-0.1, 1.1)

        plt.yticks([0, 0.5, 1])
        # Multiply the unit impulse by its coefficient
        currentSig = unit_impulses[i].multiply_signal_by_const_factor(coefficients[i])
        # Add to the reconstructed signal
        reconstructed_signal = reconstructed_signal.add(currentSig)
        plt.title(f'δ(t - ({i - (ContinuousSignal.INF / DELTA)}∇))x({i - (ContinuousSignal.INF / DELTA)}∇)∇')
        currentSig.plot()

    # Plot the reconstructed signal in the last subplot
    plt.subplot(5, 3, 13).set_ylim(-0.1, 1.1)
    plt.title('Reconstructed Signal')
    reconstructed_signal.plot()

    # Adjust subplot properties
    for i in [14, 15]:
        ax = plt.subplot(5, 3, i)
        ax.set_xticks(np.arange(-ContinuousSignal.INF, ContinuousSignal.INF + 1, 1))
        ax.set_yticklabels([])

    plt.suptitle('Impulses multiplied by coefficients')
    plt.subplots_adjust(hspace=0.8, wspace=0.2, top=0.92)

    plt.savefig(os.path.join(directory,'Figure_6_Returned_Impulses_Multiplied_by_their_coefficients.png'))
    plt.show()

    # Figure 7: Reconstruction of input signal with varying delta
    fig, axs = plt.subplots(2, 2, figsize=(PLOT_SIZE))
    fig.tight_layout()

    plt.yticks(np.arange(0, 1.2, 0.2))  # y_values are between 0 and 1.2

    delta_values = np.array([0.5, 0.1, 0.05, 0.01])  # Different delta values for approximation

    for delta in delta_values:
        for i in range(len(delta_values)):
            if(delta == delta_values[i]):
                index_of_curr_delta = i
                break

        plt.subplot(2, 2, index_of_curr_delta + 1)

        # Initialize reconstructed signal
        reconstructed_signal = ContinuousSignal(init_signal_with_zeroes)

        # Decompose input signal with current delta
        unit_impulses, coefficients = ContinuousLTIInstance.linear_combination_of_impulses(input_signal, delta)
        # Reconstruct the signal
        for i in range(len(unit_impulses)):
            currentSig = unit_impulses[i].multiply_signal_by_const_factor(coefficients[i])
            reconstructed_signal = reconstructed_signal.add(currentSig)
        # Plot the reconstructed signal and the original input signal
        reconstructed_signal.plot()
        input_signal.plot()
        plt.legend(['Reconstructed', 'x(t)'])
        plt.title(f'∇ = {delta}')
    plt.subplots_adjust(hspace=0.2, wspace=0.2, top=0.96)
    plt.savefig(os.path.join(directory,'Figure_7_Reconstruction_of_input_signal_with_varying_delta.png'))
    plt.show()

    # Figure 8: Plotting the scaled impulse responses and the output signal approximation
    fig, axs = plt.subplots(5, 3, figsize=PLOT_SIZE)
    fig.tight_layout()

    # Initialize sum_signal to accumulate the output signal
    def init_signal_with_zeroes(t):
        return 0
    sum_signal = ContinuousSignal(init_signal_with_zeroes)

    # Compute the approximate output signal
    output_signal, impulse_responses, coefficients = ContinuousLTIInstance.signal_output_approximation(input_signal, DELTA)

    for i in range(len(impulse_responses)):
        ax = plt.subplot(5, 3, i+1)
        if (i % 3 != 0):
            ax.set_yticklabels([])
        if (i < 12):
            ax.set_xticklabels([])
        ax.set_ylim(-0.1, 1.4)

        plt.yticks([0.0, 0.5, 1.0])
        # Multiply the impulse response by its coefficient
        currentSig = impulse_responses[i].multiply_signal_by_const_factor(coefficients[i])
        sum_signal = sum_signal.add(currentSig)
        plt.title(f'h(t - ({i - (ContinuousSignal.INF / DELTA)}∇)) * x({i - (ContinuousSignal.INF / DELTA)}∇)∇')
        currentSig.plot()

    # Plot the output signal in the last subplot
    plt.subplot(5, 3, 13).set_ylim(-0.1, 1.4)
    plt.title('Output = Sum')
    sum_signal.plot()

    # Adjust subplot properties
    for i in [14, 15]:
        ax = plt.subplot(5, 3, i)
        ax.set_xticks(np.arange(-ContinuousSignal.INF, ContinuousSignal.INF + 1, 1))
        ax.set_yticklabels([])

    plt.suptitle('Response of Impulse Signal')
    plt.subplots_adjust(hspace=0.8, wspace=0.2, top=0.92)

    plt.savefig(os.path.join(directory,'Figure_8_Returned_Impulses_Multiplied_by_their_coefficients.png'))
    plt.show()

    # Figure 9: Approximate output as delta tends to 0
    fig, axs = plt.subplots(2, 2, figsize=(PLOT_SIZE))
    fig.tight_layout()

    # Define the actual output signal y(t) = (1 - e^{-t})u(t)
    def my_exponential_output_function(t):
        if(t < 0):
            return 0
        else:
            return (1 - np.exp(-t))

    actual_output_signal = ContinuousSignal(my_exponential_output_function)

    plt.yticks(np.arange(0, 1.2, 0.2))  # y_values are between 0 and 1.2

    delta_values = np.array([0.5, 0.1, 0.05, 0.01])  # Different delta values for approximation

    for delta in delta_values:
        for i in range(len(delta_values)):
            if(delta == delta_values[i]):
                index_of_curr_delta = i
                break

        plt.subplot(2, 2, index_of_curr_delta + 1)

        # Compute the approximate output signal
        approx_output_signal, impulse_responses, coefficients = ContinuousLTIInstance.signal_output_approximation(input_signal, delta)

        # Plot the approximate output signal and the actual output signal
        approx_output_signal.plot()
        actual_output_signal.plot()

        plt.legend(['y_approx(t)', 'y(t)= (1-e^(-t))u(t)'])
        plt.title(f'∇ = {delta}')
    plt.subplots_adjust(hspace=0.2, wspace=0.2, top=0.96)
    plt.suptitle('Approximate output as ∇ tends to 0')
    plt.savefig(os.path.join(directory,'Figure_9_Approximate_Output_with_varying_delta.png'))
    plt.show()

def main():
    INF = 5  # Define the maximum time index for the discrete signals

    PLOT_SIZE = (12, 9)  # Define the size of the plot

    input_signal_discrete, unit_impulses_discrete, impulse_response_discrete, DiscreteLTIInstance, coefficients_discrete=main_discrete_portion(INF)

    main_discrete_portion_plot(INF=INF,
                               input_signal=input_signal_discrete,
                               unit_impulses=unit_impulses_discrete,
                               impulse_response=impulse_response_discrete,
                               DiscreteLTIInstance=DiscreteLTIInstance,
                               coefficients=coefficients_discrete,
                               PLOT_SIZE=PLOT_SIZE)
    
    DELTA = 0.5  # Sampling interval (width of approximate unit impulses)
    
    input_signal_continuous, unit_impulses_continuous, coefficients_continuous, ContinuousLTIInstance = main_continuous_portion(DELTA=DELTA)

    main_continuous_portion_plot(input_signal=input_signal_continuous,
                                 unit_impulses=unit_impulses_continuous,
                                 coefficients=coefficients_continuous,
                                 ContinuousLTIInstance=ContinuousLTIInstance,
                                 DELTA=DELTA,
                                 PLOT_SIZE=PLOT_SIZE)


main()
