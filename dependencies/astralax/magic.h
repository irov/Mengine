//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif

typedef int HM_FILE;
typedef int HM_EMITTER;
typedef int HM_IMPORT;
typedef int HM_OBSTACLE;
typedef int HM_WIND;
typedef unsigned int HM_PARTICLE;
typedef int HM_STREAM;


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
// rus: MAGIC_PARTICLE - структура частицы, описывающая ее характеристики.
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

	unsigned int color;
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

// eng: MAGIC_SEGMENT - structure to store line segment coordinates.
// rus: MAGIC_SEGMENT - структура для хранения координат отрезка.
struct MAGIC_SEGMENT
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
};

// eng: MAGIC_RECT - structure to store rectangle
// rus: MAGIC_RECT - структура для хранения прямоугольника
struct MAGIC_RECT
{
	int left;
	int top;
	int right;
	int bottom;
};

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

// eng: MAGIC_VARIABLE - structure for variables
// rus: MAGIC_VARIABLE - структура для переменных
struct MAGIC_VARIABLE
{
	const char* name;
	int type;
	char value[8];
};
#define MAGIC_VARIABLE_BOOL		0
#define MAGIC_VARIABLE_INT		1
#define MAGIC_VARIABLE_FLOAT	2
#define MAGIC_VARIABLE_STRING	3
#define MAGIC_VARIABLE_DIAGRAM	4

// eng: MAGIC_ACTION - structure for actions
// rus: MAGIC_ACTION - структура для действий
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
#define MAGIC_EVENT_CREATION					0
#define MAGIC_EVENT_DESTRUCTION					1
#define MAGIC_EVENT_EXISTENCE					2
#define MAGIC_EVENT_COLLISION					3
#define MAGIC_EVENT_MAGNET						4
#define MAGIC_EVENT_WIND						5

#define MAGIC_ACTION_EVENT						0
#define MAGIC_ACTION_DESTRUCTION				1
#define MAGIC_ACTION_DETACHING					2
#define MAGIC_ACTION_FACTOR						3
#define MAGIC_ACTION_PARTICLE					4
#define MAGIC_ACTION_MAGNET_PARTICLE			5

#define MAGIC_ACTION_DIRECTION_DISABLED			0
#define MAGIC_ACTION_DIRECTION_MOVEMENT			1
#define MAGIC_ACTION_DIRECTION_REBOUND			2

#define MAGIC_MAGNET_POINT_ANY					0
#define MAGIC_MAGNET_POINT_DIRECTION			1
#define MAGIC_MAGNET_POINT_OPPOSITE_DIRECTION	2

// eng: MAGIC_EVENT - structure to get event information
// rus: MAGIC_EVENT - структура для получения информации о событии
struct MAGIC_EVENT
{
	int event;
	HM_PARTICLE hmParticle;
	MAGIC_POSITION position1;
	MAGIC_POSITION position2;
	MAGIC_POSITION reflection;
};

// eng: MAGIC_OBSTACLE - structure defining the shape of obstacle
// rus: MAGIC_OBSTACLE - структура, которая описывает форму препятствия
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
#if defined(EXE) || defined(SCREENSAVER) || (!defined(MAGIC_3D))
#define MAGIC_OBSTACLE_CIRCLE		0	// circle/окружность (2D)
#define MAGIC_OBSTACLE_SEGMENT		1	// segment/отрезок (2D)
#endif
#ifdef MAGIC_3D
#define MAGIC_OBSTACLE_SPHERE		2	// sphere/сфера (3D)
#define MAGIC_OBSTACLE_TRIANGLE		3	// triangle/треугольник (3D)
#endif

// eng: MAGIC_WIND - structure that defines wind
// rus: MAGIC_WIND - структура, которая описывает ветер
struct MAGIC_WIND
{
	MAGIC_POSITION direction;
	float velocity;
};



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

#define MAGIC_TYPE_OBSTACLE		0
#define MAGIC_TYPE_WIND			1
#define MAGIC_TYPE_MAGNET		2

#define MAGIC_PROPERTY_ANGLE			0
#define MAGIC_PROPERTY_SIZE				1
#define MAGIC_PROPERTY_VELOCITY			2
#define MAGIC_PROPERTY_WEIGHT			3
#define MAGIC_PROPERTY_SPIN				4
#define MAGIC_PROPERTY_ANGULAR_VELOCITY	5
#define MAGIC_PROPERTY_MOTION_RAND		6
#define MAGIC_PROPERTY_VISIBILITY		7

#define MAGIC_STREAM_READ		0
#define MAGIC_STREAM_WRITE		1
#define MAGIC_STREAM_ADD		2

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

#else

#define MAGIC_pXpY		0
#define MAGIC_pYpX		1
#define MAGIC_nXpY		2
#define MAGIC_pYnX		3
#define MAGIC_pXnY		4
#define MAGIC_nYpX		5
#define MAGIC_nXnY		6
#define MAGIC_nYnX		7

#endif

// --------------------------------------------------------------------------------------

#ifdef MAGIC_3D

// eng: Returns the view matrix that is set for API
// rus: Возвращает матрицу вида, установленную для API
void Magic_GetViewMatrix(MAGIC_MATRIX* matrix);

// eng: Sets the view matrix for API
// rus: Устанавливает матрицу вида для API
void Magic_SetViewMatrix(MAGIC_MATRIX* matrix);

#endif

// eng: Returns the direction of coordinate axes
// rus: Возвращает направление координатных осей
int Magic_GetAxis();

// eng: Sets the direction of coordinate axes
// rus: Устанавливает направление координатных осей
int Magic_SetAxis(int axis_index);

// eng: Converts UTF8 string into UTF16
// rus: Конвертирует строку типа UTF8 в строку типа UTF16
const unsigned short* Magic_UTF8to16(const unsigned char* str);

// eng: Converts UTF8 string into UTF32
// rus: Конвертирует строку типа UTF8 в строку типа UTF32
const unsigned int* Magic_UTF8to32(const unsigned char* str);

// eng: Converts UTF16 string into UTF8
// rus: Конвертирует строку типа UTF16 в строку типа UTF8
const unsigned char* Magic_UTF16to8(const unsigned short* str);

// eng: Converts UTF16 string into UTF32
// rus: Конвертирует строку типа UTF16 в строку типа UTF32
const unsigned int* Magic_UTF16to32(const unsigned short* str);

// eng: Converts UTF32 string into UTF8
// rus: Конвертирует строку типа UTF32 в строку типа UTF8
const unsigned char* Magic_UTF32to8(const unsigned int* str);

// eng: Converts UTF32 string into UTF16
// rus: Конвертирует строку типа UTF32 в строку типа UTF16
const unsigned short* Magic_UTF32to16(const unsigned int* str);

// eng: Populates MAGIC_ACTION structure with default values
// rus: Заполняет структуру MAGIC_ACTION значениями по умолчанию
void MAGIC_ACTION_Identity(MAGIC_ACTION* action);

// --------------------------------------------------------------------------------------

// eng: Loads the ptc-file from the path specified
// rus: Загружает ptc-файл по указанному пути
HM_FILE Magic_OpenFile(const char* file_name);

// eng: Loads the ptc-file image from the memory
// rus: Открытие образа ptc-файла из памяти
HM_FILE Magic_OpenFileInMemory(const char* buffer);

// eng: Loads the ptc-file from the stream
// rus: Загружает ptc-файл из указанного потока
int Magic_OpenStream(HM_STREAM hmStream);

// eng: Closes the file, opened earlier by use of Magic_OpenFile, Magic_OpenFileInMemory or Magic_OpenStream
// rus: Закрывает файл, открытый ранее через Magic_OpenFile, Magic_OpenFileInMemory или Magic_OpenStream
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

// eng: Unloads all loaded emitters
// rus: Выгружает все загруженные эмиттеры
void Magic_UnloadAllEmitters();

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

// eng: Returns the value of "Speed" that was set in Magic Particles (Dev)
// rus: Возвращает заданное в Magic Particles (Dev) значение "коэффициент темпа"
float Magic_GetUpdateSpeed(HM_EMITTER hmEmitter);

// eng: Returns information about background image of emitter
// rus: Возвращает информацию о фоновом изображении эмиттера
float Magic_GetBackgroundRect(HM_EMITTER hmEmitter, MAGIC_RECT* rect);

#ifdef MAGIC_3D

// eng: Returns the flag indicating that emitter emits only 3D-particles
// rus: Возвращает признак того, что эмиттер излучает только 3D-частицы
bool Magic_Is3d(HM_EMITTER hmEmitter);

// eng: Returns the default matrix of emitter that was set using the Magic Particles (Dev).
// rus: Возвращает матрицу вида, которая была установлена для эмиттера в Magic Particles
int Magic_GetViewMatrixDefault(HM_EMITTER hmEmitter,MAGIC_MATRIX* matrix,float distance_scale);

#else

// eng: Transforms coordinates of 2D-emitter from Magic Particles (Dev) into scene coordinates
// rus: Преобразует координаты 2D-эмиттера из Magic Particles (Dev) в координаты сцены
int Magic_CorrectEmitterPosition(HM_EMITTER hmEmitter, int scene_width, int scene_height);
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

// eng: Returns the count of user defined variables of emitter or animated folder
// rus: Возвращает количество пользовательских переменных внутри эмиттера или анимированной папки
int Magic_GetEmitterVariableCount(HM_EMITTER hmEmitter);

// eng: Returns information about user defined variable of emitter or animated folder
// rus: Возвращает информацию о пользовательской переменной из эмиттера или анимированной папки
int Magic_GetEmitterVariable(HM_EMITTER hmEmitter, int index, MAGIC_VARIABLE* variable);

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

// eng: Returns count of user defined variables of particles type
// rus: Возвращает количество пользовательских переменных внутри типа частиц
int Magic_GetParticlesTypeVariableCount();

// eng: Returns information about user defined variable of particles type
// rus: Возвращает информацию о пользовательской переменной из типа частиц
int Magic_GetParticlesTypeVariable(int index, MAGIC_VARIABLE* variable);

// eng: Returns the count of actions
// rus: Возвращает количество действий
int Magic_GetActionCount();

// eng: Returns MAGIC_VARIABLE structure that contains action information
// rus: Возвращает структуру MAGIC_ACTION с информацией об указанном действии
int Magic_GetAction(int index, MAGIC_ACTION* action);

// eng: Creates new action
// rus: Создает новое действие
int Magic_CreateAction(MAGIC_ACTION* action);

// eng: Deletes specified action
// rus: Удаляет указанное действие
int Magic_DestroyAction(int index);

// eng: Returns count of attached physical objects of specified type
// rus: Возвращает количество присоединенных физических объектов указанного типа
int Magic_GetAttachedPhysicObjectsCount(int type);

// eng: Returns list of attached physical objects of specified type
// rus: Возвращает список присоединенных физических объектов указанного типа
int Magic_GetAttachedPhysicObjects(int type, int* HMs);

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

// eng: Creates obstacle
// rus: Создает препятствие
HM_OBSTACLE Magic_CreateObstacle(MAGIC_OBSTACLE* data, MAGIC_POSITION* position, int cell);

// eng: Returns information about shape of obstacle
// rus: Возвращает информацию о форме препятствия
int Magic_GetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data);

// eng: Sets new shape of obstacle
// rus: Устанавливает новую форму для препятствия
int Magic_SetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data, int cell);

// eng: Returns position of obstacle
// rus: Возвращает позицию препятствия
int Magic_GetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Sets new position of obstacle
// rus: Устанавливает новую позицию для препятствия
int Magic_SetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Creates wind
// rus: Создает ветер
HM_WIND Magic_CreateWind(MAGIC_WIND* data);

// eng: Returns information about wind
// rus: Возвращает информацию о ветре
int Magic_GetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Sets new properties for wind
// rus: Устанавливает новые свойства для ветра
int Magic_SetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Deletes obstacle or wind
// rus: Уничтожает препятствие или ветер
int Magic_DestroyPhysicObject(int type, int HM);

// eng: Deletes all obstacles and winds
// rus: Уничтожает все препятствия или ветры
int Magic_DestroyAllPhysicObjects(int type);

// eng: Duplicates specified obstacle or wind
// rus: Снимает копию с указанного препятствия или ветра
int Magic_DuplicatePhysicObject(int type, int HM);

// --------------------------------------------------------------------------------------

// eng: Returns information about subsequent event
// rus: Возвращает информацию об очередном событии
int Magic_GetNextEvent(MAGIC_EVENT* evt);

// eng: Returns user data of particle
// rus: Возвращает пользовательские данные частицы
void* Magic_ParticleGetData(HM_PARTICLE hmParticle);

// eng: Sets user data of particle
// rus: Устанавливает пользовательские данные частицы
void Magic_ParticleSetData(HM_PARTICLE hmParticle, void* data);

// eng: Returns scene coordinates of particle
// rus: Возвращает координаты частицы на сцене
void Magic_ParticleGetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);

// eng: Sets coordinates of particle
// rus: Устанавливает координаты частицы
void Magic_ParticleSetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);

// eng: Returns specified property of particle
// rus: Возвращает указанное свойство частицы
float Magic_ParticleGetProperty(HM_PARTICLE hmParticle, int property);

// eng: Sets specified property of particle
// rus: Устанавливает указанное свойство частицы
void Magic_ParticleSetProperty(HM_PARTICLE hmParticle, int property, float value);

// eng: Returns several specified properties of particle
// rus: Возвращает несколько указанных свойств частицы
void Magic_ParticleGetProperties(HM_PARTICLE hmParticle, int count, int* properties, float* values);

// eng: Sets several specified properties of particle
// rus: Устанавливает несколько указанных свойств частицы
void Magic_ParticleSetProperties(HM_PARTICLE hmParticle, int count, int* properties, float* values);

// eng: Returns physical radius of particle
// rus: Возвращает физический радиус частицы
float Magic_ParticleGetRadius(HM_PARTICLE hmParticle);

// eng: Detaches particle from emitter
// rus: Отсоединяет частицу от эмиттера
void Magic_ParticleDetaching(HM_PARTICLE hmParticle);

// eng: Deletes particle
// rus: Уничтожает частицу
void Magic_ParticleDestruction(HM_PARTICLE hmParticle);

// --------------------------------------------------------------------------------------

// eng: Opens stream from file
// rus: Открывает поток из файла
int Magic_StreamOpenFile(const char* file_name, int mode);

// eng: Opens stream in memory
// rus: Открывает поток в памяти
int Magic_StreamOpenMemory(const char* address, unsigned int length, int mode);

// eng: Closes stream that was previously opened by Magic_StreamOpenFile or Magic_StreamOpenMemory
// rus: Закрывает поток, открытый ранее через Magic_StreamOpenFile или Magic_StreamOpenMemory
int Magic_StreamClose(HM_STREAM hmStream);

// eng: Closing all opened streams
// rus: Закрытие всех открытых поток
void Magic_StreamCloseAll();

// eng: Returns the length of stream
// rus: Возвращает размер потока
unsigned int Magic_StreamGetLength(HM_STREAM hmStream);

// eng: Returns current stream position
// rus: Возвращает текущую позицию в потоке
unsigned int Magic_StreamGetPosition(HM_STREAM hmStream);

// eng: Sets the position of stream that was opened in read only mode
// rus: Устанавливает позицию потока, открытого в режиме чтения
int Magic_StreamSetPosition(HM_STREAM hmStream, unsigned int position);

// eng: Returns current stream mode
// rus: Возвращает текущий режим потока
int Magic_StreamGetMode(HM_STREAM hmStream);

// eng: Sets new stream mode
// rus: Устанавливает новый режим потока
int Magic_StreamSetMode(HM_STREAM hmStream, int mode);

// eng: Returns file name of stream opened from file
// rus: Возвращает имя файла для потока, открытого из файла
const char* Magic_StreamGetFileName(HM_STREAM hmStream);

// eng: Reads specified number of bytes from stream
// rus: Читает из потока указанное количество байт
int Magic_StreamRead(HM_STREAM hmStream, char* data, unsigned int count);

// eng: Writes specified number of bytes into stream
// rus: Сохраняет в поток указанное количество байт
int Magic_StreamWrite(HM_STREAM hmStream, const char* data, unsigned int count);

// eng: Copying particle space from stream into emitter
// rus: Копирование пространства частиц в эмиттер из потока
int Magic_LoadArrayFromStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Copying particle space from emitter into stream
// rus: Копирование пространства частиц из эмиттера а поток
int Magic_SaveArrayToStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Sets emitter animation position to left position of visibility range
// rus: Устанавливает позицию анимации эмиттера на левую позицию интервала видимости
int Magic_EmitterToInterval1_Stream(HM_EMITTER hmEmitter, float speed_factor, HM_STREAM hmStream);

// --------------------------------------------------------------------------------------

void Magic_DestroyAll();

// eng: Sets the coordinates of the emitter (attached physic objects are moved too)
// rus: Устанавливает координаты эмиттера (происходит перемещение всех прицепленных физ. объектов)
int Magic_SetEmitterPositionWithAttachedPhysicObjects(HM_EMITTER hmEmitter, MAGIC_POSITION* pos);


#ifdef __cplusplus
}
#endif

#endif
