#ifndef _OPENGL_HELPERS_H_
#define _OPENGL_HELPERS_H_

#include <ma.h>
#include <IX_WIDGET.h>
#include <GLES/gl.h>
#include "vec.h"
#include <MAUtil/Vector.h>

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
void resizeWindow(int x, int y, int width, int height, float fov=90.0f);
void initGL();
void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
          GLfloat centerx, GLfloat centery, GLfloat centerz,
          GLfloat upx, GLfloat upy, GLfloat upz);

GLuint initTexture(MAHandle image);

class TriangleStripVBO
{
public:
	struct TriangleStrip {
		TriangleStrip()
		{

		}

		MAUtil::Vector<vec3> vertices;
		MAUtil::Vector<vec2> textureCoordinates;
	};

	void initVBO(const MAUtil::Vector<TriangleStrip>& strips);
	void renderVBO();

protected:
	struct TriangleStripInfo
	{
		TriangleStripInfo()
		{
		}

		TriangleStripInfo(int vertexCoordOffset, int textureCoordOffset, int numVertices) :
			vertexCoordOffset(vertexCoordOffset), textureCoordOffset(textureCoordOffset), numVertices(numVertices)
		{
		}

		int vertexCoordOffset;
		int textureCoordOffset;
		int numVertices;
	};

	MAUtil::Vector<TriangleStripInfo> mTriangleStrips;

	GLuint mVBO;
	int mVerticesOffset;
	int mTextureCoordOffset;
	int mNumVertices;

};

#endif
