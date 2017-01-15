Description:
This program is a very simple intrusion detection system that monitors the
processes running on a Linux system by searching throughthe /proc directory
and looking for processes that violate entries listed in a configuration
file. The program can be used to restrict processes based on the process
owner, the path that a process is running in, and the amount of memory
a process is using.



Compilation:
To compile the program, just type make and then enter.



Running the Program:
Run the program with the command sudo ./phunt [-l logFile] [-c configFile].

So, for instance, you could type the command:
	"sudo ./phunt -l /var/log/output.log -c /etc/configuration.conf"
In this case you should have a configuration file already created in the /etc
directory. The defaults for the log and config files are /var/log/phunt.log
and /etc/phunt.conf, respectively.



Configuration File Instructions:
Note: In the config file, the '#' character designates a comment line and must be
placed at the very beginning of a line.

The config file should consist of a list of rules specifying the type of action
to be taken and the type of process to take it on.

The first word in a line is the type of action to take and should be one of the
following: kill (to kill the process violating the rule), suspend (to suspend the
process violating the rule), or nice (to reduce the process' priority to the lowest
possible level).

The second word specifies the type of rule and should be one of the following:
user (to restrict processes run by a specific user), path (to restrict processes
being run in a specific directory), and memory (to restrict processes exceeding a
specified memory limit)

The last word corresponds with the type of rule. For a user rule, it should be a
username. For a path rule, it should be a path. For a memory rule, it should be
a number representing the maximum amount of memory (in MB) that a process can use.

Example:
# Example Config file

# Kill any programs run by the user badguy
kill user badguy

# Suspend any programs using more than 2000 MB of memory
suspend memory 2000

# Lower the priority level of any programs running in the /tmp directory
nice path /tmp



Additional Notes:
In order to kill the program, type Ctrl-C. Once started, the program runs every 5
seconds. This can easily be changed by changing sleep(5) in the main method to
sleep(10), sleep(15), etc.
