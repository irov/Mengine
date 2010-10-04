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
		DecoderInterface * createDecoder( const String& _filename, CodecType _type, FileInputInterface * _file ) override;
		void releaseDecoder( DecoderInterface * _decoder ) override;

		EncoderInterface * createEncoder( const String& _filename, CodecType _type, FileOutputInterface * _file ) override;
		virtual void releaseEncoder( EncoderInterface * _encoder ) override;

	protected:
		FactoryManager m_decoderFactory[ECT_MAX];
		FactoryManager m_encoderFactory[ECT_MAX];
	};
}