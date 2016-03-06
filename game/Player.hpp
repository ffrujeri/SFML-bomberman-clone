#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Command.hpp"
#include "KeyBinding.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <map>


class CommandQueue;

class Player : private sf::NonCopyable
{
	public:
		typedef PlayerAction::Type Action;

		enum GameStatus
		{
			GameRunning,
			GameSuccess,
			GameLost
		};


	public:
								Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);

		void					handleEvent(const sf::Event& event, CommandQueue& commands);
		void					handleRealtimeInput(CommandQueue& commands);
		void					handleRealtimeNetworkInput(CommandQueue& commands);

		// React to events or realtime state changes received over the network
		void					handleNetworkEvent(Action action, CommandQueue& commands);
		void					handleNetworkRealtimeChange(Action action, bool actionEnabled);

		void 					setGameStatus(GameStatus status);
		GameStatus getGameStatus() const;

		void					disableAllRealtimeActions();
		bool					isLocal() const;


	private:
		void					initializeActions();


	private:
		const KeyBinding*			mKeyBinding;
		std::map<Action, Command>	mActionBinding;
		std::map<Action, bool>		mActionProxies;
		GameStatus mCurrentMissionStatus;
		int							mIdentifier;
		sf::TcpSocket*				mSocket;
};

#endif // PLAYER_HPP
