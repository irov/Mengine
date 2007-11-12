#	pragma once

#	include "ResourceReference.h"

#	include <map>

class EmitterContainerInterface;
class FileDataInterface;
class RenderImageInterface;

namespace Menge
{
	class ResourceEmitterContainer
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitterContainer )

	public:
		ResourceEmitterContainer( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:

		const EmitterContainerInterface * getContainer() const;

		RenderImageInterface * getRenderImage( const std::string & _textureName );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string m_filename;
		std::string m_texturesPath;

		EmitterContainerInterface * m_container;

		typedef std::map<std::string, RenderImageInterface * > TMapImageEmitters;
		TMapImageEmitters	m_emitterTextures;
	};
}