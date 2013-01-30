#include "HoverCraft.h"
#include <MAUtil/Vector.h>
#include "OpenGLHelpers.h"

using namespace MAUtil;

// works with quads
class Modeller
{
public:
	struct Quad
	{
		Quad()
		{
		}

		Quad(int a, int b, int c ,int d) :
			a(a), b(b), c(c), d(d)
		{
		}

		int a, b, c ,d;
	};

	struct Vertex
	{
		Vertex()
		{
		}

		Vertex(const vec3& pos) :
			pos(pos)
		{

		}

		vec3 pos;
	};


	enum BoxIndecies
	{
		BI_FRONT,
		BI_BACK,
		BI_LEFT,
		BI_RIGHT,
		BI_TOP,
		BI_BOTTOM
	};

	Vector<int> addBox(vec3 pos, vec3 size)
	{
		Vector<int> indexMap;

		pos-=size/2;
		mVertices.add(Vertex(pos + vec3(0, 0, 0)));
		mVertices.add(Vertex(pos + vec3(size.x, 0, 0)));
		mVertices.add(Vertex(pos + vec3(size.x, size.y, 0)));
		mVertices.add(Vertex(pos + vec3(0, size.y, 0)));
		mVertices.add(Vertex(pos + vec3(0, 0, size.z)));
		mVertices.add(Vertex(pos + vec3(size.x, 0, size.z)));
		mVertices.add(Vertex(pos + vec3(size.x, size.y, size.z)));
		mVertices.add(Vertex(pos + vec3(0, size.y, size.z)));

		Quad front = Quad(3, 2, 1, 0);
		Quad back = Quad(4, 5, 6, 7);

		Quad left = Quad(0, 4, 7, 3);
		Quad right = Quad(2, 6, 5, 1);

		Quad top = Quad(0, 4, 5, 1);
		Quad bottom = Quad(2, 6, 7, 3);

		int startIndex = mQuads.size();
		mQuads.add(front);
		mQuads.add(back);
		mQuads.add(left);
		mQuads.add(right);
		mQuads.add(top);
		mQuads.add(bottom);

		indexMap.clear();
		for(int i = 0; i < 6; i++)
		{
			indexMap.add(startIndex + i);
		}

		return indexMap;
	}

	vec3 getNormal(const Vertex& a, const Vertex& b, const Vertex& c)
	{
		vec3 normal = (b.pos-a.pos).cross(c.pos-a.pos);
		normal.normalize();
		return normal;
	}

	enum ExtrudeIndecies
	{
		EI_FRONT = 0,
		EI_BACK = 1,
		EI_LEFT = 2,
		EI_RIGHT = 3,
		EI_TOP= 4
	};

	Vector<int> extrude(int quadIndex, float amount)
	{
		const Quad& quad = mQuads[quadIndex];

		const Vertex& a = mVertices[quad.a];
		const Vertex& b = mVertices[quad.b];
		const Vertex& c = mVertices[quad.c];
		const Vertex& d = mVertices[quad.d];

		vec3 normal = getNormal(a, b, c);
		normal = normal * amount;

		vec3 newA = a.pos + normal;
		vec3 newB = b.pos + normal;
		vec3 newC = c.pos + normal;
		vec3 newD = d.pos + normal;

		int newAi = mVertices.size() + 0;
		int newBi = mVertices.size() + 1;
		int newCi = mVertices.size() + 2;
		int newDi = mVertices.size() + 3;

		mVertices.add(newA);
		mVertices.add(newB);
		mVertices.add(newC);
		mVertices.add(newD);

		int startIndex = mQuads.size();
		Quad aq = Quad(quad.a, quad.b, newBi, newAi);
		Quad bq = Quad(quad.b, quad.c, newCi, newBi);
		Quad cq = Quad(quad.c, quad.d, newDi, newCi);
		Quad dq = Quad(quad.d, quad.a, newAi, newDi);
		Quad lid = Quad(newAi, newBi, newCi, newDi);

		mQuads.add(aq);
		mQuads.add(bq);
		mQuads.add(cq);
		mQuads.add(dq);
		mQuads.add(lid);


		Vector<int> ret;
		ret.add (startIndex + 0);
		ret.add (startIndex + 1);
		ret.add (startIndex + 2);
		ret.add (startIndex + 3);
		ret.add (startIndex + 4);

		mQuadsTaggedForDeletion.add(quadIndex);

		return ret;
	}

	void scale(int quadIndex, vec2 size)
	{
		const Quad& quad = mQuads[quadIndex];

		Vertex& a = mVertices[quad.a];
		Vertex& b = mVertices[quad.b];
		Vertex& c = mVertices[quad.c];
		Vertex& d = mVertices[quad.d];

		vec3 mid_ab = a.pos + (b.pos-a.pos)*0.5;
		vec3 mid_cd = c.pos + (d.pos-c.pos)*0.5;
		vec3 mid_bc = b.pos + (c.pos-b.pos)*0.5;
		vec3 mid_da = d.pos + (a.pos-d.pos)*0.5;

		vec3 mid = mid_ab + (mid_cd-mid_ab)*0.5;

		a.pos = mid + (mid_ab-mid) * size.x + (mid_da-mid) * size.y;
		b.pos = mid + (mid_ab-mid) * size.x + (mid_bc-mid) * size.y;
		c.pos = mid + (mid_cd-mid) * size.x + (mid_bc-mid) * size.y;
		d.pos = mid + (mid_cd-mid) * size.x + (mid_da-mid) * size.y;
	}

	// orientation = 0/1
	enum SplitIndecies
	{
		SI_LEFT = 0,
		SI_RIGHT = 1
	};

	Vector<int> split(int quadIndex, int orientation, float amount)
	{
		const Quad& quad = mQuads[quadIndex];
		Vector<int> indecies;
		indecies.add(mQuads.size());
		indecies.add(mQuads.size()+1);

		Vertex& a = mVertices[quad.a];
		Vertex& b = mVertices[quad.b];
		Vertex& c = mVertices[quad.c];
		Vertex& d = mVertices[quad.d];

		if(orientation == 0)
		{
			vec3 mid_ab = a.pos + (b.pos-a.pos)*amount;
			vec3 mid_dc = d.pos + (c.pos-d.pos)*amount;
			int vertexIndex = mVertices.size();
			mVertices.add(mid_ab);
			mVertices.add(mid_dc);
			mQuadsTaggedForDeletion.add(quadIndex);
			mQuads.add(Quad(quad.a, quad.d, vertexIndex+1, vertexIndex));
			mQuads.add(Quad(quad.b, quad.c, vertexIndex+1, vertexIndex));
		} else {
			vec3 mid_ac = a.pos + (c.pos-a.pos)*amount;
			vec3 mid_bd = b.pos + (d.pos-b.pos)*amount;
			int vertexIndex = mVertices.size();
			mVertices.add(mid_ac);
			mVertices.add(mid_bd);
			mQuadsTaggedForDeletion.add(quadIndex);
			mQuads.add(Quad(quad.a, quad.b, vertexIndex+1, vertexIndex));
			mQuads.add(Quad(quad.c, quad.d, vertexIndex+1, vertexIndex));
		}

		return indecies;
	}

	void translate(int quadIndex, vec2 offset)
	{
		const Quad& quad = mQuads[quadIndex];

		Vertex& a = mVertices[quad.a];
		Vertex& b = mVertices[quad.b];
		Vertex& c = mVertices[quad.c];
		Vertex& d = mVertices[quad.d];

		vec3 mid_ba = (a.pos-b.pos);
		vec3 mid_bc = (c.pos-b.pos);

		mid_ba.normalize();
		mid_bc.normalize();

		a.pos = a.pos + mid_ba * offset.x + mid_bc * offset.y;
		b.pos = b.pos + mid_ba * offset.x + mid_bc * offset.y;
		c.pos = c.pos + mid_ba * offset.x + mid_bc * offset.y;
		d.pos = d.pos + mid_ba * offset.x + mid_bc * offset.y;

	}

	void deleteQuadsTaggedForDeletion()
	{
		for(int i = 0; i < mQuadsTaggedForDeletion.size(); i++)
		{
			mQuads.remove(mQuadsTaggedForDeletion[i]);
		}
		mQuadsTaggedForDeletion.clear();
	}

	const Vector<Vertex>& getVertices() const
	{
		return mVertices;
	}

	void triangulate(Vector<GLushort>& triIndecies)
	{

		triIndecies.clear();
		for(int i = 0; i < mQuads.size(); i++)
		{
			triIndecies.add(mQuads[i].a);
			triIndecies.add(mQuads[i].b);
			triIndecies.add(mQuads[i].c);
			triIndecies.add(mQuads[i].c);
			triIndecies.add(mQuads[i].d);
			triIndecies.add(mQuads[i].a);
		}
	}

	void linify(Vector<GLushort>& lineIndecies)
	{

		lineIndecies.clear();
		for(int i = 0; i < mQuads.size(); i++)
		{
			lineIndecies.add(mQuads[i].a);
			lineIndecies.add(mQuads[i].b);
			lineIndecies.add(mQuads[i].b);
			lineIndecies.add(mQuads[i].c);
			lineIndecies.add(mQuads[i].c);
			lineIndecies.add(mQuads[i].d);
			lineIndecies.add(mQuads[i].d);
			lineIndecies.add(mQuads[i].a);
		}
	}

	vec3 getSize()
	{
		vec3 min(100000.0, 100000.0, 100000.0);
		vec3 max(-100000.0, -100000.0, -100000.0);

		for(int i = 0; i < mVertices.size(); i++)
		{
			if(mVertices[i].pos.x < min.x)
				min.x = mVertices[i].pos.x;
			if(mVertices[i].pos.y < min.y)
				min.y = mVertices[i].pos.y;
			if(mVertices[i].pos.z < min.z)
				min.z = mVertices[i].pos.x;
			if(mVertices[i].pos.x > max.x)
				max.x = mVertices[i].pos.y;
			if(mVertices[i].pos.y > max.y)
				max.y = mVertices[i].pos.y;
			if(mVertices[i].pos.z > max.z)
				max.z = mVertices[i].pos.z;
		}

		return max-min;
	}

private:
	Vector<Vertex> mVertices;
	Vector<Quad> mQuads;
	Vector<int> mQuadsTaggedForDeletion;
};





HoverCraft::HoverCraft() {
	setPosition(vec3());
	setUp(vec3(0, 1, 0));
	setDirection(vec3(0, 0, 1));
}

void HoverCraft::init() {
	Modeller modeller;
	Vector<int> box = modeller.addBox(vec3(), vec3(10.0, 5.0, 10.0));

	Vector<int> front = modeller.extrude(box[Modeller::BI_FRONT], 10.0);
	modeller.scale(front[Modeller::EI_TOP], vec2(0.4, 0.7));

	front = modeller.extrude(front[Modeller::EI_TOP], 20.0);
	modeller.scale(front[Modeller::EI_TOP], vec2(0.4, 0.7));

	Vector<int> left = modeller.extrude(box[Modeller::BI_LEFT], 10.0);
	Vector<int> right = modeller.extrude(box[Modeller::BI_RIGHT], 10.0);

	modeller.scale(left[Modeller::EI_TOP], vec2(0.4, 0.7));
	modeller.scale(right[Modeller::EI_TOP], vec2(0.4, 0.7));

	left = modeller.extrude(left[Modeller::EI_TOP], 20.0);
	right = modeller.extrude(right[Modeller::EI_TOP], 20.0);

	modeller.scale(left[Modeller::EI_TOP], vec2(0.5, 0.75));
	modeller.scale(right[Modeller::EI_TOP], vec2(0.5, 0.75));

	modeller.deleteQuadsTaggedForDeletion();

	mSize = modeller.getSize();


	Vector<GLushort> indecies;
	//modeller.triangulate(indecies);
	modeller.linify(indecies);

	const Vector<Modeller::Vertex>& vertices = modeller.getVertices();

	/**/ // VBO code
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecies.size()*sizeof(GLushort), &indecies[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mNumIndecies = indecies.size();
}

vec3 HoverCraft::getSize()
{
	return mSize;
}

void HoverCraft::render() {
	glPushMatrix();

	vec3 normal = mUp;
	vec3 tangent = mDirection;
	vec3 binormal = normal.cross(tangent);

	binormal.normalize();
	float m[16];
	m[0] = normal.x;
	m[1] = normal.y;
	m[2] = normal.z;
	m[3] = 0.0;
	m[4] = binormal.x;
	m[5] = binormal.y;
	m[6] = binormal.z;
	m[7] = 0.0;
	m[8] = tangent.x;
	m[9] = tangent.y;
	m[10] = tangent.z;
	m[11] = 0.0;
	m[12] = mPosition.x;
	m[13] = mPosition.y;
	m[14] = mPosition.z;
	m[15] = 1.0;
	glMultMatrixf(m);

	glColor4x(0xffff, 0xffff, 0xffff, 0xffff);

	glScalef(0.4, 0.4, 0.4);

	glRotatef(180, 0, 1, 0);
	glRotatef(90, 0, 0, 1);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	/**/  // drawing with VBO's
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vec3), (void*)0);
	//glDrawElements(GL_TRIANGLES, mNumIndecies, GL_UNSIGNED_SHORT, (void*)0);
	glDrawElements(GL_LINES, mNumIndecies, GL_UNSIGNED_SHORT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

void HoverCraft::setPosition(const vec3& pos)
{
	mPosition = pos;
}

void HoverCraft::setDirection(const vec3& dir) {

	mDirection = dir;
	mDirection.normalize();
}

void HoverCraft::setUp(const vec3& up) {
	mUp = up;
	mUp.normalize();
}

