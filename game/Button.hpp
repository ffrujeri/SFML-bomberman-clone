
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "Component.hpp"
#include "ResourceIdentifiers.hpp"
#include "State.hpp"
#include "ResourceHolder.hpp"
#include "RoundedRectangleShape.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <vector>
#include <string>
#include <memory>
#include <functional>


class SoundPlayer;

namespace GUI
{

class Button : public Component
{
    public:

	typedef std::shared_ptr<Button>		Ptr;
	typedef std::function<void()>		Callback;

		enum Type
		{
			Normal,
			Hovered,
			Pressed,
			ButtonCount
		};


	public:
								Button(State::Context context);
								Button();

		void					setCallback(Callback callback);
		void					setEventHandler(std::function<void(const sf::Event&)> eventHandler);
        void					setText(const std::string& text);
        void					setToggle(bool flag);

        virtual bool			isSelectable() const;
        virtual void			select();
        virtual void			deselect();

        virtual void			activate();
        virtual void			deactivate();

        virtual void			handleEvent(const sf::Event& event);


		virtual bool 			contains(int x, int y) override;

		virtual void 			setHover(bool flag) override;

		virtual void 			setNormalColor(sf::Color);
		virtual void 			setHoverColor(sf::Color);
		virtual void 			setPressedColor(sf::Color);
		virtual void 			setSize(int width, int height);
		virtual sf::Sprite*		getSprite();
		virtual void 			drawSprite(bool flag);
		virtual void 			drawRect(bool flag);
		virtual void 			setNormalTexture(sf::Texture* texture);
		virtual void 			setHoverTexture(sf::Texture* texture);
		virtual void 			hoveredAt(int x, int y) override;
		virtual void 			clickedAt(int x, int y) override;


	virtual bool captureMouseWhenActive() override;

	virtual bool handleEventGently() override;

private:
        virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void					changeTexture(Type buttonType);


    private:
        Callback				mCallback;
        sf::Sprite				mSprite;
        sf::Text				mText;
        bool					mIsToggle;
		SoundPlayer&			mSounds;
		RoundedRectangleShape   mRectangle;
		bool 					mDrawSprite = false;
		bool 					mDrawRect = true;
		sf::Color				mColorNormal = sf::Color(50,151,225);
		sf::Color				mColorHover = sf::Color(93,172,236);
		sf::Color				mColorPressed = sf::Color(14, 71, 137);

		sf::Texture*			mTextureNormal = NULL;
		sf::Texture*			mTextureHover = NULL;
		bool 					mIsHovered = false;

		std::function<void(const sf::Event&)> mEventHandler;

};

}

#endif // BUTTON_HPP
