#include <MAUtil/GLMoblet.h>
#include "Game.h"

using namespace MAUtil;


class MyMoblet : public GLMoblet {
private:
	Game* mGame;
public:

	MyMoblet() : GLMoblet(GLMoblet::GL1) {
		// todo: initialize
	}

	void init()
	{
		mGame = new Game();
		mGame->init();
	}

	void keyPressEvent(int keyCode, int nativeCode) {
		// todo: handle key presses
	}

	void keyReleaseEvent(int keyCode, int nativeCode) {
		// todo: handle key releases
	}

	void draw()
	{
		Game::setFramesPerSecond(this->getActualFramesPerSecond());
		mGame->update();
		mGame->render();
	}
};

extern "C" int MAMain() {
	Moblet::run(new MyMoblet());
	return 0;
};
