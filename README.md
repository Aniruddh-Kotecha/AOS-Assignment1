# CS3.304 - Advanced Operating Systems
## Assignment 1

The project implements reversing the content's of a file and storing the result in a new file using system calls in C++

Prerequisites:

1. C++ compiler (like GCC) 
2. Terminal to execute the C++ file.

Steps to Run a C++ File:

* g++ program_name.cpp -o executable_filename
* ./executable_filename

**Q1:**

This program takes arguments in the following format: 

./executible_filename input_file flag [start_index] [end_index]

* flag can take value 0 or 1.
* if value 1 is provided in flag the start_index and end_index are mandatory.

Description: 

- Given an input file, the program will reverse the content's of the file and store it into a new file in a new directory called "Assignment1".
- If the flag is set to 0, the entire file will be reversed. If the flag is set to 1, the start and end indices must be provided as command-line arguments. In this case, the file will be reversed in two parts: from the beginning to the start index, and from the end index to the end of the document, leaving the portion of the file between the start and end indices unchanged.

Output: Progress percent showing real-time progress of file reversed.

**Q2:**

This program takes arguments in the following format: 

./executible_filename new_file old_file directory

* old_file is the path to input file from Q1.
* new_file is the path to reversed output file.
* directory is the path to the directory where output file is stored.

Description
- This program verifies if the file was reversed. It also verifies the permissions provided to the new directory and file.

Output: Display if the directory is created, if the new file is correctly reversed and if the size is equal to old file.
Also, display the 9 different permissions of the directory, the new file and the old file. 
