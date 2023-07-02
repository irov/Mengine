#include "DazzleEffectPrototypeGenerator.h"

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleEffectPrototypeGenerator::DazzleEffectPrototypeGenerator()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzleEffectPrototypeGenerator::~DazzleEffectPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffectPrototypeGenerator::setDazzleService( const dz_service_t * _service )
    {
        m_service = _service;
    }
    //////////////////////////////////////////////////////////////////////////
    const dz_service_t * DazzleEffectPrototypeGenerator::getDazzleService() const
    {
        return m_service;
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer DazzleEffectPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        DazzleEffectPtr dazzleEffect = NodePrototypeGenerator<DazzleEffect, 32>::generate( _doc );

        dazzleEffect->setDazzleService( m_service );

        return dazzleEffect;
    }
    //////////////////////////////////////////////////////////////////////////
}
