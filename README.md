Joseph Ryan Tocco

Note: I wish to use two grace days for this assignment. I don't believe I've
used any as of yet.

Submitted files include phunt.c and the Makefile.

A program has been created that monitors the processes running on a Linux
system by searching through the /proc directory and looking for processes that
violate entries listed in a configuration file.

At the top of the file is a list of global variables that will be used throughout
the program, followed by function declarations. The flow of the program moves
generally downward. The main function calls a function to parse the command line
arguments and then a function that parses the config file and stores the information
in a global variable. It then, in a loop running every 5 seconds, calls the function
loopThroughProcFolders. This function goes through the folders in /proc for every
process running on the system. It calls other functions to extract information about
the process, compare the info against the configuration instructions, and take
action if necessary.

To run the program, type make, enter, sudo ./phunt, and enter again.

In the config file, the '#' character must be at the beginning of the line
in order for it to count as a comment. In order to kill the program, type Ctrl-C.
The log file should be closed and so the information will be retained. As mentioned
above, once started, the program runs every 5 seconds. This can easily be changed
by changing sleep(5) in the main method to sleep(10), sleep(15), etc.

The project was probably took about 2 solid days of work. At first it looked
very intimidating, but I found that once I started working on it that while
it was challenging, it wasn't ridiculously difficult.
