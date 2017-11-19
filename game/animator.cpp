#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include "animator.h"

using namespace sf;
using namespace std;

int LoadAnimLoop(string path, map<string, AnimatorResource> &resources)
{
    int nSheets;
    ifstream infile(path.c_str());
    string line;
    istringstream iss;

    getline(infile, line);
    iss.str(line);
    iss >> nSheets;
    for (int sh = 0; sh < nSheets; sh++) {
        int nAnims;
        string texPath;
        sf::Texture tex;
        getline(infile, texPath);
        AnimatorResource ar;
        size_t found = texPath.rfind("/");
        if (found != string::npos) {
            ar.name = texPath.substr(found + 1);
            ar.name = ar.name.substr(0, ar.name.length() - 4);
        } else
            ar.name = texPath;

        if (!ar.texSheet.loadFromFile(texPath)) {
            cout << "ERRROR" << endl;
            return -1;
        }

        getline(infile, line);
        iss.str(line);
        iss >> nAnims;
        cout << "[" <<  ar.name << "]" << endl;
        for (int i = 0; i < nAnims; i++) {
            int animId, dirId, nFrags, timeMsec;
            getline(infile, line);
            iss.str(line);
            iss >> animId >> dirId >> nFrags >> timeMsec;
            cout << "anim " << i << " id " << animId << "dirId" << dirId <<
                " fragments " << nFrags  << "time " << timeMsec << endl;
            for (int j = 0; j < nFrags; j++) {
                int x, y, w, h;
                getline(infile, line);
                iss.str(line);
                iss >> x >> y >> w >> h;
                cout << "reg: " << x << " " << y << " " << w <<
                    " " << h << endl;
                ar.AddAnim((AnimId) animId, (DirId) dirId, timeMsec,
                        new sf::IntRect(x, y, w, h));
                iss.clear();
                iss.str(std::string());
            }
        }
        resources[ar.name] = ar;
    }
    return 0;
}
