#	pragma once

#	include "ResourceReference.h"
#	include "Utils/Core/Polygon.h"
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
		ResourceEmitterContainer();

	public:
		void setFilePath( const WString& _path );
		const WString& getFilePath() const;

		void setFolderPath( const WString& _path );
		const WString& getFolderPath() const;
		
	public:
		EmitterContainerInterface * getContainer() const;

	public:
		ResourceImageDefault* getAtlasImage( size_t _atlasId );

	public:
		void loader( const Metabuf::Metadata * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void createResource_( const ConstString & _fullname, const WString & _path );

	private:
		WString m_filename;
		WString m_folder;
		
		EmitterContainerInterface * m_container;

		typedef std::vector<ResourceImageDefault *> TVectorAtlasImages;
		TVectorAtlasImages m_atlasImages;
	};
}
