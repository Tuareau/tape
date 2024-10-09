import numpy as np
import sys

def argument_valid(argument):
    error_message = "Invalid argument: " + str(argument) + "; expected integer\n"
    try:
        value = int(argument)
    except ValueError:
        print(error_message)
        return False
    if value < 0:
        print(error_message)
    return True

def print_program_usage():
    message = """
    Usage: python generate_input_file.py N\n
    where N must be greater than 0
    """
    print(message)

def main():
    if len(sys.argv) < 2:
        print_program_usage()
        exit()

    argument = sys.argv[1]
    if not argument_valid(argument):
        exit()
    N = int(argument)

    rng = np.random.default_rng()
    samples = rng.integers(low=-2**31, high=2**31-1, size=N)
    print(samples)

    output_file = open('input.dat', 'w')
    for x in samples:
        output_file.write(str(x) + "\n")
    output_file.close() 

main()