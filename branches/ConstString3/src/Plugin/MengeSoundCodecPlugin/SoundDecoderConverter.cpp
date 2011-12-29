#	include "SoundDecoderConverter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderConverter::SoundDecoderConverter( CodecServiceInterface * _service, InputStreamInterface * _stream, FileServiceInterface * _fileService )
		: m_service(_service)
		, m_stream(_stream)
		, m_fileService(_fileService)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundDecoderConverter::~SoundDecoderConverter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const SoundCodecDataInfo * SoundDecoderConverter::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderConverter::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<SoundCodecOptions*>(_options);
		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderConverter::_invalidate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundDecoderConverter::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface * SoundDecoderConverter::getStream() const
	{
		return m_stream;	
	}
}
