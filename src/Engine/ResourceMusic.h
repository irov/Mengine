#pragma once

#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceMusic
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceMusic );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceMusic();
        ~ResourceMusic() override;

    public:
        void setVolume( float _volume );
        float getVolume() const;

        void setExternal( bool _external );
        bool isExternal() const;

    protected:
        float m_volume;
        bool m_external;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMusic> ResourceMusicPtr;
    //////////////////////////////////////////////////////////////////////////
}
