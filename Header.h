#include "Vector3D.h"
#include "Car.h"

#include "GLM\stdafx.h"
#include "GLM\glm.h"
#include "GLM\Texture.h"

#include <vector>
#include <stdlib.h>
#include <Windows.h>
#include "glut.h"
using namespace std;

// Finishing Game
bool finished = false;
double AveSpeed = 0.0f;
int position = 0;

// Screen Dimensions
GLuint W = 1280;
GLuint H = 720;

// Key Controls
bool gKeys[256];
bool drawBBoxes = false;
bool BirdsEye = false;

// Models
GLMmodel* track = NULL;
GLMmodel* fences = NULL;
GLMmodel* car = NULL;
GLMmodel* start_sign = NULL;
GLMmodel* billboards = NULL;

// Textures
Texture grassTexture;
Texture viewTexture;
Texture fenceTexture;
Texture roadTexture;
Texture startTexture;
Texture stoneTexture;
Texture billboardTexture1;
Texture billboardTexture2;

Texture carT1;
Texture carT2;

Texture onScreenTexture;
Texture speedTexture;
Texture mapTexture;

// Camera Initial Position
Vector3D camera_pos(0,33,60);

// Car
Car* player = new Car(240, Vector3D(0,0,0));

// Material Properties
GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 0.1, 0.5, 0.2, 1.0 };
GLfloat mat_diffuse2[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mat_diffuse3[] = { 0, 1, 1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 5.0 };
GLfloat high_shininess[] = { 100.0 };
GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

// Timing 
const float SLOW_MOTION_RATIO = 1;
DWORD last_idle_time;
float timeElapsed = 0;
float dt;
unsigned int frame = 0;

// Fixes FPS to 40
void fixFPS()	{
	const int FRAMES_PER_SECOND = 40;
	const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	DWORD next_game_tick = GetTickCount();
	int sleep_time = 0;
	next_game_tick += SKIP_TICKS;
	sleep_time = next_game_tick - GetTickCount();
	if( sleep_time >= 0 ) {
		Sleep( sleep_time );
	}
}

struct BoundingBox
{
	Vector3D posMin;
	Vector3D posMax;

	BoundingBox()	{}
	BoundingBox(const Vector3D& _min, const Vector3D& _max)
		:posMin(_min), posMax(_max)	{}

	BoundingBox* RotateY(float angle)
	{
		return new BoundingBox(posMin.rotateY(angle), posMax.rotateY(angle));
	}
	BoundingBox* Translate(const float& _x, const float& _y, const float& _z) const
	{
		return new BoundingBox(posMin.translate(_x,_y,_z), posMax.translate(_x,_y,_z));
	}
	BoundingBox* Scale(const float& _x, const float& _y, const float& _z) const
	{
		return new BoundingBox(posMin.scale(_x,_y,_z), posMax.scale(_x,_y,_z));
	}
	bool isCollide(BoundingBox* b)
	{
		if( b->posMin.x >= posMin.x && b->posMax.x <= posMax.x &&
            b->posMin.y >= posMin.y && b->posMax.y <= posMax.y &&
            b->posMin.z >= posMin.z && b->posMax.z <= posMax.z )
        {
                return true;
        }
        
        if( posMax.x < b->posMin.x || posMin.x > b->posMax.x )
                return false;
        if( posMax.y < b->posMin.y || posMin.y > b->posMax.y )
                return false;
        if( posMax.z < b->posMin.z || posMin.z > b->posMax.z )
                return false;
                
        return true;
	}
};

// Finishing Condition
BoundingBox* finishBox;

//Array for Collisions
BoundingBox *boxes[2000];
int boxindex = 1;

void initbox(BoundingBox *b)
{
	b->posMin.x = 100000;
	b->posMin.y = 100000;
	b->posMin.z = 100000;

	b->posMax.x = -100000;
	b->posMax.y = -100000;
	b->posMax.z = -100000;
}

void AddCollisionBox(GLMmodel *model, GLMgroup * object)
{
	BoundingBox *box;
	// GLM doesn't store each vertex together with the object that owns it. It doesn't have that notion. In GLM object don't have vertex, they have triangles. And each triangle is actually an index in the triangle list of the object.
	for(unsigned int i = 0; i < object->numtriangles; i++) 
	{ 
		box = (BoundingBox*)malloc(sizeof(BoundingBox)); 
		initbox(box);
		// for each vertex of the triangle pmodel1->triangles[object->triangles[i]]
		// calculate min and max
		for (int j=0;j<3;j++)
		{
			GLuint index = model->triangles[object->triangles[i]].vindices[j];
			GLfloat x = model->vertices[index*3 + 0];
			GLfloat y = model->vertices[index*3 + 1];
			GLfloat z = model->vertices[index*3 + 2];

			if (box->posMin.x>x) box->posMin.x = x;
			if (box->posMin.y>y) box->posMin.y = y;
			if (box->posMin.z>z) box->posMin.z = z;

			if (box->posMax.x<x) box->posMax.x = x;
			if (box->posMax.y<y) box->posMax.y = y;
			if (box->posMax.z<z) box->posMax.z = z;
		} 
		boxes[boxindex] = box;
		boxindex++;
	}
}

void DefineCollisionBoxes(GLMmodel *model, const char *part_of_the_name)
{
	GLMgroup *group = model->groups;
	while(group) 
	{ 
		if(strstr(group->name,part_of_the_name))
			AddCollisionBox(model,group);  
		group = group->next;
	}
}