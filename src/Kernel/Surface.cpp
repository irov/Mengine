#include "Surface.h"

#include "Interface/UpdationInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Surface::Surface()
        : m_revision( 0 )
        , m_anchor( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Surface::~Surface()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::setAnchor( const mt::vec2f & _anchor )
    {
        m_anchor = _anchor;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Surface::getAnchor() const
    {
        return m_anchor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Surface::initialize()
    {
        bool successful = this->_initialize();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_activationReferenceCount.getReferenceCount() == 0, "surface '%s' invalid finalize with activation reference count '%u'"
            , this->getName().c_str()
            , m_activationReferenceCount.getReferenceCount()
        );

        this->_finalize();

        Materialable::releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Surface::_initialize()
    {
        //Empty;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_finalize()
    {
        //Empty;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::activate()
    {
        uint32_t referenceCount = m_activationReferenceCount.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_activate();

        UpdationInterface * updation = this->getSurfaceUpdation();

        if( updation != nullptr )
        {
            EUpdateMode mode = updation->getUpdationMode();

            if( mode != EUM_SURFACE )
            {
                MENGINE_ASSERTION_FATAL( mode == EUM_UNKNOWN, "surface '%s' invalid updation mode '%u'"
                    , this->getName().c_str()
                    , static_cast<uint32_t>(mode)
                );

                updation->activate( EUM_SURFACE, MENGINE_UINT32_C(0) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::deactivate()
    {
        uint32_t referenceCount = m_activationReferenceCount.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        UpdationInterface * updation = this->getSurfaceUpdation();

        if( updation != nullptr )
        {
            EUpdateMode mode = updation->getUpdationMode();

            if( mode != EUM_UNKNOWN )
            {
                MENGINE_ASSERTION_FATAL( mode == EUM_SURFACE, "surface '%s' invalid updation mode '%u'"
                    , this->getName().c_str()
                    , static_cast<uint32_t>(mode)
                );

                updation->deactivate();
            }
        }

        this->_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Surface::getActivationReferenceCount() const
    {
        uint32_t referenceCount = m_activationReferenceCount.getReferenceCount();

        return referenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_activate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_deactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    UpdationInterface * Surface::getSurfaceUpdation()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Surface::getRevision() const
    {
        return m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    void Surface::_invalidateMaterial() const
    {
        ++m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
}
