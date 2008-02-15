#	pragma once

#	include "ResourceReference.h"

class SoundBufferInterface;

namespace Menge
{
	//! ResourceSound - ресурс-файл, который содержит звуковые данные.

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceSound" >
	*	<File Path = "имя_файла"/>
	*	<IsStreamable Value = "1/0"/>
	* </Resource>
	*/

	class ResourceSound
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSound )

	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourceSound( const std::string & _name );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		//! Возвращает звуковой буффер с данными.
		/*!
		\return звуковой буффер
		*/
		SoundBufferInterface * get();

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string	m_filename;
		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}