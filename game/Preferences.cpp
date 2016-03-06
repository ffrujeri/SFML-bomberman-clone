//
// Created by Côme Weber on 07/02/2016.
//

#include <unordered_map>
#include <sstream>
#include "Preferences.hpp"

namespace Preferences {

    namespace { //Creating global and private variables for the namespace "Preferences"

        std::string     prefFile = "./bomberman.prefs",
                        tmpFile = "./bomberman.prefs.tmp";

        std::unordered_map<std::string, std::string> prefs;

        bool           hasBeenModified = false,
                        prefLoaded = false;


    }

    void add(std::string key, std::string value) {
        prefs.erase(key);
        prefs.insert(std::pair<std::string, std::string>(key,value));
        hasBeenModified = true;
    }

    std::string get(std::string key) {
        if (!prefLoaded) loadPrefs();
        auto got = prefs.find(key);
        if (got==prefs.end()) {
            //std::cerr<<"key \""<<key<<"\" not found in prefs file"<<std::endl;
            return "not found";
        }
        else return got->second;
    }


    void savePrefs() {

        if (!hasBeenModified) return;
        //write pref file

        //TODO write file but keep potentially existing comments (--> inplace modification)

        std::ofstream file(tmpFile);
        for(auto kv : prefs){
            file<<kv.first<<" "<<kv.second<<std::endl;
        }

        file.close();
        std::rename(tmpFile.c_str(), prefFile.c_str());

        //reset hasBeenModified
        hasBeenModified = false;
    }

    void loadPrefs() {
        prefs.clear();

        //open the file for reading
        std::ifstream file (prefFile, std::ios::in);

        std::string key, value, line;

        while (getline(file, line)){
            std::istringstream ss(line);
            ss>>key;
            ss>>value;
            if (key[0]!='#') add(key, value); //add entry but ignore comments
        }

        file.close();
        prefLoaded=true;

    }

    int getAsInt(std::string key) {
        std::string value = get(key);
        return value.compare("not found")!=0 ? std::stoi(value) : -1;
    }

    bool getAsBool(std::string key) {
        return get(key).compare("true") == 0;
    }


    void addAsBool(std::string key, bool b) {
        add(key, b?"true":"false");
    }

    void addAsInt(std::string key, int i) {
        add(key, std::to_string(i));
    }

    bool contains(std::string key) {
        return prefs.find(key) != prefs.end();
    }
}