#include "FEFontEffectCustom.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEFontEffectCustom::FEFontEffectCustom()
        : m_bundle( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEFontEffectCustom::~FEFontEffectCustom()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FEFontEffectCustom::setFECustom( const MemoryInterfacePtr & _memory )
    {
        m_memory = _memory;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & FEFontEffectCustom::getFECustom() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectCustom::isValid() const
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEFontEffectCustom::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_memory, "invalid memory" );

        const Char * memory_buffer = m_memory->getBuffer();
        size_t memory_size = m_memory->getSize();

        fe_bundle * bundle = fe_bundle_load( memory_buffer, (int32_t)memory_size );

        if( this->compileFEBundle( bundle ) == false )
        {
            fe_bundle_free( bundle );

            return false;
        }

        m_bundle = bundle;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEFontEffectCustom::_release()
    {
        fe_bundle_free( m_bundle );
        m_bundle = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
