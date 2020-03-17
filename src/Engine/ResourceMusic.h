#pragma once

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMusic
        : public Resource
        , private BaseContent
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

    public:
        ResourceMusic();
        ~ResourceMusic() override;

    public:
        void setVolume( float _volume );
        float getVolume() const;

        void setExternal( bool _external );
        bool isExternal() const;

    private:
        float m_volume;
        bool m_external;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMusic> ResourceMusicPtr;
    //////////////////////////////////////////////////////////////////////////
}
