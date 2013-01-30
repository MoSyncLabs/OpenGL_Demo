#include "RaceTrack.h"
#include <mastdlib.h>

using namespace MAUtil;

template <typename T>
inline T catmull_rom_weight(const Vector<T>& points, int i) {

	int negative = i-1;
	if(negative<0) negative = points.size()-1;
	size_t i1 = (size_t)negative;
	size_t i2 = (size_t)(i+1) % points.size();

	return (points[i2]-points[i1])*0.5f;
}

template<typename T>
T spline_interpolate(float s, const T& P1, const T& P2, const T T1, const T T2) {
	float h1 =  2*s*s*s - 3*s*s + 1;          // calculate basis function 1
	float h2 = -2*s*s*s + 3*s*s;              // calculate basis function 2
	float h3 =    s*s*s - 2*s*s + s;		  // calculate basis function 3
	float h4 =    s*s*s -   s*s;              // calculate basis function 4

	T p = P1*h1 +							  // multiply and sum all funtions
		P2*h2 +								  // together to build the interpolated
		T1*h3 +								  // point along the curve.
		T2*h4;
	return p;
}

template <typename T>
T catmull_rom_interpolate(const Vector<T>& src, int i, float t) {
	const T& P1 = src[(i)%src.size()];
	const T& P2 = src[(i+1)%src.size()];
	const T T1 = catmull_rom_weight<T>(src, i);
	const T T2 = catmull_rom_weight<T>(src, i+1);	 
	return spline_interpolate(t, P1, P2, T1, T2);
}

template <typename T>
void catmull_rom_interpolate(const Vector<T>& src, Vector<T>& dst, int i, int steps) {
	const T& P1 = src[(i)%src.size()];
	const T& P2 = src[(i+1)%src.size()];
	const T T1 = catmull_rom_weight<T>(src, i);
	const T T2 = catmull_rom_weight<T>(src, i+1);	 

	for(int t = 0; t < steps; t++) {
		float s = (float)t / (float) steps;		// scale s to go from 0 to 1
		dst.add(spline_interpolate(s, P1, P2, T1, T2));
	}
}

void RaceTrack::setFloorTexture(GLuint floorTexture) {
	this->floorTexture = floorTexture;
}

float RaceTrack::getWidth()
{
	return mTrackWidth;
}

void RaceTrack::init(int numRaceTrackNodes) {
	const float totalSize = 400.0;
	vec3 upVector(0.0f, 1.0f, 0.0f);

	mTrackWidth = 60.0f;

	vec3 trackNormal = upVector;
	vec3 centriDir = upVector;
	vec3 start;

	MAUtil::Vector<vec3> iLeftPoints;
	MAUtil::Vector<vec3> iRightPoints;
	MAUtil::Vector<TriangleStrip> strips;

	for(int i = 0; i < numRaceTrackNodes; i++) {
		float t = ((float)i/(float)numRaceTrackNodes);
		float x = cos(t*2.0*3.14159)*totalSize*3.0;
		float z = sin(t*4.0*3.14159)*totalSize*3.0;

		float y = z + cos(t*1.0*3.14159)*(totalSize/4.0) + sin(t*8.0*3.14159)*(totalSize/2.0)*sin(t*3.14159);

		float trackWidth = mTrackWidth;
		vec3 end(x, y, z);

		if(i > 0) {
			vec3 direction = end-start;
			direction.normalize();

			float banking = 0.15;
			trackNormal += centriDir*banking;
			trackNormal.normalize();

			vec3 ortho = direction.cross(trackNormal);
			ortho.normalize();

			centriDir = direction.cross(ortho);
			centriDir.normalize();

			ortho *= trackWidth;

			leftPoints.add(start-ortho);
			rightPoints.add(start+ortho);
			if(i == numRaceTrackNodes-1) {
				leftPoints.add(end-ortho);
				rightPoints.add(end+ortho);
			}
		} 

		start = end;
	}


	for(int i = 0; i < leftPoints.size(); i++) {
		int steps = 100;

		float scale = (leftPoints[(i+1)%leftPoints.size()] - leftPoints[i]).length()/(totalSize);
		steps = (int)((float)steps*scale);
		if(steps<1)steps = 1;

		catmull_rom_interpolate<vec3>(leftPoints, iLeftPoints, i, steps);
		catmull_rom_interpolate<vec3>(rightPoints, iRightPoints, i, steps);
	}

	mTotalLength = 0;

	strips.resize(1);
	for(int i = 0; i <= iLeftPoints.size(); i++) {
		int startIndex = i-1;
		if(startIndex<0) startIndex = iLeftPoints.size()-1;
		vec3 startLeft = iLeftPoints[startIndex];
		vec3 startRight = iRightPoints[startIndex];
		float distance = (float)i / (float) iLeftPoints.size();  
		float scale = 50.0f;
		distance *= scale;
		float uLeftStart = 0.0;
		float vLeftStart = distance;
		float uRightStart = 1.0;
		float vRightStart = distance;
		strips[0].vertices.add(startLeft);
		strips[0].vertices.add(startRight);
		strips[0].textureCoordinates.add(vec2(uLeftStart, vLeftStart));
		strips[0].textureCoordinates.add(vec2(uRightStart, vRightStart));


		vec3 mid_start = startLeft + (startRight-startLeft)*0.5;
		vec3 mid_end = iLeftPoints[i] + (iRightPoints[i]-iLeftPoints[i])*0.5;
		mTotalLength += (mid_end-mid_start).length();
	}

	initVBO(strips);
}

void RaceTrack::getCameraVectors(float t, vec3& position, vec3& direction, vec3& up) {
	t+=mTotalLength;
	t/=mTotalLength;
	t = fmod(t, 1.0f);
	float location = t*(float)(leftPoints.size());
	float flooredLocation = floor(location);
	int i = (int)flooredLocation;
	location -= flooredLocation;

	// spline interpolation
	vec3 leftPos = catmull_rom_interpolate(leftPoints, i, location);
	vec3 rightPos = catmull_rom_interpolate(rightPoints, i, location);
	vec3 nextLeftPos = catmull_rom_interpolate(leftPoints, i, location + 0.001f);
	vec3 nextRightPos = catmull_rom_interpolate(rightPoints, i, location + 0.001f);
	vec3 target = nextLeftPos + (nextRightPos - nextLeftPos)*0.5f;
	vec3 pos = leftPos + (rightPos - leftPos)*0.5f;

	direction = target-pos;
	direction.normalize();

	vec3 ortho = (rightPos-leftPos);
	position = leftPos + (ortho/2.0);
	ortho.normalize();

	up = direction.cross(ortho); 
	up.normalize();

	position += up*20.0;
}

void RaceTrack::render() {
	glColor4x(0xffff, 0xffff, 0xffff, 0xffff);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, floorTexture);

	renderVBO();
}


void RaceTrack::renderMap(const vec3 &pos, const vec3& dir)
{
	glColor4x(0xffff, 0xffff, 0xffff, 0xffff>>2);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(3.2,-2.2, -5);
	glRotatef(-90, 1, 0, 0);

	vec2 up = vec2(0, 1);
	vec2 dir2d = vec2(dir.x, dir.z);
	dir2d.normalize();

	float angle = up.signed_angle(dir2d);
	glRotatef(angle*360/(M_PI*2.0), 0, 1, 0);

	glScalef(0.00075, 0.0000, 0.00075);

	renderVBO();


	// render marker
	// Set up the vertex array.
	GLfloat vcoords[4*3] = {
		-30.0f, 0.0f, -30.0f,
		 30.0f, 0.0f, -30.0f,
		-30.0f,  0.0f, 30.0f,
		 30.0f,  0.0f, 30.0f,
	};

	for(int i = 0; i < 4; i++)
	{
		vcoords[i*3+0] += pos.x;
		vcoords[i*3+1] += pos.y;
		vcoords[i*3+2] += pos.z;
	}

	glDisable(GL_BLEND);
	glColor4x(0xffff, 0, 0, 0xffff);

	// Set pointers to vertex coordinates and colors.
	glVertexPointer(3, GL_FLOAT, 0, vcoords);
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);


	glPopMatrix();
}
