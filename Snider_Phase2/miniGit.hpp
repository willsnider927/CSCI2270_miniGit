#include <string>

#pragma once

struct miniGitFiles{
    std::string fileName; //local file
    std::string fileVersion; //name in .minigit
    miniGitFiles* next;
};

struct commitNode{
    int commitNumber;
    miniGitFiles* head;
    commitNode* previous;
    commitNode* next;
};


class miniGit {
    public:
        void init(); //initialize minitGit
        void delGit(); //delete entire minitGit
        void addFile(); 
        void removeFile();
        void commit();
        void checkout();
        bool serialize();
        void deserialise();
    private:
        commitNode* head = nullptr;
        commitNode* currVersion = nullptr; //the node being prepped for commit
        commitNode* versionNode = nullptr; //the most recently commited node
};