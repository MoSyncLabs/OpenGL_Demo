#include "NetworkClient.h"

using namespace MAUtil;

NetworkClient::NetworkClient() :
		mId(-1)
{
	mConnection = new Connection(this);
}

void NetworkClient::addListener(NetworkClientListener *listener)
{
	mListeners.add(listener);
}

bool NetworkClient::connect(const char* addr)
{

	if(mConnection->connect(addr) < 0)
	{
		return false;
	}

	return true;
}

void NetworkClient::requestGameStart()
{
}

void NetworkClient::sendPositionUpdate(float velocity, float steer)
{
}

void NetworkClient::connectFinished(Connection* conn, int result)
{
}

void NetworkClient::connRecvFinished(Connection* conn, int result)
{
}

void NetworkClient::connWriteFinished(Connection* conn, int result)
{
}

void NetworkClient::connReadFinished(Connection* conn, int result)
{

}

void NetworkClient::fireConnectionEstablished(int yourId)
{
	mId = yourId;
	for(int i = 0; i < mListeners.size(); i++)
	{
		mListeners[i]->connectionEstablished(yourId);
	}
}

void NetworkClient::fireCompetitorConnected(int id)
{
	for(int i = 0; i < mListeners.size(); i++)
	{
		mListeners[i]->competitorConnected(id);
	}
}

void NetworkClient::fireStartLoadingGame(int seed)
{
	for(int i = 0; i < mListeners.size(); i++)
	{
		mListeners[i]->startLoadingGame(seed);
	}
}

void NetworkClient::fireStartGame()
{
	for(int i = 0; i < mListeners.size(); i++)
	{
		mListeners[i]->startGame();
	}
}

void NetworkClient::firePlayerPositionUpdate(int id, float velocity, float steer)
{
	for(int i = 0; i < mListeners.size(); i++)
	{
		mListeners[i]->playerPositionUpdate(id, velocity, steer);
	}
}
