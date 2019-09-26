#include "Compilable.h"

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

        return m_compile;
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
    bool Compilable::recompile( const Lambda<> & _lambda )
    {
        if( this->isCompile() == false )
        {
            (_lambda)();

            return true;
        }

        this->_release();

        (_lambda)();

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
