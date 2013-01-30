#include "Skybox.h"
#include <mastdlib.h>

using namespace MAUtil;

static void getVertexFromSphericalCoordinates(float radius, float theta, float phi, float& x, float &y, float &z, float &u, float& v) {
	x = radius * cos((phi / 180.0f) * M_PI) * cos((theta / 180.0f) * M_PI);
	y = radius * sin((phi / 180.0f) * M_PI) * cos((theta / 180.0f) * M_PI);
	z = radius * sin((theta / 180.0f) * M_PI);
	u = (phi) / 360.0f;
	v = (2.0f * (theta + 90.0f)) / 360.0f;
}

void Skybox::init(GLuint skymap) {

	mSkymap = skymap;

	const int numSegments = 16;
	const int numSlices = 16;
	const float radius = 1.0f;

	MAUtil::Vector<TriangleStrip> strips;
	float theta = -90.0f;
	float phi = 0.0f;

	float thetaDelta = (180.0f)/(float)numSegments;
	float phiDelta = (360.0f)/(float)numSlices;

	for(int j = 0; j < numSegments; j++)
	{
		strips.add(TriangleStrip());
		TriangleStrip& strip = strips[strips.size()-1];
		phi = 0.0f;
		for(int i = 0; i <= numSlices; i++)
		{
			vec3 v1;
			vec2 t1;
			vec3 v2;
			vec2 t2;
			getVertexFromSphericalCoordinates(radius, theta, phi, v1.x, v1.y, v1.z, t1.x, t1.y);
			getVertexFromSphericalCoordinates(radius, theta+thetaDelta, phi, v2.x, v2.y, v2.z, t2.x, t2.y);
			phi += phiDelta;

			strip.vertices.add(v1);
			strip.vertices.add(v2);
			strip.textureCoordinates.add(t1);
			strip.textureCoordinates.add(t2);
		}
		theta+=thetaDelta;
	}

	initVBO(strips);
}

void Skybox::setDirection(const vec3& dir)
{
	mDirection = dir;
}

void Skybox::setUp(const vec3& up)
{
	mUp = up;
}

void Skybox::render() {
	glPushMatrix();
	glLoadIdentity();

	gluLookAt(0, 0, 0 ,mDirection.x, mDirection.y, mDirection.z, mUp.x, mUp.y, mUp.z);
	glRotatef(90, 0, 0, 1);

	glColor4x(0xffff, 0xffff, 0xffff, 0xffff);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, mSkymap);

	renderVBO();

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
}
