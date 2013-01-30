#ifndef _GAME_H_
#define _GAME_H_

#include <MAUtil/Environment.h>
#include "OpenGLHelpers.h"
#include <MAUtil/Map.h>
#include <MAUtil/Vector.h>
#include "MAHeaders.h"
#include "Camera.h"
#include <mavsprintf.h>
#include <MAUtil/Graphics.h>
#include <MAUI/Font.h>

class Player;
class RaceTrack;
class Skybox;
class HoverCraft;

class Game : public MAUtil::PointerListener, public MAUtil::SensorListener
{
public:
	Game();
	void init();
	void render();
	void update();

	static float getSecondsPerFrame();
	static float getFramesPerSecond();
	static void setFramesPerSecond(float fps);

private:
	void pointerPressEvent(MAPoint2d p) {}
	void pointerMoveEvent(MAPoint2d p) {}
	void pointerReleaseEvent(MAPoint2d p) {}
	void multitouchPressEvent(MAPoint2d p, int touchId);
	void multitouchMoveEvent(MAPoint2d p, int touchId);
	void multitouchReleaseEvent(MAPoint2d p, int touchId);
	void sensorEvent(MASensor a);

	void renderView(int x, int y, int w, int h);

	GLuint mTexture;
	int width, height;
	RaceTrack* raceTrack;
	HoverCraft* hoverCraft;
	Camera trackCamera;
	Skybox* skybox;
	Player* mPlayer;
	float carVelocity;
	vec3 mGyroRotation;
	vec3 mGyroSpeed;
	MAUI::Font* mFont;
	bool press;

};

#endif // _GAME_H_
