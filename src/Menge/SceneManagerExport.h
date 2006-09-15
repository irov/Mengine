#ifdef SCENE_MANAGER_EXPORT
#define SCENE_MANAGER_API __declspec(dllexport)
#else
#define SCENE_MANAGER_API __declspec(dllimport)
#endif

#pragma warning ( disable : 4251 ) // needs to have dll-interface to be used by clients of class
