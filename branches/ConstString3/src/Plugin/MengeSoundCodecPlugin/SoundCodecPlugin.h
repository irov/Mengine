#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/ConverterInterface.h"

namespace Menge
{
	
	class SoundConverterSystem
		: public ConverterSystemInterface
	{
	public:
		SoundConverterSystem( const ConstString & _name );

	public:
		const ConstString & getName() const;

	protected:
		void setService( ConverterServiceInterface * _service ) override;

	protected:
		ConstString m_name;
		ConverterServiceInterface * m_service;
	};


	class SoundCodecPlugin
		: public PluginInterface
	{
	public:
		SoundCodecPlugin();

	protected:
		bool initialize( ServiceProviderInterface * _provider ) override;
		void finalize() override;

	protected:
		typedef std::vector<SoundConverterSystem *> TVectorSoundConverters;
		TVectorSoundConverters m_converters;
		ConverterServiceInterface * m_converterService;		
	};
}