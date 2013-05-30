#	include "StringizeService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StringizeService, Menge::StringizeServiceInterface, Menge::StringizeService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{	
    //////////////////////////////////////////////////////////////////////////
    StringizeService::StringizeService()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringizeService::~StringizeService()
    { 
    }
    //////////////////////////////////////////////////////////////////////////
    void StringizeService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * StringizeService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool StringizeService::stringize( const char * _str, size_t _size, ConstString & _cstr )
	{
        ConstStringHolderT<char> * stringHolder = nullptr;
        
        if( _size < 16 )
        {
            ConstStringHolderString16 * string = m_poolString16.createObjectT();
            string->setValue( _str, _size );

            stringHolder = string;
        }
        else if( _size < 32 )
        {
            ConstStringHolderString32 * string = m_poolString32.createObjectT();
            string->setValue( _str, _size );

            stringHolder = string;
        }
        else if( _size < 64 )
        {
            ConstStringHolderString64 * string = m_poolString64.createObjectT();
            string->setValue( _str, _size );

            stringHolder = string;
        }
        else if( _size < 128 )
        {
            ConstStringHolderString128 * string = m_poolString128.createObjectT();
            string->setValue( _str, _size );

            stringHolder = string;
        }
        else if( _size < 256 )
        {
            ConstStringHolderString256 * string = m_poolString256.createObjectT();
            string->setValue( _str, _size );

            stringHolder = string;
        }
        else
        {
            ConstStringHolderStringSTL * stringSTL = m_poolStringSTL.createObjectT();
            stringSTL->setValue( _str, _size );

            stringHolder = stringSTL;
        }

        _cstr = ConstString(stringHolder);

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StringizeService::stringizeW( const wchar_t * _str, size_t _size, ConstWString & _cstr )
    {
        ConstStringHolderWStringSTL * wstringSTL = m_poolWStringSTL.createObjectT();
        wstringSTL->setValue( _str, _size );

        _cstr = ConstWString(wstringSTL);

        return true;
    }
}
