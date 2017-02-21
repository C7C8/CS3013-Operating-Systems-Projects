For more detailed explanation of the algorithm I used, see problem_explanation.txt

This project is constructed in a way loosely modeled after OOP, with a base "class"
(struct) called `node` things that "inherit" from it, namely a normal node and a
noisemaker. There is a main file that starts off all nodes and a params file that
has a few project parameters in the form of #defines. A small linked list system
for keeping track of message IDs is stored in node.c. Function pointers are used
to implement "virtual functions".

COMPILE WARNING: THIS PROJECT USED CMAKE AS A BUILD SYSTEM. YOU CAN USE EITHER THE
PROVIDED MAKEFILE OR THE CMAKELISTS.TXT FILE IN COMBINATION WITH CMAKE. It built
on my system with cmake just fine, I'm not expecting any problems with getting it
to work on other systems. The files for the different parts of the project are
in mutex_src.zip and semaphore_src.zip for parts 1 and 2, respectively. Cmake and
the makefile expect all source files to be contained in a folder called "src", so
make sure that those zip files extract correctly!

Something to consider when grading, I guess: this project was tested on an 8-core
system. It runs very well on my system, but I have no idea how it will run on a
VM with 2 cores assigned.
