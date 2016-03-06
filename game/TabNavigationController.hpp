//
// Created by Côme Weber on 05/02/2016.
//

#ifndef BOMBERMAN_TABNAVIGATIONCONTROLLER_HPP
#define BOMBERMAN_TABNAVIGATIONCONTROLLER_HPP

#include <vector>
#include "Component.hpp"
#include "Container.hpp"
#include "NavigationBar.hpp"
#include "State.hpp"

namespace GUI {

    class TabNavigationController : public Component {


    public:


        TabNavigationController(int width, int height);

        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void clickedAt(int x, int y) override;


        virtual bool update(sf::Time dt) override;

/**
         * Add a tab to the navigationController given its name,
         * returns a shared_pointer toward the created container
         */
        virtual Container::Ptr addTab(std::string tabName);

    protected:

        int             mWidth;
        int             mHeight;
        int             mSeparatorDistance = 10; //vertical distance between NavBar and Container

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;


    private :

        std::vector<Container::Ptr> mContainers;
        NavigationBar   mNavBar;

    };




}

#endif //BOMBERMAN_TABNAVIGATIONCONTROLLER_HPP
