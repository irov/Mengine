#	include "XmlToBinDecoder.h"

#	include <windows.h>

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

		typedef bool (*PFN_Binary)( const char *, const char *, const char *, int);
		PFN_Binary p_Bynary = (PFN_Binary)::GetProcAddress(hMyDll, "writeBinary");

		if( p_Bynary( m_options.protocol.c_str(), m_options.pathXml.c_str(), m_options.pathBin.c_str(), m_options.version ) == false )
		{
//			printf("11");
		}

		::FreeLibrary( hMyDll );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::release()
	{

	}

}