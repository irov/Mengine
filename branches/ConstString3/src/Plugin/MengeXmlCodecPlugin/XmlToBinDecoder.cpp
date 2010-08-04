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
		typedef void(WINAPI *PFN_Initialize)(char *);
		typedef void(WINAPI *PFN_Convert)(char *, char *);

		HINSTANCE hMyDll = ::LoadLibraryA("d:\\Projects\\AquaHOG\\Bin\\Xml2Bin.dll");

		PFN_Initialize p_Initialize = (PFN_Initialize)::GetProcAddress(hMyDll, "initialize");
		PFN_Convert p_Convert = (PFN_Convert)::GetProcAddress(hMyDll, "convert");

		p_Initialize( (char *)m_options.protocol.c_str() );
		p_Convert( (char*)m_options.pathXml.c_str(), (char*)m_options.pathBin.c_str() );

		::FreeLibrary( hMyDll );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::release()
	{

	}

}