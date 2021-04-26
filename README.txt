Implementation of a local git system. Phase 1 is a barebones system, consisting of adding, removing, commiting,
and checking out previous versions. Phase 2 has the same features plus diff and status which allow the user to compare
the working directory to the most recent commit. Also implemented in the second phase is serialisation and deserealisation,
which allows the program to be used on multiple occasions while maintaining the same commit structure.