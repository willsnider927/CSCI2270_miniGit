#include <iostream>
#include "miniGit.hpp"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {
    miniGit repo;
    if (fs::exists(".minigit")) {

    }
    else {
        cout << "Actions: (please type the number next to the action)" << endl;
        cout << "  (1). init" << endl;
        string request;
        cin >> request;
        while (request != "1") {
            cout << "Type 1 to continue." << endl;
            cin >> request;
        }
    }
    repo.init();
    while(1) {
        cout << "Actions: (please type the number next to the action)" << endl;
        cout << "   (2). add file to next commit" << endl;
        cout << "   (3). remove file from next commit" << endl;
        cout << "   (4). commit changes to the repo" << endl;
        cout << "   (5). checkout previous commit" << endl;
        cout << "   (6). exit the program and serialize the data structure, make sure to commit what you need" << endl << endl;
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
            repo.delGit();
            return 0;
            break;
        default:
            break;
        }
    }
}