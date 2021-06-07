#include "Compilable.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Compilable::Compilable()
        : m_compile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Compilable::~Compilable()
    {
        MENGINE_ASSERTION_FATAL( m_compile == false );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Compilable::compile()
    {
        if( m_compile == true )
        {
            return true;
        }

        if( this->_compile() == false )
        {
            return false;
        }

        m_compile = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Compilable::release()
    {
        if( m_compile == false )
        {
            return;
        }

        m_compile = false;

        this->_release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Compilable::_compile()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Compilable::_release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Compilable::recompile( const LambdaRecompile & _lambda )
    {
        if( this->isCompile() == false )
        {
            bool tocompile = (_lambda)();
            MENGINE_UNUSED( tocompile );

            return true;
        }

        this->_release();

        bool tocompile = (_lambda)();

        if( tocompile == false )
        {
            m_compile = false;

            return true;
        }

        if( this->_compile() == false )
        {
            m_compile = false;

            this->_uncompile();

            return false;
        }

        this->_recompile();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Compilable::_recompile()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Compilable::_uncompile()
    {
        //Empty
    }
}
