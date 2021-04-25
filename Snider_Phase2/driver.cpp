#include <iostream>
#include "miniGit.hpp"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {
    miniGit repo;
    if (fs::exists(".minigit/.minigit.txt")) {
        std::cout << "Found repository, reconstructing the data..." << endl;
        repo.deserialise();
    }
    else {
        std::cout << "No repository found, please initialize." << endl;
        std::cout << "  (1). init" << endl;
        string request;
        cin >> request;
        while (request != "1") {
            std::cout << "Type 1 to continue." << endl;
            cin >> request;
        }
        repo.init();
    }
    while(1) {
        std::cout << "Actions: (please type the number next to the action)" << endl;
        std::cout << "   (2). add file to next commit" << endl;
        std::cout << "   (3). remove file from next commit" << endl;
        std::cout << "   (4). commit changes to the repo" << endl;
        std::cout << "   (5). checkout previous commit" << endl;
        std::cout << "   (6). diff, find the first difference between file in working directory and previous commit" << endl;
        std::cout << "   (7). status, find what files have been modified from the previous commit as well as which were removed" << endl;
        std::cout << "   (8). exit the program and serialize the data structure, make sure to commit what you need" << endl;
        std::cout << "   (9). exit the program and fully deconstruct the repository, files will remain at current checkout" << endl << endl;
        int num;
        cin >> num;
        switch (num)
        {
        case 2:
            repo.addFile();
            break;
        case 3:
            repo.removeFile();
            break;
        case 4:
            repo.commit();
            break;
        case 5:
            repo.checkout();
            break;
        case 6:
            repo.diff();
            break;
        case 7:
            repo.status();
            break;
        case 8:
            if (repo.serialize()) {
                return 0;
            }
            else {
                break;
            }
        case 9:
            repo.delGit();
            return 0;
        default:
            std::cout << "Please enter a valid command" << endl << endl;
            break;
        }
    }
}