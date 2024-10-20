import numpy as np
import sys
import os

def argument_valid(argument):
    error_message = "Invalid argument: " + str(argument) + "; no such file\n"
    try:
        file = open(argument, 'br')
    except FileNotFoundError:
        print(error_message)
        return False
    return True

def print_program_usage():
    message = """
    Usage: python translate_binary_file_to_text.py <filename>.bin\n
    """
    print(message)

def main():
    if len(sys.argv) < 2:
        print_program_usage()
        exit()

    argument = sys.argv[1]
    if not argument_valid(argument):
        exit()
    filename = argument
            
    with open(filename, 'rb') as file:
        binary_data = file.read()
        unpacked_numbers = [int.from_bytes(binary_data[i:i+4], byteorder='little', signed=True) for i in range(0, len(binary_data), 4)]
        
    fname, extension = os.path.splitext(filename)
    with open(fname + '.dat', 'wt') as file:
        for sample in unpacked_numbers:
            file.write(str(sample) + '\n')

main()