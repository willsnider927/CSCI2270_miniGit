#include <string>

#pragma once

struct commitNode{
    int commitNumber;
    miniGitFiles* head;
    commitNode* previous;
    commitNode* next;
};

struct miniGitFiles{
    std::string fileName; //local file
    std::string fileVersion; //name in .minigit
    miniGitFiles* next;
};

class miniGit {
    public:
        void init(); //initialize minitGit
        //void delGit(); //delete entire minitGit
        void addFile(); 
        void removeFile();
        void commit();
        void checkout();
    private:
        commitNode* head;
        commitNode* currVersion; //the node being prepped for commit
        commitNode* versionNode; //the most recently commited node
};