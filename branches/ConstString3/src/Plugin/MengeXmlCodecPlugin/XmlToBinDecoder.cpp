#	include "XmlToBinDecoder.h"

#	include <Windows.h>

#	include "Utils/Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Xml2BinDecoder::Xml2BinDecoder( InputStreamInterface * _stream )
		: m_stream(_stream)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<XmlCodecOptions *>(_options);
	}
	//////////////////////////////////////////////////////////////////////////
	const XmlCodecDataInfo* Xml2BinDecoder::getCodecDataInfo() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface * Xml2BinDecoder::getStream() const
	{
		return m_stream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Xml2BinDecoder::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Xml2BinDecoder::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		HINSTANCE hMyDll = ::LoadLibraryA("xml2bin.dll");

		typedef bool (*PFN_Header)( const char *, const char *);
		PFN_Header p_Header = (PFN_Header)::GetProcAddress(hMyDll, "writeHeader");
		p_Header( m_options.protocol.c_str(), "BinProtocol.h" );

		typedef bool (*PFN_Binary)( const char *, const char *, const char *, int, char *);
		PFN_Binary p_Bynary = (PFN_Binary)::GetProcAddress(hMyDll, "writeBinary");

		//if( p_Header( m_options.protocol.c_str(), "BinProtocol.h" ) == false )
		//{

		//}

		char error[256];
		if( p_Bynary( m_options.protocol.c_str(), m_options.pathXml.c_str(), m_options.pathBin.c_str(), m_options.version, error ) == false )
		{
			MENGE_LOG_ERROR( "Error: can't parse sample '%s' '%s' '%s' '%d'"
				, m_options.protocol.c_str()
				, m_options.pathXml.c_str()
				, m_options.pathBin.c_str()
				, m_options.version
				);

			MENGE_LOG_ERROR( "'%s'"
				, error
				);

			return 0;
		}

		::FreeLibrary( hMyDll );

		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::destroy()
	{
		delete this;
	}

}