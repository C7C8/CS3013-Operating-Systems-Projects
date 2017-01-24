==About==

This project has been completed to the standards as required by the specification PDF. The final
version, mc2, is capable of supporting up to 32,768 different background processes and can display
relevant statistics when each completed. All input is read from stdin using getline, so all
versions can technically be scriptable.

In each folder there are four files: A makefile, the relevant mc#.c file, a cmds.txt file, and a
cmds-output.txt file. The makefile supports `make all` and `make clean` and has relevant flags
set at the top. Implicit rules are used. The makefile at the root directory of this project simply
recurses down into each folder to build; it has no compilation commands in it. Every cmds.txt file
contains a list of commands that the mc program, once built, can read from. The accompanying
cmds-output.txt file contains the output of the mc program if the cmds.txt file is fed in.

===Explanation===

I'm not sure if the project instructions want explanations for all parts or just v2, so I wrote
explanations for all parts just in case.

--v0--
v0 is not at all smart. It prints a menu and maintains an array of four strings. The values of
those strings are changed depending on the command being run. The resulting array of strings is
then fed directly into execvp, and simple statistics are collcetd.

--v1--
v1 uses a linked list to support an expandable list of commands. Each command is embodied in a
struct that contains a pointer to the next struct and a list of arguments, 32 arguments long.
The execCmd function is changed to accept these structs instead.

--v2--
v2 expands upon v1 by adding a process table and new "process" struct that contains a PID, the
arguments list, and a timeval struct containing the time when the process was started. The process
table -- ptable -- is simply an array of pointers to processes, 32,768 elements long (the maximum ]
number of processes that the Linux kernel allows by default). By default each pointer is set to
NULL. A separate thread is created at the start of the program that continually loops over the
process table, checking for dead children with wait4 and the WNOHANG flag. When a dead child is
detected, the thread - referred to as td_processMonitor in the code, or simply "reaperThread" -
calls the printStats function on the dead child, frees its memory, and sets its slot on the ptable
to NULL again. In this way, the program is able to account for outstanding background processes
in such a way that order does not matter.

When the command to add a new command is issued, the program tokenizes the input string as normal,
but this time searches for the & character as a last argument. If detected, it is not recorded into
the command's argument list; instead, a "concurrent" flag on the linkedCmd struct is set, indicating
that the process should be run as a background job and that a '&*' should be displayed on the
main menu next to the entry for the command. When the command is actually executed, the program
forks and execvp()'s as normal, but the parent process searches for the first open slot on the
ptable and inserts it where possible. In the cataclysmic event where the process table is full
(and presumably the host computer is dying if not already dead), the program prints an error
message and runs it in the foreground instead.

When the user issues the exit command, the program first checks the entire ptable for non-NULL
elements; if one is found, the program refuses to exit. Otherwise, the reaper thread is killed
and the program exits normally. If EOF is detected at the main menu, the program will similarly
attempt to exit normally. If EOF is detected at any other time, the program will print an error
message to stderr and will immediately exit with error code 1.

==Testing==

I tested this program by manual interaction in most cases. Reading commands from a file was
tested with the cmds.txt files included in this project. Memory leaks were checked for using
valgrind (mc1 turned up clean, mc2 is acting weird but there are no definite leaks) and debugging
was done using gdb.

==Things you should know==

The program uses multithreading! Bizarrely enough, multithreading made everything so much easier
than either suggested approach was, and it even allows for those "process completed" messages
to pop up at any time like the project specification PDF describes.
