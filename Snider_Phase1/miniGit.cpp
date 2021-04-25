#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include "miniGit.hpp"

namespace fs = std::filesystem;
using namespace std;

//helper function for creating commitNodes
commitNode* createCommitNode(int commitNum, miniGitFiles* head, commitNode* prev) { 
    commitNode* newNode = new commitNode;
    newNode->previous = prev;
    newNode->commitNumber = commitNum;
    newNode->head = head;
    newNode->next = NULL;
    return newNode;
}

void miniGit::init() {
    head = createCommitNode(0, NULL, NULL);
    currVersion = head;
    versionNode = head;
    fs::create_directory(".minigit");
    std::cout << endl;
}


void miniGit::delGit() {
    if(head == NULL) return;
    commitNode* nexCommit;
    miniGitFiles* nexFile;
    for (commitNode* it = head; it != NULL; it = nexCommit) {
        nexCommit = it->next;
        for (miniGitFiles* it2 = it->head; it2 != NULL; it2 = nexFile) {
            nexFile = it2->next;
            delete it2;
        }
        delete it;
    }
    fs::remove_all(".minigit");
}


void miniGit::addFile() {
    if (!head) {
        std::cout << "No repository to add to." << endl << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) {
        std::cout << "Return to most recent commit first" << endl << endl;
        return;
    }    
    string fileName;
    std::cout << "Enter the file name to be added." << endl;
    cin >> fileName;
    while (!fs::exists(fileName)) {
        std::cout << "File not found, try again." << endl;
        cin >> fileName;
    } //check for valid file name

    miniGitFiles* currFile = currVersion->head;
    miniGitFiles* lastFile = currFile; //useful for adding if it is allowed to be
    for (currFile; currFile != NULL; currFile = currFile->next) {
        if (currFile->fileName == fileName) {
            std::cout << "File already being tracked." << endl << endl;
            return;
        }
        lastFile = currFile;
    } //ensure the file isn't already tracked

    miniGitFiles* newFile = new miniGitFiles; //initialize the miniGitFiles Node
    newFile->fileName = fileName;
    newFile->next = NULL;
    size_t lastindex = fileName.find_last_of('.');
    newFile->fileVersion = fileName.substr(0,lastindex) + "00" + fileName.substr(lastindex); 

    if (lastFile == NULL) { //case: first file added
        currVersion->head = newFile;
    }
    else {
        lastFile->next = newFile;
    }
    std::cout << "File succesfully staged for commit." << endl << endl;
    return;
}


void miniGit::removeFile() {
    if (!head) {
        std::cout << "No repository to remove from." << endl << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) { //make sure they are currently on the right version
        std::cout << "Return to most recent commit first" << endl << endl;
        return;
    }
    if (currVersion->head == NULL) {
        std::cout << "No staged commits to remove." << endl << endl;
        return;
    }

    string fileName;
    std::cout << "Enter the file name to be removed." << endl;
    cin >> fileName;

    miniGitFiles* fileNode = currVersion->head;
    miniGitFiles* prevNode = fileNode;
    for (fileNode; fileNode->fileName != fileName; fileNode = fileNode->next) { //find node if it exists
        if (fileNode->next == NULL) { //check if fileNode will be NULL
            std::cout << "File not being tracked" << endl << endl;
            return;
        }  
        prevNode = fileNode;
    }

    if (fileNode == currVersion->head) { //case:node is head
        currVersion->head = fileNode->next;
    }
    else {
        prevNode->next = fileNode->next;
    }
    delete fileNode;
    std::cout << "File succesfully removed from staged commit." << endl << endl;
    return;
}


miniGitFiles* copyLL(miniGitFiles* root) { //helper function to return identical linked list as passed in root
    miniGitFiles* newRoot = new miniGitFiles;
    miniGitFiles* newIterator = newRoot;
    miniGitFiles* newPrev = newRoot;
    for (miniGitFiles* it = root; it != NULL; it = it->next) {
        newPrev = newIterator;
        newIterator->fileVersion = it->fileVersion;
        newIterator->fileName = it->fileName;
        newIterator = new miniGitFiles;
        newPrev->next = newIterator;
    }
    delete newIterator;
    newPrev->next = NULL;
    return newRoot;
}

void miniGit::commit() {
    if (!head) {
        std::cout << "No repository to commit to." << endl << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) {
        std::cout << "Return to most recent commit first" << endl << endl;
        return;
    }
    if (currVersion->head == NULL) {
        std::cout << "No changes to commit" << endl << endl;
        return;
    }
    miniGitFiles* preFile = currVersion->head;
    for (miniGitFiles* it = currVersion->head; it != NULL; it = it->next) { //iterate through LL of current versions files
        if(!fs::exists(it->fileName)) { //ensure the file exists in working directory
            std::cout << "Staged file not present in working directory removing from commit: " << it->fileName << endl;
            if (it == currVersion->head) currVersion->head = it->next;
            else preFile->next = it->next;
            delete it;
            it = preFile;
            continue;
        }
        ifstream currFile(it->fileName); //open the current file
        if (!fs::exists(".minigit/" + it->fileVersion)) { //if not present in minigit
            ofstream newGitFile(".minigit/"+it->fileVersion); //create the output file
            string text;
            
            while(getline(currFile,text)) {
                newGitFile << text;
            }
            newGitFile.close();
        }
        else {
            ifstream prevVersion(".minigit/" + it->fileVersion);
            bool same = 1;
            string prevText, currText;
            while (getline(prevVersion,prevText)) {
                getline(currFile,currText);
                if (currText != prevText) {
                    same = 0;
                    break;
                } 
            } //check if the files are the same
            if(getline(currFile,currText)) same = 0; // if additions past the end of the file exist
            if (!same) {
                currFile.clear();
                currFile.seekg(0,ios::beg);
                int lastIndex = it->fileVersion.find_last_of('.');
                int newVersionNum = stoi(it->fileVersion.substr(lastIndex-2,lastIndex)) +1; //calculate new version number
                it->fileVersion = it->fileVersion.substr(0,lastIndex-2) + "0" + to_string(newVersionNum) + it->fileVersion.substr(lastIndex); //change the name of the file in LL
                ofstream newVersion(".minigit/" + it->fileVersion); //create new version file in .minigit
                string text;
                while (getline(currFile,text)) {
                    newVersion << text; //copy current file into .minigit copy
                }
                newVersion.close();
            } //if different create new file and modify the LL
            prevVersion.close();
        }
        currFile.close();
        preFile = it;
    }
    //create new doubly linked list
    commitNode* nextVersion = createCommitNode(currVersion->commitNumber + 1,copyLL(currVersion->head),currVersion);
    currVersion->next = nextVersion;
    versionNode = currVersion;
    currVersion = nextVersion;
    std::cout << "Commit successful, Commit ID: " << versionNode->commitNumber << endl << endl;
    return;
}


void miniGit::checkout() {
    if (head->next == NULL) {
        std::cout << "No repository to checkout." << endl << endl;
        return;
    }
    int commitNum;
    std::cout << "Enter the version number to checkout. Enter -1 to return to current version" << endl;
    cin >> commitNum;
    while (commitNum >= currVersion->commitNumber || commitNum < -1) {
        std::cout << "invalid version number, try again." << endl;
        cin >> commitNum;       
    }
    if (commitNum == -1) commitNum = currVersion->previous->commitNumber;

    versionNode = head;
    for (versionNode; versionNode->commitNumber != commitNum; versionNode = versionNode->next); // find the node you want to check out

    for (const auto &entry : fs::directory_iterator("./")) {
        if (entry.path() != "./.minigit" && entry.path().extension() != ".out") {
            fs::remove_all(entry.path());
        }
    } //remove everything besides .minigit subdirectory

    for (miniGitFiles* versionFiles = versionNode->head; versionFiles != NULL; versionFiles = versionFiles->next) {
        ofstream newFiles(versionFiles->fileName);
        ifstream loadedFiles(".minigit/" + versionFiles->fileVersion);

        string text;
        while(getline(loadedFiles,text)) {
            newFiles << text;
        }
        newFiles.close();
        loadedFiles.close();
    }//iterate throught the LL and copy the correct version over to the main directory
    std::cout << endl;
    return;
}
