#include "FETextFontEffectCustom.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectCustom::FETextFontEffectCustom()
        : m_bundle( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectCustom::~FETextFontEffectCustom()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectCustom::setFECustom( const MemoryInterfacePtr & _memory )
    {
        m_memory = _memory;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & FETextFontEffectCustom::getFECustom() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectCustom::isValid() const
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectCustom::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_memory );

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
    void FETextFontEffectCustom::_release()
    {
        fe_bundle_free( m_bundle );
        m_bundle = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
