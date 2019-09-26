#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    class SpriteCache
        : public Factorable
    {
    public:
        SpriteCache();
        ~SpriteCache();

    public:
        bool initialize();
        void finalize();

    public:
        const ResourceImagePtr & getResourceImage( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, const mt::vec2f & _maxSize );

    protected:
        struct CacheKey
        {
            CacheKey()
                : hash( ~0ULL )
            {
            }

            CacheKey( const ConstString & _fileGroupName, const FilePath & _filePath )
                : fileGroupName( _fileGroupName )
                , filePath( _filePath )
                , hash( _fileGroupName.hash() ^ _filePath.hash() )
            {
            }

            ConstString fileGroupName;
            FilePath filePath;
            HashType hash;

            bool operator == ( const CacheKey & _key ) const
            {
                return fileGroupName == _key.fileGroupName && filePath == _key.filePath;
            }
        };

        struct CacheKeyHashgen
        {
            HashType operator() ( const CacheKey & _key ) const
            {
                return _key.hash;
            }
        };

        typedef Hashtable<CacheKey, ResourceImagePtr, CacheKeyHashgen> HashtableResources;
        HashtableResources m_resources;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SpriteCache> SpriteCachePtr;
    //////////////////////////////////////////////////////////////////////////
}