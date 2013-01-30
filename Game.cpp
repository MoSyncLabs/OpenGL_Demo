#include "Game.h"
#include "Player.h"
#include "RaceTrack.h"
#include "HoverCraft.h"
#include "Skybox.h"

using namespace MAUtil;

static float sFrameConstant = 0.0;
static float sFramesPerSecond = 0.0;

// static functions

float Game::getSecondsPerFrame()
{
	return sFrameConstant;
}

void Game::setFramesPerSecond(float fps)
{
	sFramesPerSecond = fps;
	if(sFramesPerSecond==0.0)
		sFramesPerSecond = 0.000000000001;
	sFrameConstant = 1.0/sFramesPerSecond;
}

float Game::getFramesPerSecond()
{
	return sFramesPerSecond;
}

// instance methods

Game::Game() : trackCamera(vec3(), vec3(), vec3())
{
}

void Game::init()
{
	Gfx_useDriverOpenGL(-1);
	initGL();

	mFont = new MAUI::Font(R_FONT);

	width = EXTENT_X(maGetScrSize());
	height = EXTENT_Y(maGetScrSize());
	resizeWindow(0, 0, width, height);

	raceTrack = new RaceTrack();
	raceTrack->init(35);

	hoverCraft = new HoverCraft();
	hoverCraft->init();

	mTexture = initTexture(R_TEXTURE);

	raceTrack->setFloorTexture(mTexture);

	skybox = new Skybox();
	skybox->init(initTexture(R_SKYMAP));

	mPlayer = new Player(raceTrack);
	mPlayer->init();

	Environment::getEnvironment().addPointerListener(this);

	// Add ourselves as a sensor listener.
	Environment::getEnvironment().addSensorListener(this);

	// Start the gyro or accelerometer sensor with a predefined rate
	// for sending events, suitable for games.
	if(maSensorStart(SENSOR_TYPE_GYROSCOPE, SENSOR_RATE_GAME) < 0) {
	}

	press = false;
}

void Game::renderView(int x, int y, int w, int h) {
	//double time = (double)maGetMilliSecondCount()*0.001;

	resizeWindow(x, y, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec3 camPos, camDir, camUp;

	raceTrack->getCameraVectors(
			mPlayer->getCraftTrackOffset().y-40.0,
			camPos, camDir, camUp);

	trackCamera.setPosition(camPos+camUp*10.0);
	trackCamera.setTarget(mPlayer->getCraftPosition());
	trackCamera.setUpVector(camUp);

	skybox->setDirection(trackCamera.getDirection());
	skybox->setUp(trackCamera.getUpVector());
	skybox->render();

	trackCamera.setCameraMatrix();

	raceTrack->render();

	mPlayer->render();

	raceTrack->renderMap(mPlayer->getCraftPosition(), mPlayer->getCraftDirection());
}

void Game::render()
{
	glClearColorx(0, 0, 0, 0xffff);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	renderView(0, 0, width, height);

	mPlayer->setCarVelocity(carVelocity);
	mPlayer->setSteeringRotation(mGyroRotation.z);
	mPlayer->update();

	mGyroRotation += mGyroSpeed*sFrameConstant;
	mGyroRotation *= 0.9;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRotatef(90, 0, 0, 1);
	glOrthof(0, (GLfloat)height, (GLfloat)width, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	char temp[1024];
	sprintf(temp, "%d", (int)carVelocity);
	mFont->drawString(temp, 20, width*5/6);

	sprintf(temp, "fps: %.4f", sFramesPerSecond);
	mFont->drawString(temp, 20, 20);
}

void Game::update()
{
	//double time = (double)maGetMilliSecondCount()*0.001;

	if(press)
	{
		carVelocity += 200.0*sFrameConstant;
		if(carVelocity > 700.0) carVelocity = 700.0;
	}
	else
	{
		carVelocity -= 300.0*sFrameConstant;
		if(carVelocity < 0.0) carVelocity = 0.0;
	}
}

/**
* This function is called with a coordinate when a pointer is pressed.
*/
void Game::multitouchPressEvent(MAPoint2d p, int touchId) {
	press = true;
}

/**
*  This function is called with a coordinate when a pointer is moved.
*/
void Game::multitouchMoveEvent(MAPoint2d p, int touchId) {
}

/**
*  This function is called with a coordinate when a pointer is released.
*/
void Game::multitouchReleaseEvent(MAPoint2d p, int touchId) {
	press = false;
}

void Game::sensorEvent(MASensor a) {

	// If the type of sensor data received is from the accelerometer
	if (a.type == SENSOR_TYPE_GYROSCOPE) {
		mGyroSpeed = vec3(a.values[0], a.values[1], a.values[2]);
	}
}

