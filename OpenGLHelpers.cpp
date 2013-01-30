#include "OpenGLHelpers.h"

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
	GLfloat xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

void resizeWindow(int x, int y, int width, int height, float fov) {
	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if (height == 0) {
		height = 1;
	}

	ratio = (GLfloat) width / (GLfloat) height;

	/* Setup our viewport. */
	glViewport(x, y, (GLint) width, (GLint) height);

	/*
	 * change to the projection matrix and set
	 * our viewing volume.
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* Set our perspective */
	gluPerspective(fov, ratio, 0.1f, 2000.0f);

	// use landscape mode...
	glRotatef(90, 0, 0, 1.0);

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode(GL_MODELVIEW);

	/* Reset The View */
	glLoadIdentity();
}

void initGL() {
	/* Enable Texture Mapping ( NEW ) */
	glEnable(GL_TEXTURE_2D);

	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	/* Depth buffer setup */
	glClearDepthf(1.0f);

	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);

	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);

	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
		GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy,
		GLfloat upz) {
	GLfloat m[16];
	GLfloat x[3], y[3], z[3];
	GLfloat mag;

	/* Make rotation matrix */

	/* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if (mag) { /* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */

	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (mag) {
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}

	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	if (mag) {
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}

#define M(row,col)  m[col*4+row]
	M(0, 0) = x[0];
	M(0, 1) = x[1];
	M(0, 2) = x[2];
	M(0, 3) = 0.0;
	M(1, 0) = y[0];
	M(1, 1) = y[1];
	M(1, 2) = y[2];
	M(1, 3) = 0.0;
	M(2, 0) = z[0];
	M(2, 1) = z[1];
	M(2, 2) = z[2];
	M(2, 3) = 0.0;
	M(3, 0) = 0.0;
	M(3, 1) = 0.0;
	M(3, 2) = 0.0;
	M(3, 3) = 1.0;
#undef M
	glMultMatrixf(m);

	/* Translate Eye to Origin */
	glTranslatef(-eyex, -eyey, -eyez);
}

GLuint initTexture(MAHandle image) {
	GLuint textureHandle;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	maOpenGLTexImage2D(image);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  	//glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	return textureHandle;
}

void TriangleStripVBO::initVBO(const MAUtil::Vector<TriangleStrip>& strips) {

	/**/ // VBO code
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	int totalDataSize = 0;
	int vertexCoordOffset = 0;
	int textureCoordOffset = 0;
	mTriangleStrips.clear();

	for(int i = 0; i < strips.size(); i++)
	{
		textureCoordOffset = vertexCoordOffset + strips[i].vertices.size()*sizeof(vec3);
		mTriangleStrips.add(TriangleStripInfo(vertexCoordOffset, textureCoordOffset, strips[i].vertices.size()));
		vertexCoordOffset = textureCoordOffset + strips[i].textureCoordinates.size()*sizeof(vec2);
	}

	totalDataSize = vertexCoordOffset;

	glBufferData(GL_ARRAY_BUFFER, totalDataSize, NULL, GL_STATIC_DRAW);

	for(int i = 0; i < strips.size(); i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, mTriangleStrips[i].vertexCoordOffset, mTriangleStrips[i].numVertices*sizeof(vec3), &strips[i].vertices[0]);
		glBufferSubData(GL_ARRAY_BUFFER, mTriangleStrips[i].textureCoordOffset, mTriangleStrips[i].numVertices*sizeof(vec2), &strips[i].textureCoordinates[0]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TriangleStripVBO::renderVBO() {

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for(int i = 0; i < mTriangleStrips.size(); i++)
	{
		glVertexPointer(3, GL_FLOAT, sizeof(vec3), (void*) mTriangleStrips[i].vertexCoordOffset);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vec2), (void*) mTriangleStrips[i].textureCoordOffset);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, mTriangleStrips[i].numVertices);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
