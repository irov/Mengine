#pragma once

#include "Interface/DebugFileInterface.h"

#include "Kernel/PathHelper.h"

#if defined(MENGINE_DEBUG)
#   include "Config/DynamicCast.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugRelationPath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG)
            const DebugFileInterface * debug = Helper::dynamicCast<const DebugFileInterface *>( _ptr );

            if( debug == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & relationPath = debug->getDebugRelationPath();

            return relationPath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugFolderPath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG)
            const DebugFileInterface * base = Helper::dynamicCast<const DebugFileInterface *>( _ptr );

            if( base == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & folderPath = base->getDebugFolderPath();

            return folderPath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugFilePath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG)
            const DebugFileInterface * base = Helper::dynamicCast<const DebugFileInterface *>( _ptr );

            if( base == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & filePath = base->getDebugFilePath();

            return filePath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const Char * getDebugFullPath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG)
            static MENGINE_THREAD_LOCAL Char fullPath[MENGINE_MAX_PATH + 1] = {'\0'};

            const FilePath & relationPath = Helper::getDebugRelationPath( _ptr );
            const FilePath & folderPath = Helper::getDebugFolderPath( _ptr );
            const FilePath & filePath = Helper::getDebugFilePath( _ptr );

            Helper::concatenateFilePath( {relationPath, folderPath, filePath}, fullPath );

            return fullPath;
#else
            return "";
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugRelationPath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::getDebugRelationPath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugFolderPath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::getDebugFolderPath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugFilePath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::getDebugFilePath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const Char * getDebugFullPath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::getDebugFullPath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}