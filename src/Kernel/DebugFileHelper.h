#pragma once

#include "Interface/FileServiceInterface.h"

#include "Kernel/PathHelper.h"
#include "Kernel/PathString.h"

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
#   include "Config/Path.h"
#   include "Config/DynamicCast.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void addDebugFilePath( const T * _ptr, const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
        {
            MENGINE_UNUSED( _ptr );
            MENGINE_UNUSED( _relationPath );
            MENGINE_UNUSED( _folderPath );
            MENGINE_UNUSED( _filePath );

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            const Factorable * factorable = Helper::dynamicCast<const Factorable *>( _ptr );

            UniqueId id = factorable->getUniqueIdentity();

            FILE_SERVICE()
                ->addDebugFilePath( id, _relationPath, _folderPath, _filePath );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void addDebugFilePath( const IntrusivePtr<T> & _ptr, const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
        {
            return Helper::addDebugFilePath( _ptr.get(), _relationPath, _folderPath, _filePath );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeDebugFilePath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            const Factorable * factorable = Helper::dynamicCast<const Factorable *>( _ptr );

            UniqueId id = factorable->getUniqueIdentity();

            FILE_SERVICE()
                ->removeDebugFilePath( id );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void removeDebugFilePath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::removeDebugFilePath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const FilePath & getDebugRelationPath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            const Factorable * factorable = Helper::dynamicCast<const Factorable *>( _ptr );

            UniqueId id = factorable->getUniqueIdentity();

            const FilePath & relationPath = FILE_SERVICE()
                ->getDebugRelationPath( id );

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

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            const Factorable * factorable = Helper::dynamicCast<const Factorable *>( _ptr );

            UniqueId id = factorable->getUniqueIdentity();

            const FilePath & folderPath = FILE_SERVICE()
                ->getDebugFolderPath( id );

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

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            const Factorable * factorable = Helper::dynamicCast<const Factorable *>( _ptr );

            UniqueId id = factorable->getUniqueIdentity();

            const FilePath & filePath = FILE_SERVICE()
                ->getDebugFilePath( id );

            return filePath;
#else
            return FilePath::none();
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        PathString getDebugFullPath( const T * _ptr )
        {
            MENGINE_UNUSED( _ptr );

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
            Path fullPath = {'\0'};

            const FilePath & relationPath = Helper::getDebugRelationPath( _ptr );
            const FilePath & folderPath = Helper::getDebugFolderPath( _ptr );
            const FilePath & filePath = Helper::getDebugFilePath( _ptr );

            Helper::concatenateFilePath( {relationPath, folderPath, filePath}, fullPath );

            return PathString( fullPath );
#else
            return PathString();
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
        PathString getDebugFullPath( const IntrusivePtr<T> & _ptr )
        {
            return Helper::getDebugFullPath( _ptr.get() );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}