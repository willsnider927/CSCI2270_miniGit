#include <filesystem>
#include <iostream>
#include <fstream>
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
        cout << "No repository to add to." << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) {
        cout << "Return to most recent commit first" << endl;
        return;
    }    
    string fileName;
    cout << "Enter the file name to be added." << endl;
    cin >> fileName;
    while (!fs::exists(fileName)) {
        cout << "File not found, try again." << endl;
        cin >> fileName;
    } //check for valid file name

    miniGitFiles* currFile = currVersion->head;
    miniGitFiles* lastFile = currFile; //useful for adding if it is allowed to be
    for (currFile; currFile != NULL; currFile = currFile->next) {
        if (currFile->fileName == fileName) {
            cout << "File already being tracked." << endl;
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
    return;
}


void miniGit::removeFile() {
    if (!head) {
        cout << "No repository to remove from." << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) { //make sure they are currently on the right version
        cout << "Return to most recent commit first" << endl;
        return;
    }
    if (currVersion->head == NULL) {
        cout << "No staged commits to remove." << endl;
        return;
    }

    string fileName;
    cout << "Enter the file name to be removed." << endl;
    cin >> fileName;

    miniGitFiles* fileNode = currVersion->head;
    miniGitFiles* prevNode = fileNode;
    for (fileNode; fileNode->fileName != fileName; fileNode = fileNode->next) { //find node if it exists
        if (fileNode->next == NULL) { //check if fileNode will be NULL
            cout << "File not being tracked" << endl;
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
    return;
}


miniGitFiles* copyLL(miniGitFiles* root) { //helper function to return identical linked list as passed in root
    miniGitFiles* newRoot = new miniGitFiles;
    miniGitFiles* newIterator = newRoot;
    for (miniGitFiles* it = root; it != NULL; it = it->next) {
        newIterator->fileVersion = it->fileVersion;
        newIterator->fileName = it->fileName;
        miniGitFiles* nextStep = new miniGitFiles;
        newIterator->next = nextStep;
        newIterator = nextStep;
    }
    newIterator = NULL;
    return newRoot;
}

void miniGit::commit() {
    if (!head) {
        cout << "No repository to commit to." << endl;
        return;
    }
    if (currVersion->previous != versionNode && currVersion->previous != NULL) {
        cout << "Return to most recent commit first" << endl;
        return;
    }
    if (currVersion->head == NULL) {
        cout << "No changes to commit" << endl;
        return;
    }
    for (miniGitFiles* it = currVersion->head; it != NULL; it = it->next) { //iterate through LL of current versions files
        ifstream currFile(it->fileName); //open the current file
        if (!fs::exists(".minigit/" + it->fileVersion)) { //if not present in minigit
            ofstream newGitFile(".minigit/"+it->fileVersion); //create the output file
            string text;
            
            while(getline(currFile,text)) {
                newGitFile << text;
            }
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
            } //if different create new file and modify the LL
        }
    }
    //create new doubly linked list
    commitNode* nextVersion = createCommitNode(currVersion->commitNumber + 1,copyLL(currVersion->head),currVersion);
    currVersion->next = nextVersion;
    versionNode = currVersion;
    currVersion = nextVersion;
    return;
}


void miniGit::checkout() {
    if (!head) {
        cout << "No repository to checkout." << endl;
        return;
    }
    int commitNum;
    cout << "Enter the version number to checkout. Enter -1 to return to current version" << endl;
    cin >> commitNum;
    while (commitNum >= currVersion->commitNumber || commitNum < -1) {
        cout << "invalid version number, try again." << endl;
        cin >> commitNum;       
    }
    if (commitNum == -1) commitNum = currVersion->previous->commitNumber;

    versionNode = head;
    for (versionNode; versionNode->commitNumber != commitNum; versionNode = versionNode->next); // find the node you want to check out

    for (const auto &entry : fs::directory_iterator("./")) {
        if (entry.path() != "./.minigit" && entry.path() != "./a.out") {
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
    }//iterate throught the LL and copy the correct version over to the main directory
    return;
}
