#	pragma once

#	include "ResourceReference.h"

#	include <map>

class EmitterContainerInterface;
class FileDataInterface;
class RenderImageInterface;

namespace Menge
{
	//! ResourceEmitterContainer - ������-���� ���������� ���������, ������� ����������� �� ������� *.ptc �� Astralax. ptc �������� ������ ����� �������, ������� ���������� �������� � ����� ���������� ��� ���������.

    /*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceEmitterContainer" >
	*	<File Path = "���_�����"/>
	*	<Folder Path = "�����_�_����������_���������"/>
	* </Resource>
	*/

	class ResourceEmitterContainer
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitterContainer )

	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceEmitterContainer( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:

		//! ���������� ��������� ���������.
		/*!
		\return ��������� ���������
		*/
		const EmitterContainerInterface * getContainer() const;

		//! ���������� �������� �� �����.
		/*!
		\param _name ��� ��������
		\return �����������
		*/
		RenderImageInterface * getRenderImage( const std::string & _name );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string m_filename;
		std::string m_folder;

		EmitterContainerInterface * m_container;

		typedef std::map<std::string, RenderImageInterface * > TMapImageEmitters;
		TMapImageEmitters	m_mapImageEmitters;
	};
}