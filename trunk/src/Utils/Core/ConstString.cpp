#   include "Config/Typedef.h"

#	include "ConstString.h"

#   include "Factory/Factory.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class ConstStringHolderNone
            : public ConstStringHolder
            , public Factory
        {
        public:
            ConstStringHolderNone()
                : ConstStringHolder()
            {
                this->setup("", 0);

                this->setFactory( this );
            }
            
        protected:
            Factorable * _createObject() override
            {
                return NULL;
            }

        protected:
            void _destroyObject( Factorable * _object ) override
            {
                (void)_object;
                //delete _object;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ConstString::none()
    {
        static ConstString s_none( new Detail::ConstStringHolderNone() );

        return s_none;
    }
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString()
        : m_holder(ConstString::none().m_holder->owner())
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const ConstString & _cstr )
		: m_holder(_cstr.m_holder->owner())
	{
	}
	//////////////////////////////////////////////////////////////////////////
    ConstString::ConstString( ConstStringHolder * _holder )
		: m_holder(_holder)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool operator == ( const ConstString & _left, const char * _right )
	{
		const char * left_str = _left.c_str();

		if( strcmp( left_str, _right ) != 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool operator == ( const char * _left, const ConstString & _right )
	{
		const char * right_str = _right.c_str();

		if( strcmp( _left, right_str ) != 0 )
		{
			return false;
		}

		return true;
	}
}