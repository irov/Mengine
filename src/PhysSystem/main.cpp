#include "vld.h"
#include <windows.h>
#include <string.h>
#include <windows.h>
#include <gl\gl.h>
#include "glut.h"
#include "PhysSystem.h"
#include "Joint.h"

PhysEngine world;
class MyBody
{
public:
	MyBody()
	{
		MyAngle = 0;
	}
	float MyAngle;
};

MyBody BODYA;

// Some global variables used to measure the time
float  timeAtGameStart;
UINT64 ticksPerSecond;

// Called every time you need the current game time
float GetGameTime()
{
	UINT64 ticks;
	float time;

	// This is the number of clock ticks since start
	if( !QueryPerformanceCounter((LARGE_INTEGER *)&ticks) )
		ticks = (UINT64)timeGetTime();

	// Divide by frequency to get the time in seconds
	time = (float)(__int64)ticks/(float)(__int64)ticksPerSecond;

	// Subtract the time at game start to get
	// the time since the game started
	time -= timeAtGameStart;

	return time;
}

// Called once at the start of the game
void InitGameTime()
{
	// We need to know how often the clock is updated
	if( !QueryPerformanceFrequency((LARGE_INTEGER *)&ticksPerSecond) )
		ticksPerSecond = 1000;

	// If timeAtGameStart is 0 then we get the time since
	// the start of the computer when we call GetGameTime()
	timeAtGameStart = 0;
	timeAtGameStart = GetGameTime();
}


// Global variables for measuring fps
float lastUpdate        = 0;
float fpsUpdateInterval = 0.5f;
unsigned int  numFrames         = 0;
float fps               = 0;

// Called once for every frame
void UpdateFPS()
{
	numFrames++;
	float currentUpdate = GetGameTime();
	if( currentUpdate - lastUpdate > fpsUpdateInterval )
	{
		fps = numFrames / (currentUpdate - lastUpdate);
		lastUpdate = currentUpdate;
		numFrames = 0;
	}
}

float lastGameTime = 0;

float GetDeltaTime()
{
	float dt = GetGameTime() - lastGameTime;
	lastGameTime = GetGameTime();
	return dt;
}

float accumulator = 0.0f;
float dt = 1/60.0f;

float time()
{
	static __int64 start = 0;
	static __int64 frequency = 0;

	if (start==0)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		return 0.0f;
	}

	__int64 counter = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
	return (float) ((counter - start) / double(frequency));
}

float currentTime = 0.0f;
//float accumulator = 0.0f;

void SimulationLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, -7.0f, -25.0f);

	///float deltaTime = GetGameTime() - lastGameTime;
	//lastGameTime = GetGameTime();

	//accumulator += deltaTime;

	const float newTime = time();
	float deltaTime = newTime - currentTime;
	currentTime = newTime;

	if (deltaTime>0.25f)
		deltaTime = 0.05f;

	accumulator += deltaTime;

////	while (accumulator>=dt)
	//{
	//	i++;
	//	accumulator -= dt;
		world.integrate(1/120.0f);
//	}

//	printf(" i = %d \n",i);
//	printf(" d = %f \n",deltaTime);
	//printf(" f = %f \n",BODYA.MyAngle);


	//int i = 0;
	//while(accumulator>=dt)
	//{
	//	i++;
	//	world.step(deltaTime);
	//	accumulator -= dt;
//	}
//
//	

	for (int j = 0; j < world.numBodies; ++j)
	{
		if	(world.bodies[j]->getShape()->GetShapeType() == BOX)
		{
			BoxShape* cs = (BoxShape*)world.bodies[j]->getShape();
			mt::vec2f h = cs->width * 0.5f;

			if(world.bodies[j]->isDead()) glColor3f(1,1,1);
			else glColor3f(0.5,1,0.5);

			float c = cosf(world.bodies[j]->rot), s = sinf(world.bodies[j]->rot);

			mt::vec2f v1 = world.bodies[j]->x + mt::vec2f(s * h.y - c * h.x, -s * h.x - c * h.y);
			mt::vec2f v2 = world.bodies[j]->x + mt::vec2f(c * h.x + s * h.y, s * h.x - c * h.y);
			mt::vec2f v3 = world.bodies[j]->x + mt::vec2f(c * h.x - s * h.y, s * h.x + c * h.y);
			mt::vec2f v4 = world.bodies[j]->x + mt::vec2f(-c * h.x - s * h.y, c * h.y - s * h.x);

			glBegin(GL_LINE_LOOP);
			glVertex2f(v1.x, v1.y);
			glVertex2f(v2.x, v2.y);
			glVertex2f(v3.x, v3.y);
			glVertex2f(v4.x, v4.y);
			glEnd();
		}
		else
		{
			CircleShape* cs = (CircleShape*)world.bodies[j]->getShape();
			const float twoPI = 2.0f * 3.14159f;
			float xp, yp;
			glBegin(GL_LINE_LOOP);
			for (float i=0;i<twoPI;i+=twoPI/16.0f)
			{
				xp = world.bodies[j]->x.x + cos(i) * cs->radius;
				yp = world.bodies[j]->x.y + sin(i) * cs->radius;
				glVertex2f(xp, yp);
			}
			glEnd();
		}
	}

	for (int i = 0; i < world.numJoints; ++i)
	{
		//#ifdef	_DEBUG
		ElasticJoint* j = (ElasticJoint*)world.joints[i];
		RigidBodyInterface* b1 = j->body1;
		RigidBodyInterface* b2 = j->body2;

		float c = cosf(b1->rot), s = sinf(b1->rot);
		mt::vec2f locAnchor;
		locAnchor.x = c * j->localAnchor1.x + -s * j->localAnchor1.y;
		locAnchor.y = s * j->localAnchor1.x + c * j->localAnchor1.y;

		mt::vec2f x1 = b1->x;
		mt::vec2f p1 = x1 + locAnchor;

		float c1 = cosf(b2->rot), s1 = sinf(b2->rot);
		mt::vec2f locAnchor2;
		locAnchor2.x = c1 * j->localAnchor2.x + -s1 * j->localAnchor2.y;
		locAnchor2.y = s1 * j->localAnchor2.x + c1 * j->localAnchor2.y;

		mt::vec2f x2 = b2->x;
		mt::vec2f p2 = x2 + locAnchor2;

		glColor3f(0.5f, 0.5f, 0.8f);
		glBegin(GL_LINES);
		glVertex2f(x1.x, x1.y);
		glVertex2f(p1.x, p1.y);
		glVertex2f(x2.x, x2.y);
		glVertex2f(p2.x, p2.y);
		glEnd();
	}
	
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	RigidBody * b;
	switch (key)
	{
	case 27:
		exit(0);
		break;

	case ' ':
		/*b = new RigidBody(new BoxShape(mt::vec2f(4.0f, 0.3)),30,world.numBodies);
		b->x=mt::vec2f((x/1280.0f)*20.0f-10.0f, -((y/1024.0f)*20.0f-10.0f)+7);
		world.addRigidBody(b);
		*/
		break;
	case 'x':
		/*b = new RigidBody(new CircleShape(1),20,world.numBodies);
		b->x=mt::vec2f((x/1280.0f)*20.0f-10.0f, -((y/1024.0f)*20.0f-10.0f)+7);
		world.addRigidBody(b);*/
		break;
	}
}

void Reshape(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)width/(float)height, 0.1, 100.0);
}

void MyCallback(RigidBodyInterface* rb)
{
	if(MyBody* protya = ((MyBody*)rb->getUserData()))
	{
		protya->MyAngle = rb->rot;
	}
}

void main(int argc, char** argv)
{
	PhysParamSimDesc desc;
	desc.dtvalue = 1.0f/120.0f;
	desc.fpstarget = 60;
	desc.g = jupiter_g;
	desc.iterval = 25;

	world.loadParams(desc);
	world.setUserDataCallback(MyCallback);

	//world.setNumInterations(20);
	//world.setTimeCoherensy(60.0f);
	RigidBodyInterface* b;
//	b = new RigidBody(new CircleShape(1), 1,world.numBodies);
//	b->x=mt::vec2f(0.0f, 10);
//	world.add(b);
	//b = new RigidBody(new BoxShape(mt::vec2f(100.0f, 20.0f)), FLT_MAX,world.numBodies);
	//b->x=mt::vec2f(0.0f, -0.5f * 20);

	CollisionShape* shape1 = world.createBoxShape(mt::vec2f(100, 20.0f));
	world.addRigidBody(shape1,mt::vec2f(0.0f, -10),0,FLT_MAX,0,0);
	CollisionShape* shape2 = world.createBoxShape(mt::vec2f(12.0f, 0.5f));
	world.addRigidBody(shape2,mt::vec2f(-1.5f, 10.0f),0,FLT_MAX,0,0);

	CollisionShape* shape3 = world.createBoxShape(mt::vec2f(5, 0.5f));
	for (int i = 0; i < 10; ++i)
	{
		world.addRigidBody(shape3,mt::vec2f(-6.0f + 1.0f * i, 11.125f),0,10.0f,10.0f,0);
	}

	CollisionShape* shape4 = world.createCircleShape(1);
	RigidBodyInterface* b1 = world.addRigidBody(shape4,mt::vec2f(2,15),0,10.0f,10.0f,0);

	CollisionShape* shape5 = world.createBoxShape(mt::vec2f(2, 0.5f));
	RigidBodyInterface* b2 = world.addRigidBody(shape5,mt::vec2f(2,20),0,10.0f,10.0f,0);

	world.addJoint(b1,b2,mt::vec2f(0, 0));

	//j = new ElasticJoint(world.bodies[0], world.bodies[14], mt::vec2f(-2.0f, 1.0f));
	//world.addJoint(j);

	/*
	b = new RigidBody(new BoxShape(mt::vec2f(14.0f, 0.5f)), FLT_MAX,world.numBodies);
	b->x=mt::vec2f(1.0f, 6.0f);
	b->rot = 0.3f;

	world.addRigidBody(b);

	b = new RigidBody(new BoxShape(mt::vec2f(0.5f, 3.0f)), FLT_MAX,world.numBodies);
	b->x=mt::vec2f(-7.0f, 4.0f);
	world.addRigidBody(b);

	b = new RigidBody(new BoxShape(mt::vec2f(12.0f, 0.25f)), 20.0f,world.numBodies,&BODYA);
	b->x=mt::vec2f(-0.9f, 1.0f);
	

	world.addRigidBody(b);
JointInterface* j;
	j = new ElasticJoint(world.bodies[0], world.bodies[14], mt::vec2f(-2.0f, 1.0f));
	world.addJoint(j);

	b = new RigidBody(new BoxShape(mt::vec2f(0.5f, 0.5f)), 10.0f,world.numBodies);
	b->x=mt::vec2f(-10.0f, 15.0f);
	world.addRigidBody(b);

	j = new ElasticJoint(world.bodies[13], world.bodies[15], mt::vec2f(-7.0f, 15.0f));
	world.addJoint(j);

	b = new RigidBody(new BoxShape(mt::vec2f(2.0f, 2.0f)), 20.0f,world.numBodies);
	b->x=mt::vec2f(6.0f, 2.5f);
	b->nu = 0.1f;
	world.addRigidBody(b);


	j = new ElasticJoint(world.bodies[0], world.bodies[16], mt::vec2f(6.0f, 2.6f));
	world.addJoint(j);

	b = new RigidBody(new BoxShape(mt::vec2f(2.0f, 0.2f)), 10,world.numBodies);
	b->x=mt::vec2f(6.0f, 3.6f);
	world.addRigidBody(b);

	j = new ElasticJoint(world.bodies[16], world.bodies[17], mt::vec2f(7.0f, 3.5f));
	world.addJoint(j);
	*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1280, 1024);
	glutCreateWindow("phys world");

	glutReshapeFunc(Reshape);
	glutDisplayFunc(SimulationLoop);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(SimulationLoop);

	glutMainLoop();

	world.removeBoxShape(shape1);
	world.removeBoxShape(shape2);
	world.removeBoxShape(shape3);
	world.removeBoxShape(shape5);
	world.removeCircleShape(shape4);

	//delete BODYA;
}
