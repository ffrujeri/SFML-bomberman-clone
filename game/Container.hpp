#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "Component.hpp"
#include <vector>
#include <memory>


namespace GUI
{
class Container : public Component
{
    public:
        typedef std::shared_ptr<Container> Ptr;
            

	public:
							Container();

        void				pack(Component::Ptr component);

        virtual bool		isSelectable() const;
        virtual void		handleEvent(const sf::Event& event);
		void				select(std::size_t index); //select ith child
		virtual bool 		update(sf::Time dt) override;

protected:
        virtual void		draw(sf::RenderTarget& target, sf::RenderStates states) const;

        bool				hasSelection() const;

        void				selectNext();
        void				selectPrevious();

	protected:
        std::vector<Component::Ptr>		mChildren;
        int								mSelectedChild;

};

}

#endif // CONTAINER_HPP
