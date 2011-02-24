#	pragma once

#	include "ResourceReference.h"

#	include <map>

namespace Menge
{
	class EmitterContainerInterface;
	class ResourceImageDefault;

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
		ResourceEmitterContainer();

	public:
		void loader( XmlElement * _xml ) override;
		


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
		ResourceImageDefault* getRenderImage( const String& _name );
		bool releaseRenderImage( ResourceImageDefault * _resourceImage );

		const String& getFilePath() const;
		
		const String& getFolderPath() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		String m_filename;
		String m_folder;

		void setFilePath( const String& _path );
		void setFolderPath( const String& _path );

		EmitterContainerInterface * m_container;

		typedef std::map< String, ResourceImageDefault* > TMapImageEmitters;
		TMapImageEmitters	m_mapImageEmitters;
	};
}
