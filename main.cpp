#include "Header.h"

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

void render_string(void* font, const char* string)
{
	char* p = (char*) string;
	while (*p != '\0') glutBitmapCharacter(font, *p++);
}

void TimeUpdate()
{
	// dt update
	DWORD milliseconds = GetTickCount() - last_idle_time;
	dt = milliseconds / 1000.0f;

	dt /= SLOW_MOTION_RATIO;

	timeElapsed += dt;
	float maxPossible_dt = 0.1f;

	int numOfIterations = (int)(dt / maxPossible_dt) + 1;
	if (numOfIterations != 0)
		dt = dt / numOfIterations;
}

// Maya Inports
void initModels()
{
	// Load the model only if it hasn't been loaded before
	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwisee it's null
    if (!track)
	{
			// this is the call that actualy reads the OBJ and creates the model object
        track = glmReadOBJ("..\\assets\\models\\track.obj");
        if (!track) exit(0);
			// This will rescale the object to fit into the unity matrix
			// Depending on your project you might want to keep the original size and positions you had in 3DS Max or GMAX so you may have to comment this.
        // glmUnitize(fences);
			// These 2 functions calculate triangle and vertex normals from the geometry data.
			// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
			// So if you can export these directly from you modeling tool do it and comment these line
			// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
        //glmFacetNormals(pmodel1);
		//glmVertexNormals(pmodel1, 90.0);
    }
	if (!fences)
	{
        fences = glmReadOBJ("..\\assets\\models\\fences.obj");
        if (!fences) exit(0);
    }
	if (!start_sign)
	{
        start_sign = glmReadOBJ("..\\assets\\models\\start_sign.obj");
        if (!start_sign) exit(0);
    }
	if (!car)
	{
        car = glmReadOBJ("..\\assets\\models\\carrgt.obj");
        if (!car) exit(0);
    }
	if (!billboards)
	{
        billboards = glmReadOBJ("..\\assets\\models\\billboards.obj");
        if (!billboards) exit(0);
    }
}

void LoadTexture(Texture& t, const char* file){
	// first of all we call the tga file loader. It doesn't do anything special: it fills the Texture struct with information about the image (height, width, bits per pixel). You can easily replace it with a function to load bmps or jpegs.
	// The important thing is do load the image corectly in the structure you give it
	if (LoadTGA(&t, file))
	{
		// This tells opengl to create 1 texture and put it's ID in the given integer variable
		// OpenGL keeps a track of loaded textures by numbering them: the first one you load is 1, second is 2, ...and so on.
		glGenTextures(1, &t.texID);
		// Binding the texture to GL_TEXTURE_2D is like telling OpenGL that the texture with this ID is now the current 2D texture in use
		// If you draw anything the used texture will be the last binded texture
		glBindTexture(GL_TEXTURE_2D, t.texID);
		// This call will actualy load the image data into OpenGL and your video card's memory. The texture is allways loaded into the current texture
		// you have selected with the last glBindTexture call
		// It asks for the width, height, type of image (determins the format of the data you are giveing to it) and the pointer to the actual data
		glTexImage2D(GL_TEXTURE_2D, 0, t.bpp / 8, t.width, t.height, 0, t.type, GL_UNSIGNED_BYTE, t.imageData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (t.imageData)
		{
			// You can now free the image data that was allocated by LoadTGA
			// You don't want to keep a few Mb of worthless data on heap. It's worthless because OpenGL stores the image someware else after
			// you call glTexImage2D (usualy in you video card)
			free(t.imageData);
		}
	}
}

void LoadTexture()
{
	LoadTexture(grassTexture, "..\\assets\\textures\\grass.tga");
	LoadTexture(viewTexture, "..\\assets\\textures\\view.tga");
	LoadTexture(onScreenTexture, "..\\assets\\textures\\back.tga");
	LoadTexture(speedTexture, "..\\assets\\textures\\speed.tga");
	LoadTexture(fenceTexture, "..\\assets\\textures\\fence.tga");
	LoadTexture(roadTexture, "..\\assets\\textures\\asphalt.tga");
	LoadTexture(startTexture, "..\\assets\\textures\\start_sign.tga");
	LoadTexture(carT2, "..\\assets\\textures\\0001.tga");
	LoadTexture(stoneTexture, "..\\assets\\textures\\stone.tga");
	LoadTexture(billboardTexture1, "..\\assets\\textures\\sabanci_logo.tga");
	LoadTexture(billboardTexture2, "..\\assets\\textures\\logo2.tga");
	LoadTexture(mapTexture, "..\\assets\\textures\\map.tga");
}

void OnScreen()
{
	// Switching to Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, W, H, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);

	AveSpeed = (double)player->sumSpeed/(frame);

	if(AveSpeed > 130 && AveSpeed <= 155)
		position = 3;
	else if(AveSpeed > 155 && AveSpeed <= 180)
		position = 2;
	else if(AveSpeed > 180)
		position = 1;

	if(finished)
	{
		char str[128];
		glColor3f(1,1,1);
		if(position == 1)		sprintf(str, "Congratulations! Gold Medal - %7.2fm/s", AveSpeed);
		else if(position == 2)	sprintf(str, "Congratulations! Silver Medal - %7.2fm/s", AveSpeed);
		else if(position == 3)	sprintf(str, "Congratulations! Bronze Medal - %7.2fm/s", AveSpeed);
		else					sprintf(str, "Sorry. You must achieve at least 130m/s - %7.2fm/s", AveSpeed);
		glRasterPos2f(W/2-200,H/2);
		render_string(GLUT_BITMAP_HELVETICA_18, str);
	}

	char str[128];

	if(position == 1)	glColor3f(255/255.0,215/255.0,0/255.0);
	else if(position == 2)	glColor3f(192/255.0,192/255.0,192/255.0);
	else if(position == 3)	glColor3f(205/255.0,133/255.0,63/255.0);
	else	glColor3f(255.0/255,229/255.0,204/255.0);

	sprintf(str, "Ave. Speed: %7.2fm/s", AveSpeed);
	glRasterPos2f(35,48);
	render_string(GLUT_BITMAP_HELVETICA_18, str);

	sprintf(str, "Total Time: %7.2fs", (float)frame/40.0);
	glRasterPos2f(35,80);
	render_string(GLUT_BITMAP_HELVETICA_18, str);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, onScreenTexture.texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex2f(10.0, 10.0);
		glTexCoord2f(0,1); glVertex2f(260, 10.0);
		glTexCoord2f(1,1); glVertex2f(260, 110);
		glTexCoord2f(1,0); glVertex2f(10.0, 110);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
		float ang;
		player->speed >= 0 ? ang= 270*player->speed/player->TOP_SPEED : ang = -270*player->speed/player->TOP_SPEED;
		glTranslatef(110,H-110,0);
		glRotatef(ang, 0,0,1);
		glBegin(GL_TRIANGLES);
			glColor3f(0.7,0.2,0.2);
			glVertex2f(-5,-5);
			glVertex2f(-56.56, 56.56);
			glVertex2f(5,5);
		glEnd();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, speedTexture.texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBegin(GL_QUADS);
		glTexCoord2f(1,0);
		glVertex2f(210, H-10);	//Bottom-Right
		glTexCoord2f(1,1);
		glVertex2f(210, H-210);	//Top-Right
		glTexCoord2f(0,1);
		glVertex2f(10, H-210);	//Top-Left
		glTexCoord2f(0,0);
		glVertex2f(10, H-10);	//Bottom-Left
	glEnd();
	glDisable(GL_TEXTURE_2D);

	float x_map = (10724.52+(player->pos.z - 5129.22))/(30.4673) + W-362;
	float y_map = (player->pos.x+4702.32)/(28.5868) + H-210;

	glPushMatrix();
		glTranslatef(x_map, y_map, 0);
		glBegin(GL_QUADS);
			glColor3f(0.7,0.2,0.2);
			glVertex2f(-5,-5);
			glVertex2f(-5, 5);
			glVertex2f( 5, 5);
			glVertex2f( 5,-5);
		glEnd();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mapTexture.texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0,0);
		glVertex2f(W-10, H-10);		 //Bottom-Right
		glTexCoord2f(1.0,1.0);
		glVertex2f(W-10, H-210);	 //Top-Right
		glTexCoord2f(0,1.0);
		glVertex2f(W-362, H-210);	 //Top-Left
		glTexCoord2f(0,0);
		glVertex2f(W-362, H-10);	 //Bottom-Left
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	// Switching to Modelview Matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void DrawEnvironment()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grassTexture.texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();
		glBegin(GL_QUADS);	//plane
			glTexCoord2f(0,0);		glNormal3f(0,1,0);
			glVertex3f( 10000,-25,-10000);

			glTexCoord2f(0,200);	glNormal3f(0,1,0);
			glVertex3f( 10000,-25, 10000);

			glTexCoord2f(200,0);	glNormal3f(0,1,0);
			glVertex3f(-10000,-25, 10000);

			glTexCoord2f(200,200);	glNormal3f(0,1,0);
			glVertex3f(-10000,-25,-10000);
		glEnd();
	glPopMatrix();

	glColor3f(0.5,0.5,0.5);
	glmDraw(track, GLM_NONE | GLM_TEXTURE);
	glmDraw(fences, GLM_NONE | GLM_TEXTURE);
	glmDraw(start_sign, GLM_NONE | GLM_TEXTURE);
	glmDraw(billboards, GLM_NONE | GLM_TEXTURE);

	//skybox
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, viewTexture.texID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);		glNormal3f(0,0,1);
			glVertex3f( -10000,-1200, -10000);
			glTexCoord2f(1, 0);	glNormal3f(0,0,1);
			glVertex3f( 10000,-1200, -10000);
			glTexCoord2f(1,1);	glNormal3f(0,0,1);
			glVertex3f( 10000, 7000, -10000);
			glTexCoord2f(0,1);	glNormal3f(0,0,1);
			glVertex3f( -10000, 7000, -10000);
		glEnd();
		glBegin(GL_QUADS);
			glTexCoord2f(1,0);	glNormal3f(-1,0,0);
			glVertex3f( 10000,-1200, -10000);
			glTexCoord2f(0, 0);	glNormal3f(-1,0,0);
			glVertex3f( 10000,-1200, 10000);
			glTexCoord2f(0,1);	glNormal3f(-1,0,0);
			glVertex3f( 10000, 7000, 10000);
			glTexCoord2f(1,1);	glNormal3f(-1,0,0);
			glVertex3f( 10000, 7000, -10000);
		glEnd();
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);		glNormal3f(0,0,-1);
			glVertex3f( 10000,-1200, 10000);
			glTexCoord2f(1, 0);	glNormal3f(0,0,-1);
			glVertex3f( -10000,-1200, 10000);
			glTexCoord2f(1,1);	glNormal3f(0,0,-1);
			glVertex3f( -10000, 7000, 10000);
			glTexCoord2f(0,1);	glNormal3f(0,0,-1);
			glVertex3f( 10000, 7000, 10000);
		glEnd();
		glBegin(GL_QUADS);
			glTexCoord2f(1,0);	glNormal3f(1,0,0);
			glVertex3f( -10000,-1200, 10000);
			glTexCoord2f(0, 0);	glNormal3f(1,0,0);
			glVertex3f( -10000,-1200, -10000);
			glTexCoord2f(0,1);	glNormal3f(1,0,0);
			glVertex3f( -10000, 7000, -10000);
			glTexCoord2f(1,1);	glNormal3f(1,0,0);
			glVertex3f( -10000, 7000, 10000);
		glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();	// For testing the light
		glTranslatef (-250.0, 15.0, -150.0);
		/*glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);*/
		glutSolidSphere(25.0, 64, 64);
	glPopMatrix();

	glPushMatrix();	// For testing the light
		glTranslatef (+200.0, 15.0, -150.0);
		/*glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);*/
		glutSolidSphere(25.0, 64, 64);
	glPopMatrix();
}

//void AICar1Movement()
//{
//	BoundingBox *box = (BoundingBox*)malloc(sizeof(BoundingBox));
//	box->posMin = Vector3D(-25,-25,-25);
//	box->posMax = Vector3D( 25, 25, 25);
//	box = (box->Scale(0.7, 0.4, 0.7))->Translate(65,-15,0);
//	boxes[1] = box;
//
//	AIcar1->collided = false;
//
//	AIcar1->CheckStopped();
//	AIcar1->ApplyFriction();
//
//	/*if(AIcar1->pos.x > destination.x)
//		AIcar1->Left(2);
//	if(AIcar1->pos.z > destination.z)*/
//		AIcar1->Forward(0.8);
//
//	Vector3D next_move = AIcar1->NextMove(dt);
//	boxes[1] = boxes[1]->Translate(next_move.x, next_move.y, -next_move.z);
//
//	for(int i = 2; i < 1807; i++)
//	{
//		if(boxes[1]->isCollide(boxes[i]))
//		{
//			AIcar1->collided = true;
//			break;
//		}
//		else
//		{
//			AIcar1->collided = false;
//		}
//	}
//
//	// Position update
//	if(AIcar1->collided == false)
//		AIcar1->Simulate(dt);
//	else
//		AIcar1->speed = AIcar1->speed*0.4;
//}

void CarMovement()
{
	BoundingBox *box = (BoundingBox*)malloc(sizeof(BoundingBox));
	box->posMin = Vector3D(-25,-25,-25);
	box->posMax = Vector3D( 25, 25, 25);
	box = (box->Scale(0.7, 0.4, 0.7))->Translate(0,-15,0);
	boxes[0] = box;

	player->collided = false;
	bool forward = gKeys['w'];
	bool backward = gKeys['s'];
	bool right = gKeys['d'];
	bool left = gKeys['a'];

	player->CheckStopped();
	player->ApplyFriction();

	// Controls
	if(forward)
	{
		player->Forward(1.5);
	}
	if(backward)
	{
		player->Backward(3.5);
	}
	if(right)
	{
		if(backward)
			player->Right(4-(3/player->TOP_SPEED * player->speed));
		else
			if(player->speed > 1.0)
				player->Right(2.8-(0.8/player->TOP_SPEED * player->speed));
	}
	if(left)
	{
		if(backward)
			player->Left(4-(3/player->TOP_SPEED * player->speed));
		else
			if(player->speed > 1.0)
				player->Left(2.8-(0.8/player->TOP_SPEED * player->speed));
	}

	Vector3D next_move = player->NextMove(dt);
	boxes[0] = boxes[0]->Translate(next_move.x, next_move.y, -next_move.z);

	for(int i = 1; i < 1806; i++)
	{
		if(boxes[0]->isCollide(boxes[i]))
		{
			player->collided = true;
			break;
		}
		else
		{
			player->collided = false;
		}
	}

	// Position update
	if(player->collided == false)
		player->Simulate(dt);
	else
		player->speed = player->speed*0.4;


	player->UpdateSumSpeed();

	camera_pos = Vector3D(0,33,60);
	camera_pos = (camera_pos.rotateY(player->angle * PI_ON_180)).translate(player->pos.x, player->pos.y, -player->pos.z);
}

// For testing bounding boxes
void drawbox(BoundingBox *b)
{
	glColor3f(1,1,1);
	glBegin (GL_LINE_LOOP);
	glVertex3f(b->posMax.x,b->posMax.y,b->posMin.z); // 0
	glVertex3f(b->posMin.x,b->posMax.y,b->posMin.z); // 1
	glVertex3f(b->posMin.x,b->posMin.y,b->posMin.z); // 2
	glVertex3f(b->posMax.x,b->posMin.y,b->posMin.z); // 3
	glEnd();

	glBegin (GL_LINE_LOOP);
	glVertex3f(b->posMax.x,b->posMin.y,b->posMax.z); // 4
	glVertex3f(b->posMax.x,b->posMax.y,b->posMax.z); // 5
	glVertex3f(b->posMin.x,b->posMax.y,b->posMax.z); // 6
	glVertex3f(b->posMin.x,b->posMin.y,b->posMax.z); // 7
	glEnd();

	glBegin (GL_LINE_LOOP);
	glVertex3f(b->posMax.x,b->posMax.y,b->posMin.z); // 0
	glVertex3f(b->posMax.x,b->posMax.y,b->posMax.z); // 5
	glVertex3f(b->posMin.x,b->posMax.y,b->posMax.z); // 6
	glVertex3f(b->posMin.x,b->posMax.y,b->posMin.z); // 1
	glEnd();

	glBegin (GL_LINE_LOOP);
	glVertex3f(b->posMax.x,b->posMin.y,b->posMax.z); // 4
	glVertex3f(b->posMin.x,b->posMin.y,b->posMax.z); // 7
	glVertex3f(b->posMin.x,b->posMin.y,b->posMin.z); // 2
	glVertex3f(b->posMax.x,b->posMin.y,b->posMin.z); // 3
	glEnd();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(BirdsEye)
		gluLookAt(player->pos.x,1000,-player->pos.z, player->pos.x,0, -player->pos.z, 0,0,-1);	//Top Camera
	else
		gluLookAt(camera_pos.x, camera_pos.y-25, camera_pos.z,
		player->pos.x, 0, -player->pos.z,
		0,1,0);

	//Fixes Light Problem
	//////////////////////
	GLfloat light_position [] = { 1000.0, 200.0, 500, 0.0 };
	GLfloat light_position1[] = { -1000, 200, -500,   0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position );
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	//////////////////////

	TimeUpdate();

	DrawEnvironment();

	if(!finished)
	{
		frame++;
	}

	if(gKeys['r'])
		player->ResetCar();

	// Player Car
	glPushMatrix();
		CarMovement();
		glTranslatef(player->pos.x, player->pos.y-12.5, -player->pos.z);
		glRotatef(-player->angle, 0,1,0);
		glScalef(.4f, .4f, .4f);
		glmDraw(car, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();

	if(drawBBoxes)
		for(int i = 0; i < 1806; i++) // HARDCODED NUMBER!!
			drawbox(boxes[i]);

	if(frame > 800) // Make game finishable
		if(boxes[0]->isCollide(finishBox))
			finished = true;

	OnScreen();
	fixFPS();

	glutSwapBuffers();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch(key)
	{
		case 'b':
			drawBBoxes = !drawBBoxes;
			break;
		case 't':
			BirdsEye = !BirdsEye;
			break;
		case 27:
			exit(0);
	}
	glutPostRedisplay();
}
void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;
	glutPostRedisplay();
}

void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat aspectratio;

	if(h == 0)
		h=1;

	glViewport(0,0,w,h);
	aspectratio = (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspectratio, 1, 30000);
}

void init()
{
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	glClearColor(0.9, 0.9, 1, 0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);

	LoadTexture();
	initModels();

	finishBox = new BoundingBox(Vector3D(200,50,10), Vector3D(-200,-50,-10));
	boxes[1807] = finishBox;
	DefineCollisionBoxes(fences, "Fence");
	DefineCollisionBoxes(fences, "pCube");
}

int main()
{
	cout << "DEBUG: STARTED" << endl;

	int argc = 1;
	char *argv[1] = {(char*)"Racing Game"};
	glutInit(&argc, argv);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(10,10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("OpenGL Assignment");

	init();

	glutKeyboardFunc(KeyboardCallback);
	glutKeyboardUpFunc(KeyboardUpCallback);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	last_idle_time = GetTickCount();

	glutMainLoop();
	return 0;
}
