//
// Created by Côme Weber on 07/02/2016.
//

#ifndef BOMBERMAN_PREFERENCES_HPP
#define BOMBERMAN_PREFERENCES_HPP

#include <fstream>
#include <iostream>


/**
 * Functions for reading and saving preferences for the settings.
 * For the moment it is a generic class, not specific to bomberman.
 * Settings are stored as <key, values> entries.
 * keys and values are std::strings and must not contain whitespaces.
 * Nevertheless can values of type int or bool can be inserted and retrieved by using the appropriate function
 */
namespace Preferences {


    /**
     * stores the pair <key, value>.
     * Caution : if the key already exists, the value is overwritten.
     */
    void add(std::string key, std::string value);

    /**
    * Caution : if the key already exists, the value is overwritten.
    */
    void addAsBool(std::string key, bool b);

    /**
    * Caution : if the key already exists, the value is overwritten.
    */
    void addAsInt(std::string key, int i);

    std::string get(std::string key);

    int getAsInt(std::string key);

    bool getAsBool(std::string key);

    void savePrefs();

    void loadPrefs();

    /**
     * returns true if the stored preferences has a value for the given key
     */
    bool contains(std::string key);

}


#endif //BOMBERMAN_PREFERENCES_HPP
