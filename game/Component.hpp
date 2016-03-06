#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <memory>
#include <SFML/Graphics.hpp>


namespace sf
{
	class Event;
}

namespace GUI
{
class Component : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
    public:
        typedef std::shared_ptr<Component> Ptr;


	public:
							Component();
        virtual				~Component();

        virtual bool		isSelectable() const = 0;
		bool				isSelected() const;
        virtual void		select();
        virtual void		deselect();
		virtual bool 		captureMouseWhenActive();

        virtual bool		isActive() const;
        virtual void		activate();
        virtual void		deactivate();
		virtual bool 		update(sf::Time dt);

        virtual void		handleEvent(const sf::Event& event) = 0;
		virtual bool		handleEventGently();
		virtual bool 		contains(int x, int y);
		virtual void		setHover(bool flag){}
		virtual void		hoveredAt(int x,int y){} //do whatever you have to do if mouse is at position (x,y)
		virtual void		clickedAt(int x, int y){} //idem if mouse click at (x,y)

		virtual sf::Vector2f getSize(){return sf::Vector2f(0,0);};

		virtual bool 		draw();

    private:
        bool				mIsSelected;
        bool				mIsActive;

};

}

#endif // COMPONENT_HPP
