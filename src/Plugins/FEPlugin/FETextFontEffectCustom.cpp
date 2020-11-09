#include "FETextFontEffectCustom.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectCustom::FETextFontEffectCustom()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FETextFontEffectCustom::~FETextFontEffectCustom()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FETextFontEffectCustom::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_memory );

        const char * memory_buffer = m_memory->getBuffer();
        size_t memory_size = m_memory->getSize();

        fe_bundle * bundle = fe_bundle_load( memory_buffer, (int32_t)memory_size );

        bool successful = this->compileFEBundle( bundle );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void FETextFontEffectCustom::_release()
    {
        m_memory = nullptr;
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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
