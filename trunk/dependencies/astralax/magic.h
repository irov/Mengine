//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#pragma warning (disable: 4068)
#pragma anon_unions

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define MAGIC_PTF

typedef int HM_FILE;
typedef int HM_EMITTER;
typedef int HM_IMPORT;
typedef int HM_OBSTACLE;
typedef int HM_WIND;

#define MAGIC_PI 3.1415926535897932384626433832795028841971693993751058209

// eng: MAGIC_POSITION - structure to define coordinates
// rus: MAGIC_POSITION - ��������� ��� �������� ���������
struct MAGIC_POSITION
{
	float x;
	float y;
	#ifdef MAGIC_3D
	float z;
	#endif
};

// eng: MAGIC_DIRECTION - structure to define direction
// rus: MAGIC_DIRECTION - ��������� ��� �������� �����������
struct MAGIC_DIRECTION
{
	#ifdef MAGIC_3D
	float x,y,z,w;
	#else
	float angle;
	#endif
};

// eng: MAGIC_FIND_DATA - structure that is used in searching emitters and directories
// rus: MAGIC_FIND_DATA - c�������� ��� �������� ����� � ��������� � ������� �����
struct MAGIC_FIND_DATA
{
	// ���������
	int type;
	const char* name;
	int animate;

	int mode;

	// �������������� ������
	void* folder;
	int index;
};
#define MAGIC_FOLDER	1
#define MAGIC_EMITTER	2

// eng: MAGIC_PARTICLE - particle structure, containing its properties
// rus: MAGIC_PARTICLE- ��������� �������, ����������� �� ��������������. 
struct MAGIC_PARTICLE
{
	MAGIC_POSITION position;
	float size;
	float size_factor;
	float angle;
	unsigned int color;
	unsigned int frame;
};

// eng: MAGIC_TEXTURE - structure, containing texture frame-file information
// rus: MAGIC_TEXTURE - ���������, �������� ���������� � ��������
struct MAGIC_TEXTURE
{
	unsigned int length;
	const char* data;
	int crc;
	const char* file;
	const char* path;

	float left,top,right,bottom;

	int frame_width;
	int frame_height;

	int texture_width;
	int texture_height;

	float pivot_x;
	float pivot_y;

	float scale;

	int source_frame_width;
	int source_frame_height;
	int optimized_frame_x;
	int optimized_frame_y;
	int optimized_frame_width;
	int optimized_frame_height;
};

// eng: MAGIC_ATLAS - structure, containing information on frame file locations within the textural atlas
// rus: MAGIC_ATLAS - ���������, �������� ���������� � ������������ ����-������ �� ���������� ������
struct MAGIC_ATLAS
{
	int width;
	int height;
	int count;
	MAGIC_TEXTURE** textures;
};

// eng: MAGIC_STATIC_ATLAS - structure, containing information of static textural atlas 
// rus: MAGIC_STATIC_ATLAS - ���������, �������� ���������� � ����������� ���������� ������
struct MAGIC_STATIC_ATLAS
{
	const char* file;
	const char* path;
	int width, height;
};

// eng: MAGIC_CHANGE_ATLAS - structure, containing information on changes in textural atlas
// rus: MAGIC_CHANGE_ATLAS - ���������, �������� ���������� �� ��������� � ���������� ������
struct MAGIC_CHANGE_ATLAS
{
	int type;
	int index;
	HM_EMITTER emitter;
	int x,y;
	int width, height;
	const char* file;
	const char* path;
	unsigned int length;
	const char* data;
};
#define MAGIC_CHANGE_ATLAS_CREATE	0
#define MAGIC_CHANGE_ATLAS_DELETE	1
#define MAGIC_CHANGE_ATLAS_LOAD		2
#define MAGIC_CHANGE_ATLAS_CLEAN	3

// eng: MAGIC_PARTICLE_VERTEXES - structure, containing particle's vertex coordinates information
// rus: MAGIC_PARTICLE_VERTEXES - ���������, �������� ���������� � ����������� ������ �������
struct MAGIC_PARTICLE_VERTEXES
{
	MAGIC_POSITION vertex1;
	float u1,v1;

	MAGIC_POSITION vertex2;
	float u2,v2;

	MAGIC_POSITION vertex3;
	float u3,v3;

	MAGIC_POSITION vertex4;
	float u4,v4;

	unsigned int color;
};

// eng: MAGIC_RENDERING - structure taking part in emitter visualization
// rus: MAGIC_RENDERING - ���������, ����������� � �������� ������������ ��������
struct MAGIC_RENDERING
{
	int count;
	unsigned int texture_id;
	bool intense;	
};

// eng: MAGIC_KEY - structure, that allows modifying keys on a Timeline
// rus: MAGIC_KEY - ���������, ����������� �������������� ����� �� ����� �������
struct MAGIC_KEY
{
	double time;

	MAGIC_POSITION position;
	MAGIC_POSITION s1,s2;
	
	float scale;

	int number;
	MAGIC_DIRECTION direction;

	float opacity;
};
#define MAGIC_KEY_POSITION	0
#define MAGIC_KEY_SCALE		1
#define MAGIC_KEY_DIRECTION	2
#define MAGIC_KEY_OPACITY	3

// eng: MAGIC_MATRIX - structure describing the matrix
// rus: MAGIC_MATRIX - ���������, ����������� �������
struct MAGIC_MATRIX
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
};

// eng: MAGIC_TRIANGLE - structure to define a triangle
// rus: MAGIC_TRIANGLE - ��������� ��� �������� ������������
struct MAGIC_TRIANGLE
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
	MAGIC_POSITION vertex3;
};

// eng: MAGIC_TAIL - structure for constructing "tail" from particles
// rus: MAGIC_TAIL - ��������� ��� ���������� "������" �� ������
struct MAGIC_TAIL
{
	int existing_particles;
	float factor;
	int count;
	float distance;
	float step;
	bool rand;
	bool single_source;
	bool direction;
	float animation_position;
	float over_life1, over_life2;
	float size1, size2;
	bool emitter_end;
};
#define MAGIC_TAIL_EXISTING_PARTICLES_TAIL			0
#define MAGIC_TAIL_EXISTING_PARTICLES_DESTROY		1
#define MAGIC_TAIL_EXISTING_PARTICLES_MOVE			2
#define MAGIC_TAIL_EXISTING_PARTICLES_NOMOVE		3
#define MAGIC_TAIL_EXISTING_PARTICLES_MOVE_DEFAULT	4

// eng: MAGIC_BBOX - structure to define Bounds Box
// rus: MAGIC_BBOX - ��������� ��� �������� Bounds Box
struct MAGIC_BBOX
{
	MAGIC_POSITION corner1;
	MAGIC_POSITION corner2;
};

// eng: MAGIC_BIRTH - structure to restrict the region of "birth" fo new particles
// rus: MAGIC_BIRTH - ��������� ��� ����������� ������� "��������" ����� ������
struct MAGIC_BIRTH
{
	int type;

	MAGIC_POSITION position;
	MAGIC_POSITION size;
	float radius;
};
#define MAGIC_BIRTH_ALWAYS			0
#define MAGIC_BIRTH_RECTANGLE		1
#define MAGIC_BIRTH_CIRCLE			2
#define MAGIC_BIRTH_PARALLELEPIPED	3
#define MAGIC_BIRTH_SPHERE			4

// eng: MAGIC_ORIENTATION - structure for storing particles type orientation
// rus: MAGIC_ORIENTATION - ��������� ��� ��������� ���������� ���� ������
struct MAGIC_ORIENTATION
{
	int orientation;
	float x,y,z;
};
#define MAGIC_ORIENTATION_X				0
#define MAGIC_ORIENTATION_Y				1
#define MAGIC_ORIENTATION_Z				2
#define MAGIC_ORIENTATION_AXIS			3
#define MAGIC_ORIENTATION_CAMERA		4
#define MAGIC_ORIENTATION_CAMERA_X		5
#define MAGIC_ORIENTATION_CAMERA_Y		6
#define MAGIC_ORIENTATION_CAMERA_Z		7
#define MAGIC_ORIENTATION_CAMERA_AXIS	8
#define MAGIC_ORIENTATION_DIRECTION		9



#define MAGIC_SUCCESS	-1
#define MAGIC_ERROR		-2
#define MAGIC_UNKNOWN	-3

#define MAGIC_NOLOOP	0
#define MAGIC_LOOP		1
#define MAGIC_FOREVER	2

#define MAGIC_COLOR_STANDARD	0
#define MAGIC_COLOR_TINT		1
#define MAGIC_COLOR_USER		2

#define MAGIC_EMITTER_POINT		0
#define MAGIC_EMITTER_LINE		1
#define MAGIC_EMITTER_CIRCLE	2
#define MAGIC_EMITTER_ELLIPSE	3
#define MAGIC_EMITTER_SQUARE	4
#define MAGIC_EMITTER_RECTANGLE	5
#define MAGIC_EMITTER_IMAGE		6
#define MAGIC_EMITTER_TEXT		7
#define MAGIC_EMITTER_MODEL		8

#define MAGIC_DIAGRAM_LIFE				0
#define MAGIC_DIAGRAM_NUMBER			1
#define MAGIC_DIAGRAM_SIZE				2
#define MAGIC_DIAGRAM_VELOCITY			3
#define MAGIC_DIAGRAM_WEIGHT			4
#define MAGIC_DIAGRAM_SPIN				5
#define MAGIC_DIAGRAM_ANGULAR_VELOCITY	6
#define MAGIC_DIAGRAM_MOTION_RAND		7
#define MAGIC_DIAGRAM_VISIBILITY		8
#define MAGIC_DIAGRAM_DIRECTION			9

#define MAGIC_NOSORT			0
#define MAGIC_SORT_MIX			1
#define MAGIC_SORT_MIX_INV		2
#define MAGIC_SORT_CAMERA_NEAR	3
#define MAGIC_SORT_CAMERA_FAR	4


// eng: Loads the ptc-file from the path specified
// rus: ��������� ptc-���� �� ���������� ����
HM_FILE Magic_OpenFile(const char* file_name);

// eng: Loads the ptc-file image from the memory
// rus: �������� ������ ptc-����� �� ������
HM_FILE Magic_OpenFileInMemory(const char* buffer);

// eng: Closes the file, opened earlier by use of Magic_OpenFile or Magic_OpenFileInMemory
// rus: ��������� ����, �������� ����� ����� Magic_OpenFile ��� Magic_OpenFileInMemory
int Magic_CloseFile(HM_FILE hmFile);

// eng: Closing all the opened files
// rus: �������� ���� �������� ������
void Magic_CloseAllFiles();

// eng: Returns the current folder path
// rus: ���������� ������ ���� � ������� �����
const char* Magic_GetCurrentFolder(HM_FILE hmFile);

// eng: Sets the new current folder
// rus: ���������� ����� ���� � ������� �����
int Magic_SetCurrentFolder(HM_FILE hmFile, const char* path);

// eng: Searches for the first folder or emitter within the current folder and returns the type of the object found
// rus: ���� ������ ����� ��� ������ ������� � ������� ����� � ���������� ��� � ��� ���������� �������
const char* Magic_FindFirst(HM_FILE hmFile,MAGIC_FIND_DATA* data,int mode);

// eng: Searches for the next folder or emitter within the current folder and returns the type of the object found
// rus: ���� ��������� ����� ��� ��������� ������� � ������� ����� � ���������� ��� � ��� ���������� �������
const char* Magic_FindNext(HM_FILE hmFile, MAGIC_FIND_DATA* data);

// eng: Returns the name of the file that was opened through the Magic_OpenFile
// rus: ���������� ��� �����, ��������� ����� Magic_OpenFile
const char* Magic_GetFileName(HM_FILE hmFile);

// eng: Returns the flag indicating that textures are stored within the file
// rus: ���������� ������� ����, ��� ���� �������� ��������
bool Magic_HasTextures(HM_FILE hmFile);

// eng: Returns the number of static textural atlases attached to specified file
// rus: ���������� ���������� ����������� ���������� �������, ������������� � ���������� �����
int Magic_GetStaticAtlasCount(HM_FILE hmFile);

// eng: Returns information on static textural atlas attached to specified file
// rus: ���������� ���������� � ����������� ���������� ������, �������������� � ���������� �����
int Magic_GetStaticAtlas(HM_FILE hmFile, int index, MAGIC_STATIC_ATLAS* atlas);

// --------------------------------------------------------------------------------

// eng: Creates the emitter object and loads its data
// rus: ������� ������� � ��������� � ���� ������
HM_EMITTER Magic_LoadEmitter(HM_FILE hmFile, const char* name);

// eng: Gets the copy of the emitter
// rus: ��������� �������
HM_EMITTER Magic_DuplicateEmitter(HM_EMITTER hmEmitter);

// eng: Unloads the emitter data and destroys it
// rus: ��������� ������ �� �������� � ���������� ���
int Magic_UnloadEmitter(HM_EMITTER hmEitter);

// eng: Processes the emitter. Creates, displaces and removes the particles
// rus: ������������ ��������� ��������: �������, ���������� � ���������� �������
bool Magic_Update(HM_EMITTER hmEmitter, double time);

// eng: Stops the emitter
// rus: ������������� ������ ��������
int Magic_Stop(HM_EMITTER hmEmitter);

// eng: Restarts the emitter from the beginning
// rus: ������������� ������� � ������� �������
int Magic_Restart(HM_EMITTER hmEmitter);

// eng: Returns the flag showing that emitter is in interrupted mode
// rus: ���������� ������� ����, ��� ������� �����������
bool Magic_IsInterrupt(HM_EMITTER hmEmitter);

// eng: Interrupts/Starts emitter work
// rus: ��������� ��� ��������� ������ ��������
int Magic_SetInterrupt(HM_EMITTER hmEmitter, bool interrupt);

// eng: Returns the Magic Particles (Dev) time increment, used for the animation
// rus: ���������� �������� � Magic Particles ���������� �������, ������������ ��� �������� ��������
double Magic_GetUpdateTime(HM_EMITTER hmEmitter);

// eng: Returns current animation position
// rus: ���������� ������� ������� ��������
double Magic_GetPosition(HM_EMITTER hmEmitter);

// eng: Sets the current animation position
// rus: ������������� ������� ������� ��������
int Magic_SetPosition(HM_EMITTER hmEmitter,double position);

// eng: Returns animation duration
// rus: ���������� ����������������� ��������
double Magic_GetDuration(HM_EMITTER hmEmitter);

// eng: Returns the left position of the visibility range
// rus: ���������� ����� ������� ��������� ���������
double Magic_GetInterval1(HM_EMITTER hmEmitter);

// eng: Sets the left position of the visibility range
// rus: ������������� ����� ������� ��������� ���������
int Magic_SetInterval1(HM_EMITTER hmEmitter,double position);

// eng: Returns the right position of the visibility range
// rus: ���������� ������ ������� ��������� ���������
double Magic_GetInterval2(HM_EMITTER hmEmitter);

// eng: Sets the right position of the visibility range
// rus: ������������� ������ ������� ��������� ���������
int Magic_SetInterval2(HM_EMITTER hmEmitter,double position);

// eng: Figures out if the current animation position is within the visibility range
// rus: ����������, �������� �� ������� ������� �������� � �������� ���������
bool Magic_InInterval(HM_EMITTER hmEmitter);

// eng: Sets the animation position at the left position of visibility range
// rus: ������������� ������� �� ������ ������� ��������� ���������
int Magic_EmitterToInterval1(HM_EMITTER hmEmitter, float speed_factor, const char* file);

// eng: Returns the flag of the animation of emitter that begins not from 0 position
// rus: ���������� ������� ����, ��� �������� �������� ���������� �� � ������
bool Magic_IsInterval1(HM_EMITTER hmEmitter);

// eng: Copying the particles array into emitter from the file
// rus: ����������� ������������ ������ � ������� �� �����. 
int Magic_LoadArrayFromFile(HM_EMITTER hmEmitter, const char* file);

// eng: Copying the particles array from the emitter into the file
// rus: ����������� ������������ ������ �������� � ����
int Magic_SaveArrayToFile(HM_EMITTER hmEmitter, const char* file);

// eng: Returns the particle positions interpolation usage flag
// rus: ���������� ������� ������ ������������ ��������
bool Magic_IsInterpolationMode(HM_EMITTER hmEmitter);

// eng: Sets/resets the particle positions interpolation usage flag
// rus: ��������/��������� ����� ������������ ��������� ������
int Magic_SetInterpolationMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns the flag of stability/randomness of the emitter behaviour
// rus: ���������� ������� ������������/����������� ��������� ��������
bool Magic_IsRandomMode(HM_EMITTER hmEmitter);

// eng: Sets the flag of stability/randomness of the emitter behaviour
// rus: ������������� ������� ������������/����������� ��������� ��������
int Magic_SetRandomMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns the emitter behaviour mode at the end of the animation
// rus: ���������� ����� ��������� �������� ����� ��������� ��������
int Magic_GetLoopMode(HM_EMITTER hmEmitter);

// eng: Sets the emitter behaviour mode at the end of the animation
// rus: ������������� ����� ��������� �������� ����� ��������� ��������
int Magic_SetLoopMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the color management mode
// rus: ���������� ����� ���������� ������ ������
int Magic_GetColorMode(HM_EMITTER hmEmitter);

// eng: Sets the color management mode
// rus: ������������� ����� ���������� ������ ������
int Magic_SetColorMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the user defined tint
// rus: ���������� ������� ������������
int Magic_GetTint(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint
// rus: ������������� ������� ������������
int Magic_SetTint(HM_EMITTER hmEmitter,int tint);

// eng: Returns the user defined tint strength
// rus: ���������� ���� ������� ������������
float Magic_GetTintStrength(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint strength
// rus: ������������� ���� ������� ������������
int Magic_SetTintStrength(HM_EMITTER hmEmitter,float tint_strength);

// eng: Returns the emitter scale
// rus: ���������� ������� ��������
float Magic_GetScale(HM_EMITTER hmEmitter);

// eng: Sets the emitter scale
// rus: ������������� ������� ��������
int Magic_SetScale(HM_EMITTER hmEmitter, float scale);

// eng: Sets the user data
// rus: ������������� ���������������� ������
int Magic_SetData(HM_EMITTER hmEmitter, int data);

// eng: Returns the user data
// rus: ���������� ���������������� ������
int Magic_GetData(HM_EMITTER hmEmitter);

// rus: ���������� ������� �������� ������
int Magic_GetBirthPlace(HM_EMITTER hmEmitter,MAGIC_BIRTH* place);

// rus: ������������� ������� �������� ������
int Magic_SetBirthPlace(HM_EMITTER hmEmitter,MAGIC_BIRTH* place);

// eng: Returns the name of the emitter
// rus: ���������� ��� ��������
const char* Magic_GetEmitterName(HM_EMITTER hmEmitter);

// eng: Returns the shape of the emitter itself or the shape of the emitter for the specified particles type
// rus: ���������� ����� �������� ��� ����� �������� ��� ���������� ���� ������
int Magic_GetEmitterType(HM_EMITTER hmEmitter,int index);

// eng: Returns the ID of emitter
// rus: ���������� ������������� ��������
unsigned int Magic_GetEmitterID(HM_EMITTER hmEmitter);

#ifdef MAGIC_3D

// eng: Returns the flag indicating that emitter emits only 3D-particles
// rus: ���������� ������� ����, ��� ������� �������� ������ 3D-�������
bool Magic_Is3d(HM_EMITTER hmEmitter);

// eng: Returns the default matrix of emitter that was set using the Magic Particles (Dev). 
// rus: ���������� ������� ����, ������� ���� ����������� ��� �������� � Magic Particles
int Magic_GetViewMatrixDefault(HM_EMITTER hmEmitter,MAGIC_MATRIX* matrix,float distance_scale);

#endif

// eng: Returns coordinates of the emitter
// rus: ���������� ���������� ��������
int Magic_GetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter
// rus: ������������� ���������� ��������
int Magic_SetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the emitter position. "Tail" from particles is formed between old and new position
// rus: ������������� ���������� ��������. ����� ������ � ����� �������� �������� ���������� "�����" �� ������
int Magic_SetEmitterPositionWithTail(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Returns the mode of the emitter coordinates
// rus: ���������� ����� ��������� ��������
bool Magic_GetEmitterPositionMode(HM_EMITTER hmEmitter);

// eng: Sets the mode of the emitter coordinates
// rus: ������������� ����� ��������� ��������
int Magic_SetEmitterPositionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Moves particles
// rus: ���������� �������
int Magic_MoveEmitterParticles(HM_EMITTER hmEmitter,MAGIC_POSITION* offset);

// eng: Returns emitter direction
// rus: ���������� ����������� ��������
int Magic_GetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Sets the direction of the emitter
// rus: ������������� ����������� ��������
int Magic_SetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Gets the emitter's direction mode
// rus: ���������� ����� �������� ��������
bool Magic_GetEmitterDirectionMode(HM_EMITTER hmEmitter);

// eng: Sets emitter's rotation mode
// rus: ������������� ����� �������� ��������
int Magic_SetEmitterDirectionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Rotates particles
// rus: ������� �������
int Magic_RotateEmitterParticles(HM_EMITTER hmEmitter, MAGIC_DIRECTION* offset);

// eng: Returns the animate folder flag
// rus: ���������� ������� ������������� �����
bool Magic_IsFolder(HM_EMITTER hmEmitter);

// eng: Returns the number of emitters contained in animate folder. 1 is returned for emitter
// rus: ���������� ���������� ��������� ������ ��������
int Magic_GetEmitterCount(HM_EMITTER hmEmitter);

// eng: Returns the specified emitter from animate folder. Returns itself for emitter
// rus: ���������� ���������� �������� ������ ��������
HM_EMITTER Magic_GetEmitter(HM_EMITTER hmEmitter,int index);

// eng: Creates the first portion of particles for emitter visualization and returns information on visualization settings. 
// rus: ������� ������ ������ ������ ��� ������������ �������� � ���������� ���������� � ���������� ������������
int Magic_CreateFirstRenderedParticlesList(HM_EMITTER hmEmitter, MAGIC_RENDERING* rendering);

// eng: Creates the next portion of particles for emitter visualization and returns information on visualization settings
// rus: ������� ��������� ������ ������ ��� ������������ �������� � ���������� ���������� � ���������� ������������
void Magic_CreateNextRenderedParticlesList(MAGIC_RENDERING* rendering);

// eng: Returns the MAGIC_PARTICLE_VERTEXES structure, containing the coordinates of the visualization rectangle vertice
// rus: ���������� ���������� � �������� ��������� ������� ��� � ������������
int Magic_GetNextParticleVertexes(MAGIC_PARTICLE_VERTEXES* vertexes);

// eng: Returns particles sorting mode
// rus: ���������� ����� ���������� ������
int Magic_GetSortingMode(HM_EMITTER hmEmitter);

// eng: Sets particles sorting mode
// rus: ������������� ����� ���������� ������
int Magic_SetSortingMode(HM_EMITTER hmEmitter, int mode);

#ifdef MAGIC_3D

// eng: Returns the view matrix that is set for API
// rus: ���������� ������� ����, ������������� ��� API
void Magic_GetViewMatrix(MAGIC_MATRIX* matrix);

// eng: Sets the view matrix for API
// rus: ������������� ������� ���� ��� API
void Magic_SetViewMatrix(MAGIC_MATRIX* matrix);

#endif

// eng: Returnes Bounds Box
// rus: ���������� BBox
int Magic_GetBBox(HM_EMITTER hmEmitter,MAGIC_BBOX* bbox);

// eng: Returns Bounds Box recalculation period
// rus: ���������� ������ ����������� Bounds Box
int Magic_GetBBoxPeriod(HM_EMITTER hmEmitter);

// eng: Sets Bounds Box recalculation period
// rus: ������������� ������ ����������� Bounds Box
int Magic_SetBBoxPeriod(HM_EMITTER hmEmitter,int period);

// eng: Forces Bounds Box recalculation
// rus: ������������� ������������� Bounds Box
int Magic_RecalcBBox(HM_EMITTER hmEmitter);

// --------------------------------------------------------------------------------

// eng: Returns the name of the particles type
// rus: ���������� ��� ���� ������
const char* Magic_GetParticlesTypeName(HM_EMITTER hmEmitter, int index);

// eng: Returns the number particles type contained in emitter
// rus: ���������� ���������� ����� ������ ������ ��������
int Magic_GetParticlesTypeCount(HM_EMITTER hmEmitter);

// eng: Locks the specified particles type for the further processing
// rus: ����������� ��� ���������� ��������� ��������� ��� ������
int Magic_LockParticlesType(HM_EMITTER hmEmitter, int index);

// eng: Releases previously locked particles type
// rus: ����������� ����������� ����� ��� ������
int Magic_UnlockParticlesType();

// eng: Returns the number of textural frame-files in locked particles type
// rus: ���������� ���������� ���������� ����-������ � ����������� ���� ������
int Magic_GetTextureCount();

// eng: Returns MAGIC_TEXTURE structure containing the specified frame-file information
// rus: ���������� ���������   � ����������� �� ��������� ����-�����
int Magic_GetTexture(int index, MAGIC_TEXTURE* texture);

// eng: Sets texture coordinates for specified frame-file
// rus: ������������� ���������� ���������� ��� ���������� ����-�����
int Magic_SetTextureUV(int index, float left, float top, float right, float bottom);

// eng: Returns the number used as a user resources texture identificator
// rus: ���������� �����, ������� ������������� ��� ������������� �������� � �������� ������������
unsigned int Magic_GetTextureID();

// eng: Sets the number used as a user resources texture identificator
// rus: ������������� �����, ������� ������������� ��� ������������� �������� � �������� ������������
int Magic_SetTextureID(unsigned int id);

// eng: Returns the Intensity flag
// rus: ���������� ������� �������������
bool Magic_IsIntensive();

#ifdef MAGIC_3D

// eng: Returns particle type orientation for 3D-emitters
// rus: ���������� ���������� ���� ������ ��� 3D-���������
int Magic_GetOrientation(MAGIC_ORIENTATION* orientation);

// eng: Sets particle type orientation for 3D-emitters
// rus: ������������� ���������� ���� ������ ��� 3D-���������
int Magic_SetOrientation(MAGIC_ORIENTATION* orientation);

#endif

// eng: Returns "tail" properties
// rus: ���������� �������� "������"
int Magic_GetTailProperties(MAGIC_TAIL* tail);

// eng: Sets "tail" properties
// rus: ������������� �������� "������"
int Magic_SetTailProperties(MAGIC_TAIL* tail);

// eng: Returns the next particle. Is used to go through all the existing particles
// rus: ���������� ���������� �� ��������� �������. ������������ ��� �������� ���� ������������ ������
int Magic_GetNextParticle(MAGIC_PARTICLE* particle);

// eng: Changes the position of the particle that is got by Magic_GetNextParticle
// rus: �������� ���������� �������, ���������� ����� Magic_GetNextParticle
void Magic_MoveParticle(MAGIC_POSITION* offset);

// eng: Rotates the particle that was obtained by Magic_GetNextParticle around the emitter
// rus: ������� ������� ���������� ����� Magic_GetNextParticle ������ ��������
void Magic_RotateParticle(MAGIC_DIRECTION* offset);

// --------------------------------------------------------------------------

// eng: Sets the mode of atlas
// rus: ������������� ����� ������ ������
void Magic_SetCleverModeForAtlas(bool clever);

// eng: Returns the mode of atlas
// rus: ���������� ����� ������ ������
bool Magic_IsCleverModeForAtlas();

// eng: Returns the information on next change in textural atlas
// rus: ���������� ���������� �� ��������� ��������� � ���������� ������
int Magic_GetNextAtlasChange(MAGIC_CHANGE_ATLAS* change);

// eng: Creates textural atlases for all loaded emitters
// rus: ������� ���������� ������ ��� ���� ����������� ���������
float Magic_CreateAtlases(int width,int height,int step,float scale_step);

// eng: Creates textural atlases for specified emitters
// rus: ������� ���������� ������ ��� ��������� ���������
float Magic_CreateAtlasesForEmitters(int width,int height,int count, HM_EMITTER* emitters,int step,float scale_step);

// eng: Sets up the initial scale for atlas creation
// rus: ������������� ��������� ������� ��� ����������� ������
void Magic_SetStartingScaleForAtlas(float scale);

// eng: Returns the initial scale for atlas creation
// rus: ���������� ��������� ������� ��� ����������� ������
float Magic_GetStartingScaleForAtlas();

// eng: Returns the number of textural atlases
// rus: ���������� ���������� �������
int Magic_GetAtlasCount();

// eng: Returns the textural atlas specified
// rus: ���������� ��������� ���������� �����
int Magic_GetAtlas(int index, MAGIC_ATLAS* atlas);

// --------------------------------------------------------------------------

// eng: Figures out if the diagram is managable
// rus: ����������, �������� �� ��������� ������
bool Magic_IsDiagramEnabled(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Returns the specified diagram factor
// rus: ���������� ��������� ��� ���������� �������
float Magic_GetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the specified diagram factor
// rus: ������������� ��������� ��� ���������� �������
int Magic_SetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index, float factor);

// eng: Returns the factor for emitter form diagram
// rus: ���������� ��������� ��� ������� ����� ��������
float Magic_GetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the factor for emitter form diagram
// rus: ������������� ��������� ��� ������� ����� ��������
int Magic_SetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line, float factor);

// eng: Returns the offset for the specified diagram
// rus: ���������� �������� ��� ���������� �������
float Magic_GetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the offset for the specified diagram
// rus: ������������� �������� ��� ���������� �������
int Magic_SetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index, float addition);

// eng: Returns the offset for emitter form diagram
// rus: ���������� �������� ��� ������� ����� ��������
float Magic_GetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the offset for emitter form diagram
// rus: ������������� �������� ��� ������� ����� ��������
int Magic_SetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line, float addition);

// eng: Allows substituting a graphical pattern which is used to generate particles of "Image" and "Text" formed emitters
// rus: ��������� �������� ����������� �����, �� �������� ���������� ��������� ������ � ��������� ���� "��������" � "�����"
int Magic_ChangeImage(HM_EMITTER hmEmitter, int type_index, int width, int height, unsigned char* data, int bytes_per_pixel);

// eng: Allows changing the triangle based model by which particle generation occurs in "Model" formed emitters
// rus: ��������� �������� ������ �� �������������, �� ������� ���������� ��������� ������ � �������� ���� "������"
int Magic_ChangeModel(HM_EMITTER hmEmitter, int type_index, int count, MAGIC_TRIANGLE* triangles);

// --------------------------------------------------------------------------------------

// eng: Creates a new key on a Timeline
// rus: ������� ����� ���� ���������� ���� �� ����� �������
int Magic_CreateKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key);

// eng: Deletes the specified key of desired type from Timeline
// rus: ������� ��������� ���� ���������� ���� �� ����� �������
int Magic_DeleteKey(HM_EMITTER hmEmitter, int type, int index);

// eng: Returns the number of keys of specified type from Timeline
// rus: ���������� ���������� ������ ���������� ���� �� ����� �������
int Magic_GetKeyCount(HM_EMITTER hmEmitter, int type);

// eng: Returns information for the key of specified type
// rus: ���������� ���������� � ����� ���������� ����
int Magic_GetKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key, int index);

// eng: Sets the new data for the key of specified type
// rus: ������������� ����� ������ ��� ����� ���������� ����
int Magic_SetKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key, int index);

// --------------------------------------------------------------------------------------

// eng: Converts UTF16 string into UTF8
// rus: ������������ ������ ���� UTF16 � ������ ���� UTF8
const unsigned char* Magic_UTF16to8(const unsigned short* str);

// eng: Converts UTF8 string into UTF16
// rus: ������������ ������ ���� UTF8 � ������ ���� UTF16
const unsigned short* Magic_UTF8to16(const unsigned char* str);

// eng: Converts UTF32 string into UTF8
// rus: ������������ ������ ���� UTF32 � ������ ���� UTF8
const unsigned char* Magic_UTF32to8(const unsigned int* str);

// eng: Converts UTF8 string into UTF32
// rus: ������������ ������ ���� UTF8 � ������ ���� UTF32
const unsigned int* Magic_UTF8to32(const unsigned char* str);

// eng: Converts UTF32 string into UTF16
// rus: ������������ ������ ���� UTF32 � ������ ���� UTF16
const unsigned short* Magic_UTF32to16(const unsigned int* str);

// eng: Converts UTF16 string into UTF32
// rus: ������������ ������ ���� UTF16 � ������ ���� UTF32
const unsigned int* Magic_UTF16to32(const unsigned short* str);

// --------------------------------------------------------------------------------------

typedef int HM_STREAM;


#define MAGIC_STREAM_READ		0
#define MAGIC_STREAM_WRITE		1
#define MAGIC_STREAM_ADD		2


// �������� ������ �� �����
// �������� file_name - ���� � ����� � ������� UTF8.
// �������� mode:
// mode=MAGIC_STREAM_READ - ����� ����������� �� ������. ������� ������ ��������������� �� ������.
// mode=MAGIC_STREAM_WRITE - ����� ����������� �� ������. ���� ���� ������������, �� �� ��������� ������, ���� ���� �����������, �� �� ���������.
// mode=MAGIC_STREAM_ADD - ����� ����������� �� ������. ������� ������ ��������������� �� ����� �����. ���� ���� ������������, �� �� ���������.
// ���������� ������������� ������ ��� 0
int Magic_StreamOpenFile(const char* file_name, int mode);

// �������� ������ �� ������
// �������� address - ����� ������ ������ ������.
// �������� length - ������ ������ ������ � ������.
// �������� mode:
// mode=MAGIC_STREAM_READ - ����� ����������� �� ������. ������� ������ ��������������� �� ������. address ������ ������ ������. length - ������ ������. ���� length=0, �� ������ ������ �� ���������, �� ������ �� ���� �����.
// mode=MAGIC_STREAM_WRITE - ����� ����������� �� ������. � ������ ��������� ������ �����. ��������� address � length �� ����� �������� (������ ���� ����� 0).
// mode=MAGIC_STREAM_ADD - ����� ����������� �� ������. ��� ���� ��������� ����� � ������� ������ address � �������� length. ��� ����� �������� ���������� ������ API, �������� ���������� �����. ������� ������ ��������������� �� �����.
// ���������� ������������� ������ ��� 0
int Magic_StreamOpenMemory(const char* address, unsigned int length, int mode);

// �������� ������
// �����, ��������� �� ����� ��������� ���� ����, �� �� ������� ���, � ����� �������� � ������ ���������� ��� ���� ������.
int Magic_StreamClose(HM_STREAM hmStream);

// �������� ���� ������������ �������
void Magic_StreamCloseAll();

// ���������� ������ ������
unsigned int Magic_StreamGetLength(HM_STREAM hmStream);

// ���������� ������� ������� � ������
unsigned int Magic_StreamGetPosition(HM_STREAM hmStream);

// ������������� ������� � ������. �������� ������ ��� ������ MAGIC_STREAM_READ
int Magic_StreamSetPosition(HM_STREAM hmStream, unsigned int position);

// ���������� � ����� ����� � ��������� ����������
int Magic_StreamWrite(HM_STREAM hmStream, const char* data, unsigned int count);

// ������ �� ������ ����� � ��������� ����������
int Magic_StreamRead(HM_STREAM hmStream, char* data, unsigned int count);

// ���������� ����� ������ ������ MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD
int Magic_StreamGetMode(HM_STREAM hmStream);

// ������������� ����� ������ ������ MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD
int Magic_StreamSetMode(HM_STREAM hmStream, int mode);

// ���������� ��� ����� ������ ��� NULL. ����� ������������, ����� �������� ����� �� ����� � ����� �� ������
const char* Magic_StreamGetFileName(HM_STREAM hmStream);

// �������� ptc-�����. ���������� �������� Magic_OpenFile/Magic_OpenFileInMemory
int Magic_OpenStream(HM_STREAM hmStream);

// ����������� ������������ ������ �������� � �����
int Magic_SaveArrayToStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// ������ ������������ ������ �������� ����� ��������� ������ �� ������
int Magic_LoadArrayFromStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// ������������� ������� �� ������ ������� ��������� ���������. ���� hmStream �� ����� 0, �� ���������� ������ � �������:
// ���� ������ ������ ����� 0, �� � ���� ����� ����������� ������ � �������� ����� Magic_SaveArrayToStream
// ���� ������ ������ �� ����� 0, �� �� ����� ������ ����������� ������ � �������� ����� Magic_LoadArrayFromStream
int Magic_EmitterToInterval1_Stream(HM_EMITTER hmEmitter, float speed_factor, HM_STREAM hmStream);

// �������� ���� ������ ����������
void Magic_DestroyAll();


// ���������� �������� � Magic Particles ����, ������������ ��� �������� ��������
float Magic_GetUpdateSpeed(HM_EMITTER hmEmitter);

struct MAGIC_RECT
{
	int left;
	int top;
	int right;
	int bottom;
};

// ���������� ��������� ������� ��������
float Magic_GetBackgroundRect(HM_EMITTER hmEmitter, MAGIC_RECT* rect);

//#define MAGIC_BEAM
#define MAGIC_ADDITIONAL_TYPE_NORMAL	0
#define MAGIC_ADDITIONAL_TYPE_TRAIL		1
#define MAGIC_ADDITIONAL_TYPE_BEAM		2

//#define MAGIC_MATERIAL


#define MAGIC_VARIABLE_BOOL		0
#define MAGIC_VARIABLE_INT		1
#define MAGIC_VARIABLE_FLOAT	2
#define MAGIC_VARIABLE_STRING	3
#define MAGIC_VARIABLE_DIAGRAM	4

struct MAGIC_VARIABLE
{
	const char* name;
	int type;
	char value[8];
};

int Magic_GetEmitterVariableCount(HM_EMITTER hmEmitter);
int Magic_GetEmitterVariable(HM_EMITTER hmEmitter, int index, MAGIC_VARIABLE* variable);

int Magic_GetParticlesTypeVariableCount();
int Magic_GetParticlesTypeVariable(int index, MAGIC_VARIABLE* variable);




typedef unsigned int HM_PARTICLE;

#define MAGIC_EVENT_CREATION	0
#define MAGIC_EVENT_DESTRUCTION	1
#define MAGIC_EVENT_EXISTENCE	2
#define MAGIC_EVENT_COLLISION	3
#define MAGIC_EVENT_MAGNET		4
#define MAGIC_EVENT_WIND		5

#define MAGIC_ACTION_EVENT				0
#define MAGIC_ACTION_DESTRUCTION		1
#define MAGIC_ACTION_DETACHING			2
#define MAGIC_ACTION_FACTOR				3
#define MAGIC_ACTION_PARTICLE			4
#define MAGIC_ACTION_MAGNET_PARTICLE	5

#define MAGIC_ACTION_DIRECTION_DISABLED	0
#define MAGIC_ACTION_DIRECTION_MOVEMENT	1
#define MAGIC_ACTION_DIRECTION_REBOUND	2

#define MAGIC_MAGNET_POINT_ANY				0
#define MAGIC_MAGNET_POINT_DIRECTION		1
#define MAGIC_MAGNET_POINT_ANTI_DIRECTION	2

struct MAGIC_ACTION
{
	int event;
	int HM;
	int action;
	float factor;

	int magnet_particles_type;
	int magnet_direction;
	int magnet_distance;
	float magnet_strength1;
	float magnet_strength2;

	int particles_type;
	int direction;
	float angle;
	float size;
	float velocity;
	float weight;
	float spin;
	float angular_velocity;
	float motion_rand;
	float visibility;
};

// ���������� ���������� ��������
int Magic_GetActionCount();
// ���������� ��������
int Magic_GetAction(int index, MAGIC_ACTION* action);
// ������� ����� ��������
int Magic_CreateAction(MAGIC_ACTION* action);
// ������� ��������� ��������
int Magic_DestroyAction(int index);

struct MAGIC_EVENT
{
	int event;
	HM_PARTICLE hmParticle;
	MAGIC_POSITION position1;
	MAGIC_POSITION position2;
	MAGIC_POSITION reflection;
};

struct MAGIC_SEGMENT
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
};

// ����� �����������
#if defined(EXE) || defined(SCREENSAVER) || (!defined(MAGIC_3D))
#define MAGIC_OBSTACLE_CIRCLE		0	// ���������� (2D)
#define MAGIC_OBSTACLE_SEGMENT		1	// ������� (2D)
#endif
#ifdef MAGIC_3D
#define MAGIC_OBSTACLE_SPHERE		2	// ����� (3D)
#define MAGIC_OBSTACLE_TRIANGLE		3	// ����������� (3D)
#endif

struct MAGIC_OBSTACLE
{
	int type;
	float radius;
	int count;
	#if defined(EXE) || defined(SCREENSAVER)
	void* primitives;
	#else
	#ifdef MAGIC_3D
	MAGIC_TRIANGLE* primitives;
	#else
	MAGIC_SEGMENT* primitives;
	#endif
	#endif
};

struct MAGIC_WIND
{
	MAGIC_POSITION direction;
	float velocity;
};

#define MAGIC_TYPE_OBSTACLE		0
#define MAGIC_TYPE_WIND			1
#define MAGIC_TYPE_MAGNET		2

// ���������� ��� ������������ ���� ������ ���������� ������������ ���. �������� ���������� ����
int Magic_GetAttachedPhysicObjectsCount(int type);
// ���������� ��� ������������ ���� ������ ��� ������������ ���. ������� ���������� ����
int Magic_GetAttachedPhysicObjects(int type, int* HMs);

// eng: Returns the coordinates of the obstacle
// rus: ���������� ���������� �����������
int Magic_GetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the obstacle
// ru: ������������� ���������� �����������
int Magic_SetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// �������� ���� ���������
void Magic_UnloadAllEmitters();

// �������� �����������
HM_OBSTACLE Magic_CreateObstacle(MAGIC_OBSTACLE* data, MAGIC_POSITION* position, int cell);

// �������� �����
HM_WIND Magic_CreateWind(MAGIC_WIND* data);

// �������� ���.������� ���������� ����
int Magic_DestroyPhysicObject(int type, int HM);
// �������� ���� ���. �������� ���������� ����
int Magic_DestroyAllPhysicObjects(int type);

// ������������ ���. ������� ���������� ����
int Magic_DuplicatePhysicObject(int type, int HM);

// ���������� ���������� � �����
int Magic_GetWindData(HM_WIND hmWind, MAGIC_WIND* data);
// ������������� ���������� � �����
int Magic_SetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// ���������� ���������� � �����������
int Magic_GetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data);
// ������������� ���������� � �����������
int Magic_SetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data, int cell);

// ���������� ��������� �������
int Magic_GetNextEvent(MAGIC_EVENT* evt);

// ������������ �������
void Magic_ParticleDetaching(HM_PARTICLE hmParticle);

// ����������� �������
void Magic_ParticleDestruction(HM_PARTICLE hmParticle);

// ��������� ���������������� ������ ��� �������
void Magic_ParticleSetData(HM_PARTICLE hmParticle, void* data);
// ���������� ���������������� ������ ��� �������
void* Magic_ParticleGetData(HM_PARTICLE hmParticle);

// ���������� ���������� ���������� �������
void Magic_ParticleGetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);
// ������������� ���������� ���������� �������
void Magic_ParticleSetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);

#define MAGIC_PROPERTY_ANGLE			0
#define MAGIC_PROPERTY_SIZE				1
#define MAGIC_PROPERTY_VELOCITY			2
#define MAGIC_PROPERTY_WEIGHT			3
#define MAGIC_PROPERTY_SPIN				4
#define MAGIC_PROPERTY_ANGULAR_VELOCITY	5
#define MAGIC_PROPERTY_MOTION_RAND		6
#define MAGIC_PROPERTY_VISIBILITY		7

// ������������� ��������� �������� �������
void Magic_ParticleSetProperty(HM_PARTICLE hmParticle, int property, float value);
// ������������� ������ ������� �������
void Magic_ParticleSetProperties(HM_PARTICLE hmParticle, int count, int* properties, float* values);
// ���������� ��������� �������� �������
float Magic_ParticleGetProperty(HM_PARTICLE hmParticle, int property);
// ���������� ������ ������� �������
void Magic_ParticleGetProperties(HM_PARTICLE hmParticle, int count, int* properties, float* values);

// ���������� ���������� ������ �������
float Magic_ParticleGetRadius(HM_PARTICLE hmParticle);

// ��������� ��������� MAGIC_ACTION ������� �� ���������
void MAGIC_ACTION_Identity(MAGIC_ACTION* action);

#ifndef MAGIC_3D
// ������������ ������� 2D-�������� ��� ������� ����� �� ������� � ���������
int Magic_CorrectEmitterPosition(HM_EMITTER hmEmitter, int scene_width, int scene_height);
#endif

#ifdef MAGIC_3D

#define MAGIC_pXpYpZ	0
#define MAGIC_pYpXpZ	1
#define MAGIC_pZpXpY	2
#define MAGIC_pXpZpY	3
#define MAGIC_pYpZpX	4
#define MAGIC_pZpYpX	5
#define MAGIC_nXpYpZ	6
#define MAGIC_pYnXpZ	7
#define MAGIC_pZnXpY	8
#define MAGIC_nXpZpY	9
#define MAGIC_pYpZnX	10
#define MAGIC_pZpYnX	11
#define MAGIC_pXnYpZ	12
#define MAGIC_nYpXpZ	13
#define MAGIC_pZpXnY	14
#define MAGIC_pXpZnY	15
#define MAGIC_nYpZpX	16
#define MAGIC_pZnYpX	17
#define MAGIC_pXpYnZ	18
#define MAGIC_pYpXnZ	19
#define MAGIC_nZpXpY	20
#define MAGIC_pXnZpY	21
#define MAGIC_pYnZpX	22
#define MAGIC_nZpYpX	23
#define MAGIC_nXnYpZ	24
#define MAGIC_nYnXpZ	25
#define MAGIC_pZnXnY	26
#define MAGIC_nXpZnY	27
#define MAGIC_nYpZnX	28
#define MAGIC_pZnYnX	29
#define MAGIC_nXpYnZ	30
#define MAGIC_pYnXnZ	31
#define MAGIC_nZnXpY	32
#define MAGIC_nXnZpY	33
#define MAGIC_pYnZnX	34
#define MAGIC_nZpYnX	35
#define MAGIC_pXnYnZ	36
#define MAGIC_nYpXnZ	37
#define MAGIC_nZpXnY	38
#define MAGIC_pXnZnY	39
#define MAGIC_nYnZpX	40
#define MAGIC_nZnYpX	41
#define MAGIC_nXnYnZ	42
#define MAGIC_nYnXnZ	43
#define MAGIC_nZnXnY	44
#define MAGIC_nXnZnY	45
#define MAGIC_nYnZnX	46
#define MAGIC_nZnYnX	47

int Magic_SetAxis(int axis_index);
int Magic_GetAxis();

#endif

#ifdef __cplusplus
}
#endif

#endif
