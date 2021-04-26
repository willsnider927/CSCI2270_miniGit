Usage:
compile the driver and miniGit.cpp into a .out and run the program in the directory you want to turn into a repo.
The command prompt will display all the information needed to use the program. After intializing the repo,
the ability to add files, remove files, commit versions, and checkout previous versions will become available 
to use. 

Phase 2 extra features:

The ability to exit and reopen the program with the repo still intact is now possible, the data structure will
be written to .minigit.txt in the .minigit directory. Upon relaunching the program, this file will be looked for
and initialization will be skipped if it is found. Two different quit options are present in case deconstruction of 
the repo is desired.

Status and Diff have also been implemented and can be called when prompted. Status will display files in the working
directory that have been changed from the most recent commit as well as ones which are no longer present in the
directory. Diff will prompt the user for the file to compare to and will display the first different line.