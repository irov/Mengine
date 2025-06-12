#pragma once

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct FrameImageSequence
    {
        ResourceImagePtr resourceImage;
        float delay;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FrameImageSequence> VectorFrameImageSequence;
    //////////////////////////////////////////////////////////////////////////
    class ResourceImageSequence
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceImageSequence );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceImageSequence();
        ~ResourceImageSequence() override;

    public:
        uint32_t getSequenceCount() const;
        float getSequenceDelay( uint32_t _index ) const;
        const ResourceImagePtr & getSequenceResource( uint32_t _index ) const;

        uint32_t getLastFrameIndex() const;

        void setSequenceDuration( float _duration );
        float getSequenceDuration() const;

    public:
        void addFrame( const ResourceImagePtr & _resourceImage, float _delay );

    public:
        void setSequence( const VectorFrameImageSequence & _sequence );
        const VectorFrameImageSequence & getSequence() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        VectorFrameImageSequence m_sequence;

        float m_duration;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceImageSequence> ResourceImageSequencePtr;
    //////////////////////////////////////////////////////////////////////////
}





