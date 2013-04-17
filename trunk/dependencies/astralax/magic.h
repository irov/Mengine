//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define MAGIC_PTF

typedef int HM_FILE;
typedef int HM_EMITTER;
typedef int HM_IMPORT;

#define MAGIC_PI 3.1415926535897932384626433832795028841971693993751058209

// eng: MAGIC_POSITION - structure to define coordinates
// rus: MAGIC_POSITION - структура для хранения координат
struct MAGIC_POSITION
{
	float x;
	float y;
	#ifdef MAGIC_3D
	float z;
	#endif
};

// eng: MAGIC_DIRECTION - structure to define direction
// rus: MAGIC_DIRECTION - структура для хранения направления
struct MAGIC_DIRECTION
{
	#ifdef MAGIC_3D
	float x,y,z,w;
	#else
	float angle;
	#endif
};

// eng: MAGIC_FIND_DATA - structure that is used in searching emitters and directories
// rus: MAGIC_FIND_DATA - cтруктура для перебора папок и эмиттеров в текущей папке
struct MAGIC_FIND_DATA
{
	// результат
	int type;
	const char* name;
	int animate;

	int mode;

	// дополнительные данные
	void* folder;
	int index;
};
#define MAGIC_FOLDER	1
#define MAGIC_EMITTER	2

// eng: MAGIC_PARTICLE - particle structure, containing its properties
// rus: MAGIC_PARTICLE- структура частицы, описывающая ее характеристики. 
struct MAGIC_PARTICLE
{
	MAGIC_POSITION position;
	float size;
	float size_factor;
	float angle;
	unsigned long color;
	unsigned int frame;
};

// eng: MAGIC_TEXTURE - structure, containing texture frame-file information
// rus: MAGIC_TEXTURE - структура, хранящая информацию о текстуре
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
// rus: MAGIC_ATLAS - структура, хранящая информацию о расположении файл-кадров на текстурном атласе
struct MAGIC_ATLAS
{
	int width;
	int height;
	int count;
	MAGIC_TEXTURE** textures;
};

// eng: MAGIC_STATIC_ATLAS - structure, containing information of static textural atlas 
// rus: MAGIC_STATIC_ATLAS - структура, хранящая информацию о статическом текстурном атласе
struct MAGIC_STATIC_ATLAS
{
	const char* file;
	const char* path;
	int width, height;
};

// eng: MAGIC_CHANGE_ATLAS - structure, containing information on changes in textural atlas
// rus: MAGIC_CHANGE_ATLAS - структура, хранящая информацию об изменении в текстурном атласе
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
// rus: MAGIC_PARTICLE_VERTEXES - структура, хранящая информацию о координатах вершин частицы
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

	unsigned long color;
};

// eng: MAGIC_RENDERING - structure taking part in emitter visualization
// rus: MAGIC_RENDERING - структура, участвующая в процессе визуализации эмиттера
struct MAGIC_RENDERING
{
	int count;
	unsigned int texture_id;
	bool intense;	
};

// eng: MAGIC_KEY - structure, that allows modifying keys on a Timeline
// rus: MAGIC_KEY - структура, позволяющая модифицировать ключи на Шкале времени
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
// rus: MAGIC_MATRIX - структура, описывающая матрицу
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
// rus: MAGIC_TRIANGLE - структура для хранения треугольника
struct MAGIC_TRIANGLE
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
	MAGIC_POSITION vertex3;
};

// eng: MAGIC_TAIL - structure for constructing "tail" from particles
// rus: MAGIC_TAIL - структура для построения "хвоста" из частиц
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
// rus: MAGIC_BBOX - структура для хранения Bounds Box
struct MAGIC_BBOX
{
	MAGIC_POSITION corner1;
	MAGIC_POSITION corner2;
};

// eng: MAGIC_BIRTH - structure to restrict the region of "birth" fo new particles
// rus: MAGIC_BIRTH - структура для ограничения области "рождения" новых частиц
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
// rus: MAGIC_ORIENTATION - структура для храниения ориентации типа частиц
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
// rus: Загружает ptc-файл по указанному пути
HM_FILE Magic_OpenFile(const char* file_name);

// eng: Loads the ptc-file image from the memory
// rus: Открытие образа ptc-файла из памяти
HM_FILE Magic_OpenFileInMemory(const char* buffer);

// eng: Closes the file, opened earlier by use of Magic_OpenFile or Magic_OpenFileInMemory
// rus: Закрывает файл, открытый ранее через Magic_OpenFile или Magic_OpenFileInMemory
int Magic_CloseFile(HM_FILE hmFile);

// eng: Closing all the opened files
// rus: Закрытие всех открытых файлов
void Magic_CloseAllFiles();

// eng: Returns the current folder path
// rus: Возвращает полный путь к текущей папке
const char* Magic_GetCurrentFolder(HM_FILE hmFile);

// eng: Sets the new current folder
// rus: Установить новый путь к текущей папке
int Magic_SetCurrentFolder(HM_FILE hmFile, const char* path);

// eng: Searches for the first folder or emitter within the current folder and returns the type of the object found
// rus: Ищет первую папку или первый эмиттер в текущей папке и возвращает имя и тип найденного объекта
const char* Magic_FindFirst(HM_FILE hmFile,MAGIC_FIND_DATA* data,int mode);

// eng: Searches for the next folder or emitter within the current folder and returns the type of the object found
// rus: Ищет очередную папку или очередной эмиттер в текущей папке и возвращает имя и тип найденного объекта
const char* Magic_FindNext(HM_FILE hmFile, MAGIC_FIND_DATA* data);

// eng: Returns the name of the file that was opened through the Magic_OpenFile
// rus: Возвращает имя файла, открытого через Magic_OpenFile
const char* Magic_GetFileName(HM_FILE hmFile);

// eng: Returns the flag indicating that textures are stored within the file
// rus: Возвращает признак того, что файл содержит текстуры
bool Magic_HasTextures(HM_FILE hmFile);

// eng: Returns the number of static textural atlases attached to specified file
// rus: Возвращает количество статических текстурных атласов, прикрепленных к указанному файлу
int Magic_GetStaticAtlasCount(HM_FILE hmFile);

// eng: Returns information on static textural atlas attached to specified file
// rus: Возвращает информацию о статическом текстурном атласе, прикрепленному к указанному файлу
int Magic_GetStaticAtlas(HM_FILE hmFile, int index, MAGIC_STATIC_ATLAS* atlas);

// --------------------------------------------------------------------------------

// eng: Creates the emitter object and loads its data
// rus: Создает эмиттер и загружает в него данные
HM_EMITTER Magic_LoadEmitter(HM_FILE hmFile, const char* name);

// eng: Gets the copy of the emitter
// rus: Дублирует эмиттер
HM_EMITTER Magic_DuplicateEmitter(HM_EMITTER hmEmitter);

// eng: Unloads the emitter data and destroys it
// rus: Выгружает данные из эмиттера и уничтожает его
int Magic_UnloadEmitter(HM_EMITTER hmEitter);

// eng: Processes the emitter. Creates, displaces and removes the particles
// rus: Осуществляет обработку эмиттера: создает, перемещает и уничтожает частицы
bool Magic_Update(HM_EMITTER hmEmitter, double time);

// eng: Stops the emitter
// rus: Останавливает работу эмиттера
int Magic_Stop(HM_EMITTER hmEmitter);

// eng: Restarts the emitter from the beginning
// rus: Перезапускает эмиттер с нулевой позиции
int Magic_Restart(HM_EMITTER hmEmitter);

// eng: Returns the flag showing that emitter is in interrupted mode
// rus: Возврашает признак того, что эмиттер прерывается
bool Magic_IsInterrupt(HM_EMITTER hmEmitter);

// eng: Interrupts/Starts emitter work
// rus: Прерывает или запускает работу эмиттера
int Magic_SetInterrupt(HM_EMITTER hmEmitter, bool interrupt);

// eng: Returns the Magic Particles (Dev) time increment, used for the animation
// rus: Возвращает заданное в Magic Particles приращение времени, используемое для анимации эмиттера
double Magic_GetUpdateTime(HM_EMITTER hmEmitter);

// eng: Returns current animation position
// rus: Возвращает текущую позицию анимации
double Magic_GetPosition(HM_EMITTER hmEmitter);

// eng: Sets the current animation position
// rus: Устанавливает текущую позицию анимации
int Magic_SetPosition(HM_EMITTER hmEmitter,double position);

// eng: Returns animation duration
// rus: Возвращает продолжительность анимации
double Magic_GetDuration(HM_EMITTER hmEmitter);

// eng: Returns the left position of the visibility range
// rus: Возвращает левую позицию интервала видимости
double Magic_GetInterval1(HM_EMITTER hmEmitter);

// eng: Sets the left position of the visibility range
// rus: Устанавливает левую позицию интервала видимости
int Magic_SetInterval1(HM_EMITTER hmEmitter,double position);

// eng: Returns the right position of the visibility range
// rus: Возвращает правую позицию интервала видимости
double Magic_GetInterval2(HM_EMITTER hmEmitter);

// eng: Sets the right position of the visibility range
// rus: Устанавливает правую позицию интервала видимости
int Magic_SetInterval2(HM_EMITTER hmEmitter,double position);

// eng: Figures out if the current animation position is within the visibility range
// rus: Определяет, попадает ли текущая позиция анимации в интервал видимости
bool Magic_InInterval(HM_EMITTER hmEmitter);

// eng: Sets the animation position at the left position of visibility range
// rus: Устанавливает эмиттер на первую границу интервала видимости
int Magic_EmitterToInterval1(HM_EMITTER hmEmitter, float speed_factor, const char* file);

// eng: Returns the flag of the animation of emitter that begins not from 0 position
// rus: Возвращает признак того, что анимация эмиттера начинается не с начала
bool Magic_IsInterval1(HM_EMITTER hmEmitter);

// eng: Copying the particles array into emitter from the file
// rus: Копирование пространства частиц в эмиттер из файла. 
int Magic_LoadArrayFromFile(HM_EMITTER hmEmitter, const char* file);

// eng: Copying the particles array from the emitter into the file
// rus: Копирование пространства частиц эмиттера в файл
int Magic_SaveArrayToFile(HM_EMITTER hmEmitter, const char* file);

// eng: Returns the particle positions interpolation usage flag
// rus: Возвращает признак режима интерполяции эмиттера
bool Magic_IsInterpolationMode(HM_EMITTER hmEmitter);

// eng: Sets/resets the particle positions interpolation usage flag
// rus: Включает/отключает режим интреполяции положения частиц
int Magic_SetInterpolationMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns the flag of stability/randomness of the emitter behaviour
// rus: Возвращает признак стабильности/случайности поведения эмиттера
bool Magic_IsRandomMode(HM_EMITTER hmEmitter);

// eng: Sets the flag of stability/randomness of the emitter behaviour
// rus: Устанавливает признак стабильности/случайности поведения эмиттера
int Magic_SetRandomMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns the emitter behaviour mode at the end of the animation
// rus: Возвращает режим поведения эмиттера после окончания анимации
int Magic_GetLoopMode(HM_EMITTER hmEmitter);

// eng: Sets the emitter behaviour mode at the end of the animation
// rus: Устанавливает режим поведения эмиттера после окончания анимации
int Magic_SetLoopMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the color management mode
// rus: Возвращает режим управления цветом частиц
int Magic_GetColorMode(HM_EMITTER hmEmitter);

// eng: Sets the color management mode
// rus: Устанавливает режим управления цветом частиц
int Magic_SetColorMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the user defined tint
// rus: Возвращает оттенок пользователя
int Magic_GetTint(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint
// rus: Устанавливает оттенок пользователя
int Magic_SetTint(HM_EMITTER hmEmitter,int tint);

// eng: Returns the user defined tint strength
// rus: Возвращает силу оттенка пользователя
float Magic_GetTintStrength(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint strength
// rus: Устанавливает силу оттенка пользователя
int Magic_SetTintStrength(HM_EMITTER hmEmitter,float tint_strength);

// eng: Returns the emitter scale
// rus: Возвращает масштаб эмиттера
float Magic_GetScale(HM_EMITTER hmEmitter);

// eng: Sets the emitter scale
// rus: Устанавливает масштаб эмиттера
int Magic_SetScale(HM_EMITTER hmEmitter, float scale);

// eng: Sets the user data
// rus: Устанавливает пользовательские данные
int Magic_SetData(HM_EMITTER hmEmitter, int data);

// eng: Returns the user data
// rus: Возвращает пользовательские данные
int Magic_GetData(HM_EMITTER hmEmitter);

// rus: Возвращает область рождения частиц
int Magic_GetBirthPlace(HM_EMITTER hmEmitter,MAGIC_BIRTH* place);

// rus: Устанавливает область рождения частиц
int Magic_SetBirthPlace(HM_EMITTER hmEmitter,MAGIC_BIRTH* place);

// eng: Returns the name of the emitter
// rus: Возвращает имя эмиттера
const char* Magic_GetEmitterName(HM_EMITTER hmEmitter);

// eng: Returns the shape of the emitter itself or the shape of the emitter for the specified particles type
// rus: Возвращает форму эмиттера или форму эмиттера для указанного типа частиц
int Magic_GetEmitterType(HM_EMITTER hmEmitter,int index);

// eng: Returns the ID of emitter
// rus: Возвращает идентификатор эмиттера
unsigned int Magic_GetEmitterID(HM_EMITTER hmEmitter);

#ifdef MAGIC_3D

// eng: Returns the flag indicating that emitter emits only 3D-particles
// rus: Возвращает признак того, что эмиттер излучает только 3D-частицы
bool Magic_Is3d(HM_EMITTER hmEmitter);

// eng: Returns the default matrix of emitter that was set using the Magic Particles (Dev). 
// rus: Возвращает матрицу вида, которая была установлена для эмиттера в Magic Particles
int Magic_GetViewMatrixDefault(HM_EMITTER hmEmitter,MAGIC_MATRIX* matrix,float distance_scale);

#endif

// eng: Returns coordinates of the emitter
// rus: Возвращает координаты эмиттера
int Magic_GetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter
// rus: Устанавливает координаты эмиттера
int Magic_SetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the emitter position. "Tail" from particles is formed between old and new position
// rus: Устанавливает координаты эмиттера. Между старой и новой позицией эмиттера образуется "хвост" из частиц
int Magic_SetEmitterPositionWithTail(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Returns the mode of the emitter coordinates
// rus: Возвращает режим координат эмиттера
bool Magic_GetEmitterPositionMode(HM_EMITTER hmEmitter);

// eng: Sets the mode of the emitter coordinates
// rus: Устанавливает режим координат эмиттера
int Magic_SetEmitterPositionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Moves particles
// rus: Перемещает частицы
int Magic_MoveEmitterParticles(HM_EMITTER hmEmitter,MAGIC_POSITION* offset);

// eng: Returns emitter direction
// rus: Возвращает направление эмиттера
int Magic_GetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Sets the direction of the emitter
// rus: Устанавливает направление эмиттера
int Magic_SetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Gets the emitter's direction mode
// rus: Возвращает режим вращения эмиттера
bool Magic_GetEmitterDirectionMode(HM_EMITTER hmEmitter);

// eng: Sets emitter's rotation mode
// rus: Устанавливает режим вращения эмиттера
int Magic_SetEmitterDirectionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Rotates particles
// rus: Вращает частицы
int Magic_RotateEmitterParticles(HM_EMITTER hmEmitter, MAGIC_DIRECTION* offset);

// eng: Returns the animate folder flag
// rus: Возвращает признак анимированной папки
bool Magic_IsFolder(HM_EMITTER hmEmitter);

// eng: Returns the number of emitters contained in animate folder. 1 is returned for emitter
// rus: Возвращает количество эмиттеров внутри эмиттера
int Magic_GetEmitterCount(HM_EMITTER hmEmitter);

// eng: Returns the specified emitter from animate folder. Returns itself for emitter
// rus: Возвращает дескриптор эмиттера внутри эмиттера
HM_EMITTER Magic_GetEmitter(HM_EMITTER hmEmitter,int index);

// eng: Creates the first portion of particles for emitter visualization and returns information on visualization settings. 
// rus: Создает первую порцию частиц для визуализации эмиттера и возвращает информацию о настройках визуализации
int Magic_CreateFirstRenderedParticlesList(HM_EMITTER hmEmitter, MAGIC_RENDERING* rendering);

// eng: Creates the next portion of particles for emitter visualization and returns information on visualization settings
// rus: Создает очередную порцию частиц для визуализации эмиттера и возвращает информацию о настройках визуализации
void Magic_CreateNextRenderedParticlesList(MAGIC_RENDERING* rendering);

// eng: Returns the MAGIC_PARTICLE_VERTEXES structure, containing the coordinates of the visualization rectangle vertice
// rus: Возвращает информацию о вершинах очередной частицы для её визуализации
int Magic_GetNextParticleVertexes(MAGIC_PARTICLE_VERTEXES* vertexes);

// eng: Returns particles sorting mode
// rus: Возвращает режим сортировки частиц
int Magic_GetSortingMode(HM_EMITTER hmEmitter);

// eng: Sets particles sorting mode
// rus: Устанавливает режим сортировки частиц
int Magic_SetSortingMode(HM_EMITTER hmEmitter, int mode);

#ifdef MAGIC_3D

// eng: Returns the view matrix that is set for API
// rus: Возвращает матрицу вида, установленную для API
void Magic_GetViewMatrix(MAGIC_MATRIX* matrix);

// eng: Sets the view matrix for API
// rus: Устанавливает матрицу вида для API
void Magic_SetViewMatrix(MAGIC_MATRIX* matrix);

#endif

// eng: Returnes Bounds Box
// rus: Возвращает BBox
int Magic_GetBBox(HM_EMITTER hmEmitter,MAGIC_BBOX* bbox);

// eng: Returns Bounds Box recalculation period
// rus: Возвращает период перерасчета Bounds Box
int Magic_GetBBoxPeriod(HM_EMITTER hmEmitter);

// eng: Sets Bounds Box recalculation period
// rus: Устанавливает период перерасчета Bounds Box
int Magic_SetBBoxPeriod(HM_EMITTER hmEmitter,int period);

// eng: Forces Bounds Box recalculation
// rus: Принудительно пересчитывает Bounds Box
int Magic_RecalcBBox(HM_EMITTER hmEmitter);

// --------------------------------------------------------------------------------

// eng: Returns the name of the particles type
// rus: Возвращает имя типа частиц
const char* Magic_GetParticlesTypeName(HM_EMITTER hmEmitter, int index);

// eng: Returns the number particles type contained in emitter
// rus: Возвращает количество типов частиц внутри эмиттера
int Magic_GetParticlesTypeCount(HM_EMITTER hmEmitter);

// eng: Locks the specified particles type for the further processing
// rus: Захватывает для дальнейшей обработки указанный тип частиц
int Magic_LockParticlesType(HM_EMITTER hmEmitter, int index);

// eng: Releases previously locked particles type
// rus: Освобождает захваченный ранее тип частиц
int Magic_UnlockParticlesType();

// eng: Returns the number of textural frame-files in locked particles type
// rus: Возвращает количество текстурных файл-кадров в захваченном типе частиц
int Magic_GetTextureCount();

// eng: Returns MAGIC_TEXTURE structure containing the specified frame-file information
// rus: Возвращает структуру   с информацией об указанном файл-кадре
int Magic_GetTexture(int index, MAGIC_TEXTURE* texture);

// eng: Sets texture coordinates for specified frame-file
// rus: Устанавливает текстурные координаты для указанного файл-кадра
int Magic_SetTextureUV(int index, float left, float top, float right, float bottom);

// eng: Returns the number used as a user resources texture identificator
// rus: Возвращает число, которое предназанчено для идентификации текстуры в ресурсах пользователя
unsigned int Magic_GetTextureID();

// eng: Sets the number used as a user resources texture identificator
// rus: Устанавливает число, которое предназанчено для идентификации текстуры в ресурсах пользователя
int Magic_SetTextureID(unsigned int id);

// eng: Returns the Intensity flag
// rus: Возвращает признак Интентивность
bool Magic_IsIntensive();

#ifdef MAGIC_3D

// eng: Returns particle type orientation for 3D-emitters
// rus: Возвращает ориентацию типа частиц для 3D-эмиттеров
int Magic_GetOrientation(MAGIC_ORIENTATION* orientation);

// eng: Sets particle type orientation for 3D-emitters
// rus: Устанавливает ориентацию типа частиц для 3D-эмиттеров
int Magic_SetOrientation(MAGIC_ORIENTATION* orientation);

#endif

// eng: Returns "tail" properties
// rus: Возвращает свойства "хвоста"
int Magic_GetTailProperties(MAGIC_TAIL* tail);

// eng: Sets "tail" properties
// rus: Устанавливает свойства "хвоста"
int Magic_SetTailProperties(MAGIC_TAIL* tail);

// eng: Returns the next particle. Is used to go through all the existing particles
// rus: Возвращает информацию об очередной частице. Используется для перебора всех существующих частиц
int Magic_GetNextParticle(MAGIC_PARTICLE* particle);

// eng: Changes the position of the particle that is got by Magic_GetNextParticle
// rus: Изменяет координаты частицы, полученной через Magic_GetNextParticle
void Magic_MoveParticle(MAGIC_POSITION* offset);

// eng: Rotates the particle that was obtained by Magic_GetNextParticle around the emitter
// rus: Вращает частицу полученную через Magic_GetNextParticle вокруг эмиттера
void Magic_RotateParticle(MAGIC_DIRECTION* offset);

// --------------------------------------------------------------------------

// eng: Sets the mode of atlas
// rus: Устанавливает режим работы атласа
void Magic_SetCleverModeForAtlas(bool clever);

// eng: Returns the mode of atlas
// rus: Возвращает режим работы атласа
bool Magic_IsCleverModeForAtlas();

// eng: Returns the information on next change in textural atlas
// rus: Возвращает информацию об очередном изменении в текстурном атласе
int Magic_GetNextAtlasChange(MAGIC_CHANGE_ATLAS* change);

// eng: Creates textural atlases for all loaded emitters
// rus: Создает текстурные атласы для всех загруженных эмиттеров
float Magic_CreateAtlases(int width,int height,int step,float scale_step);

// eng: Creates textural atlases for specified emitters
// rus: Создает текстурные атласы для указанных эмиттеров
float Magic_CreateAtlasesForEmitters(int width,int height,int count, HM_EMITTER* emitters,int step,float scale_step);

// eng: Sets up the initial scale for atlas creation
// rus: Устанавливает стартовый масштаб для постороения атласа
void Magic_SetStartingScaleForAtlas(float scale);

// eng: Returns the initial scale for atlas creation
// rus: Возвращает стартовый масштаб для постороения атласа
float Magic_GetStartingScaleForAtlas();

// eng: Returns the number of textural atlases
// rus: Возвращает количество атласов
int Magic_GetAtlasCount();

// eng: Returns the textural atlas specified
// rus: Возвращает указанный текстурный атлас
int Magic_GetAtlas(int index, MAGIC_ATLAS* atlas);

// --------------------------------------------------------------------------

// eng: Figures out if the diagram is managable
// rus: Определяет, доступен ли указанный график
bool Magic_IsDiagramEnabled(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Returns the specified diagram factor
// rus: Возвращает множитель для указанного графика
float Magic_GetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the specified diagram factor
// rus: Устанавливает множитель для указанного графика
int Magic_SetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index, float factor);

// eng: Returns the factor for emitter form diagram
// rus: Возвращает множитель для графика формы эмиттера
float Magic_GetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the factor for emitter form diagram
// rus: Устанавливает множитель для графика формы эмиттера
int Magic_SetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line, float factor);

// eng: Returns the offset for the specified diagram
// rus: Возвращает смещение для указанного графика
float Magic_GetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the offset for the specified diagram
// rus: Устанавливает смещение для указанного графика
int Magic_SetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index, float addition);

// eng: Returns the offset for emitter form diagram
// rus: Возвращает смещение для графика формы эмиттера
float Magic_GetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the offset for emitter form diagram
// rus: Устанавливает смещение для графика формы эмиттера
int Magic_SetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line, float addition);

// eng: Allows substituting a graphical pattern which is used to generate particles of "Image" and "Text" formed emitters
// rus: Позволяет заменить графический образ, по которому происходит генерация частиц у эмиттеров типа "Картинка" и "Текст"
int Magic_ChangeImage(HM_EMITTER hmEmitter, int type_index, int width, int height, unsigned char* data, int bytes_per_pixel);

// eng: Allows changing the triangle based model by which particle generation occurs in "Model" formed emitters
// rus: Позволяет заменить модель из треугольников, по которой происходит генерация частиц у эмиттера типа "Модель"
int Magic_ChangeModel(HM_EMITTER hmEmitter, int type_index, int count, MAGIC_TRIANGLE* triangles);

// --------------------------------------------------------------------------------------

// eng: Creates a new key on a Timeline
// rus: Создает новый ключ указанного типа на Шкале времени
int Magic_CreateKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key);

// eng: Deletes the specified key of desired type from Timeline
// rus: Удаляет выбранный ключ указанного типа со Шкалы времени
int Magic_DeleteKey(HM_EMITTER hmEmitter, int type, int index);

// eng: Returns the number of keys of specified type from Timeline
// rus: Возвращает количество ключей указанного типа на Шкале времени
int Magic_GetKeyCount(HM_EMITTER hmEmitter, int type);

// eng: Returns information for the key of specified type
// rus: Возвращает информацию о ключе указанного типа
int Magic_GetKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key, int index);

// eng: Sets the new data for the key of specified type
// rus: Устанавливает новые данные для ключа указанного типа
int Magic_SetKey(HM_EMITTER hmEmitter, int type, MAGIC_KEY* key, int index);

// --------------------------------------------------------------------------------------

// eng: Converts UTF16 string into UTF8
// rus: Конвертирует строку типа UTF16 в строку типа UTF8
const char* Magic_UTF16to8(const short* str);

// eng: Converts UTF8 string into UTF16
// rus: Конвертирует строку типа UTF8 в строку типа UTF16
const short* Magic_UTF8to16(const char* str);

// --------------------------------------------------------------------------------------

typedef int HM_STREAM;


#define MAGIC_STREAM_READ		0
#define MAGIC_STREAM_WRITE		1
#define MAGIC_STREAM_ADD		2


// Открытие потока из файла
// Параметр file_name - путь к файлу в формате UTF8.
// Параметр mode:
// mode=MAGIC_STREAM_READ - поток открывается на чтение. Позиция потока устанавливается на начало.
// mode=MAGIC_STREAM_WRITE - поток открывается на запись. Если файл отсутствовал, то он создается пустым, если файл существовал, то он очищается.
// mode=MAGIC_STREAM_ADD - поток открывается на запись. Позиция потока устанавливается на конец файла. Если файл отсутствовал, то он создается.
// Возвращает ижентификатор потока или 0
int Magic_StreamOpenFile(const char* file_name, int mode);

// Открытие потока из памяти
// Параметр address - адрес буфера потока памяти.
// Параметр length - размер буфера потока в байтах.
// Параметр mode:
// mode=MAGIC_STREAM_READ - поток открывается на чтение. Позиция потока устанавливается на начало. address задает начало потока. length - размер потока. Если length=0, то размер потока не определен, но читать из него можно.
// mode=MAGIC_STREAM_WRITE - поток открывается на запись. В памяти создается пустой поток. Параметры address и length не имеют значения (должны быть равны 0).
// mode=MAGIC_STREAM_ADD - поток открывается на запись. При этом снимается копия с области памяти address с размером length. Эта копия работает независимо внутри API, позволяя дописывать байты. Позиция потока устанавливается на конец.
// Возвращает ижентификатор потока или 0
int Magic_StreamOpenMemory(const char* address, unsigned int length, int mode);

// Закрытие потока
// Поток, созданный из файла закрывает этот файл, но не удаляет его, а поток сделаный в памяти уничтожает все свои данные.
int Magic_StreamClose(HM_STREAM hmStream);

// Закрытие всех существующих потоков
void Magic_StreamCloseAll();

// Возвращает размер потока
unsigned int Magic_StreamGetLength(HM_STREAM hmStream);

// Возвращает текущую позицию в потоке
unsigned int Magic_StreamGetPosition(HM_STREAM hmStream);

// Устанавливает позицию в потоке. Работает только для режима MAGIC_STREAM_READ
int Magic_StreamSetPosition(HM_STREAM hmStream, unsigned int position);

// Записывает в поток байты в указанном количестве
int Magic_StreamWrite(HM_STREAM hmStream, const char* data, unsigned int count);

// Читает из потока байты в указанном количестве
int Magic_StreamRead(HM_STREAM hmStream, char* data, unsigned int count);

// Возвращает режим работы потока MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD
int Magic_StreamGetMode(HM_STREAM hmStream);

// Устанавливает режим работы потока MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD
int Magic_StreamSetMode(HM_STREAM hmStream, int mode);

// Возвращает имя файла потока или NULL. Можно использовать, чтобы отличить поток из файла и поток из памяти
const char* Magic_StreamGetFileName(HM_STREAM hmStream);

// Открытие ptc-файла. Аналогичен функциям Magic_OpenFile/Magic_OpenFileInMemory
int Magic_OpenStream(HM_STREAM hmStream);

// Копирование пространства частиц эмиттера в поток
int Magic_SaveArrayToStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// Замена пространства частиц эмиттера ранее созданной копией из потока
int Magic_LoadArrayFromStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// Устанавливает эмиттер на первую границу интервала видимости. Если hmStream не равен 0, то происходит работа в потоком:
// Если размер потока равен 0, то в этот поток сохраняются данные о частицах через Magic_SaveArrayToStream
// Если размер потока не равен 0, то из этого потока извлекаются данные о частицах через Magic_LoadArrayFromStream
int Magic_EmitterToInterval1_Stream(HM_EMITTER hmEmitter, float speed_factor, HM_STREAM hmStream);

// Удаление всех данных библиотеки
void Magic_DestroyAll();


// Возвращает заданный в Magic Particles темп, используемый для анимации эмиттера
float Magic_GetUpdateTemp(HM_EMITTER hmEmitter);

struct MAGIC_RECT
{
	int left;
	int top;
	int right;
	int bottom;
};

// Возвращает положение фоновой картинки
int Magic_GetBackgroundRect(HM_EMITTER hmEmitter, MAGIC_RECT* rect);

//#define MAGIC_BEAM
#define MAGIC_ADDITIONAL_TYPE_NORMAL	0
#define MAGIC_ADDITIONAL_TYPE_TRAIL		1
#define MAGIC_ADDITIONAL_TYPE_BEAM		2

//#define MAGIC_MATERIAL

#ifdef __cplusplus
}
#endif

#endif
