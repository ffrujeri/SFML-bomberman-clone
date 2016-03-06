//
// Created by Côme Weber on 08/02/2016.
//

#ifndef BOMBERMAN_SETTINGSTABSCONTAINERS_HPP
#define BOMBERMAN_SETTINGSTABSCONTAINERS_HPP

#include "Container.hpp"

namespace GUI {
    namespace SettingsTabsContainers {

        void makeGeneralTab(Container::Ptr container);
        void makeAppearanceTab(Container::Ptr container);
        void makeKeymapTab(Container::Ptr container);
    };
}

#endif //BOMBERMAN_SETTINGSTABSCONTAINERS_HPP
