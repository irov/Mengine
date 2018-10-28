#pragma once

#include "Kernel/AffectorType.h"
#include "Kernel/Mixin.h"

#include "math/vec3.h"

#include "stdex/intrusive_slug_list_ptr.h"
#include "stdex/intrusive_slug_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct UpdateContext;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Affector> AffectorPtr;
    //////////////////////////////////////////////////////////////////////////
    class Affectorable
        : public Mixin
    {
    public:
        Affectorable();
        ~Affectorable() override;

    public:
        AFFECTOR_ID addAffector( const AffectorPtr & _affector );
        bool stopAffector( AFFECTOR_ID _id );

    public:
        void stopAffectors( EAffectorType _type );

    public:
        void stopAllAffectors();

    public:
        void setAngularSpeed( float _angular );
        float getAngularSpeed() const;

        void setLinearSpeed( const mt::vec3f & _linearSpeed );
        const mt::vec3f & getLinearSpeed() const;

    public:
        virtual uint32_t getAffectorableUpdatableMode() const = 0;
        virtual uint32_t getAffectorableUpdatableLeafDeep() const = 0;

    protected:
        typedef stdex::intrusive_slug_list_ptr<Affector> IntrusiveSlugListAffector;
        typedef stdex::intrusive_slug_ptr<IntrusiveSlugListAffector> TSlugAffector;

        IntrusiveSlugListAffector m_affectors;

        float m_angularSpeed;
        mt::vec3f m_linearSpeed;

        uint32_t m_enumerator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affectorable> AffectorablePtr;
    //////////////////////////////////////////////////////////////////////////
}
