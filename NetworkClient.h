#ifndef _NETWORK_CLIENT_H_
#define _NETWORK_CLIENT_H_

#include <MAUtil/Vector.h>
#include <MAUtil/Connection.h>

class NetworkClientListener
{
public:
	virtual void connectionEstablished(int yourId) = 0;
	virtual void competitorConnected(int id) = 0;
	virtual void startLoadingGame(int seed) = 0;
	virtual void startGame() = 0;
	virtual void playerPositionUpdate(int id, float velocity, float steer) = 0;
};

class NetworkClient : public MAUtil::ConnectionListener
{
public:
	NetworkClient();
	virtual ~NetworkClient() {}

	void addListener(NetworkClientListener *listener);

	bool connect(const char* addr);
	void requestGameStart();
	void sendPositionUpdate(float velocity, float steer);

private:
	virtual void connectFinished(MAUtil::Connection* conn, int result);
	virtual void connRecvFinished(MAUtil::Connection* conn, int result);
	virtual void connWriteFinished(MAUtil::Connection* conn, int result);
	virtual void connReadFinished(MAUtil::Connection* conn, int result);

	void fireConnectionEstablished(int yourId);
	void fireCompetitorConnected(int id);
	void fireStartLoadingGame(int seed);
	void fireStartGame();
	void firePlayerPositionUpdate(int id, float velocity, float steer);

	MAUtil::Vector<NetworkClientListener*> mListeners;

	MAUtil::Connection* mConnection;

	int mId;
};

#endif // _NETWORK_CLIENT_H_
