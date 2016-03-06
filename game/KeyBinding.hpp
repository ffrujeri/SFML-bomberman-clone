#ifndef KEYBINDING_HPP
#define KEYBINDING_HPP

#include <SFML/Window/Keyboard.hpp>

#include <map>
#include <vector>


namespace PlayerAction
{
	enum Type
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		PlaceBomb,
		GuidedBomb,
		Count
	};
}

class KeyBinding
{
	public:
		typedef PlayerAction::Type Action;


	public:
		explicit				KeyBinding(int controlPreconfiguration);

		void					assignKey(Action action, sf::Keyboard::Key key);
		sf::Keyboard::Key		getAssignedKey(Action action) const;

		bool					checkAction(sf::Keyboard::Key key, Action& out) const;
		std::vector<Action>		getRealtimeActions() const;

		void 			setControlPreconfiguration(int i);


	private:
		void					initializeActions();


	private:
		std::map<sf::Keyboard::Key, Action>		mKeyMap;
};

bool					isRealtimeAction(PlayerAction::Type action);

#endif // KEYBINDING_HPP
