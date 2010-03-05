#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Utils/Factory/FactoryManager.h"

namespace Menge
{
	class CodecSystem
		: public CodecSystemInterface
	{
	public:
		bool initialize() override;

	public:
		DecoderInterface * createDecoder( const String& _filename, const String& _type, FileInputInterface * _file ) override;
		void releaseDecoder( DecoderInterface * _decoder ) override;

		EncoderInterface * createEncoder( const String& _filename, const String& _type, FileOutputInterface * _file ) override;
		virtual void releaseEncoder( EncoderInterface * _encoder ) override;

	protected:
		FactoryManager m_decoderFactory;
		FactoryManager m_encoderFactory;
	};
}