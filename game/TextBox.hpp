//
// Created by Côme Weber on 09/02/2016.
//

#ifndef BOMBERMAN_TEXTBOX_HPP
#define BOMBERMAN_TEXTBOX_HPP

#include "Component.hpp"
#include "RoundedRectangleShape.hpp"
#include <SFML/Graphics.hpp>

namespace GUI {

    class TextBox : public Component{

    protected:
        std::string             mString = "";
        std::string             mKeyHolder = ""; //default text when no text has been typed
        int                     mWidth=100, mHeight=40;
        std::function<void()>   mDeactivationCallback = [](){};
        int                     mCurrentCaretPosition = 0;
        RoundedRectangleShape   mBackgroundRectangle;
        sf::Color               mInactiveColor = sf::Color(189, 195, 199);
        sf::Color               mTextColor = sf::Color(52,73,96);
        sf::Keyboard::Key       mCurrentKey = sf::Keyboard::Key::Unknown;
        int                     mCurrentKeyCount=-10;
        int                     mCursorBlinkCount=0;
        int                     mViewOffset=1;


    public:
        virtual void activate() override;

        virtual void deactivate() override;

        sf::Color               mActiveColor = sf::Color(31, 188, 153);


    public:

                    TextBox(std::string keyHolder);

        virtual void setKeyHolder(std::string keyholder);

        virtual void setSize(sf::Vector2f size);

        virtual void setBackgroundColor(sf::Color color);

        virtual void setTextColor(sf::Color color);

        virtual void setKeyHolderColor(sf::Color color);

        virtual void setText(std::string text);

        virtual std::string getText();

        virtual void setDeactivationCallback(std::function<void()> callback);

        virtual sf::Vector2f getSize();

        virtual bool contains(int x, int y) override;

        virtual bool isSelectable() const;

        virtual void handleEvent(const sf::Event &event);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void clickedAt(int x, int y) override;

        virtual bool captureMouseWhenActive() override;

        virtual bool update(sf::Time dt) override;

        bool isUnicodeAllowed(sf::Uint32 unicode);

        void handleKey(sf::Keyboard::Key key);

        void updateDrawConstants();
    };

}

#endif //BOMBERMAN_TEXTBOX_HPP
