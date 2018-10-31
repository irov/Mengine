#pragma once

#include "Kernel/Resource.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMusic
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceMusic();
        ~ResourceMusic() override;

    public:
        const FilePath & getPath() const;
        const ConstString & getCodec() const;

        float getVolume() const;
        bool isExternal() const;

    protected:
        bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    private:
        FilePath m_path;
        ConstString m_codec;
        ConstString m_converter;

        float m_volume;
        bool m_external;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceMusic> ResourceMusicPtr;
}
