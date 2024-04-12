MENGINE_PROJECT(AndroidFileGroupPlugin)

ADD_FILTER(
src
    AndroidFileGroupPlugin.h
    AndroidFileGroupPlugin.cpp

    AndroidAssetGroupDirectory.cpp
    AndroidAssetGroupDirectory.h
    AndroidAssetInputStream.h
    AndroidAssetInputStream.cpp
    AndroidMutexAssetInputStream.h
    AndroidMutexAssetInputStream.cpp
    
    AndroidFileGroupDirectory.cpp
    AndroidFileGroupDirectory.h
    AndroidFileInputStream.h
    AndroidFileInputStream.cpp
    AndroidMutexFileInputStream.h
    AndroidMutexFileInputStream.cpp
    AndroidFileOutputStream.cpp
    AndroidFileOutputStream.h
)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_FILEGROUP)