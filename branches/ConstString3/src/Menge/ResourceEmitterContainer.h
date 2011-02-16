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
		ResourceEmitterContainer();

	public:
		void setFilePath( const ConstString& _path );
		const ConstString& getFilePath() const;

		void setFolderPath( const ConstString& _path );
		const ConstString& getFolderPath() const;
	
	public:
		const EmitterContainerInterface * getContainer() const;
		ResourceImageDefault* getRenderImage( const char * _name );

	public:
		void loader( BinParser * _parser ) override;
	
	protected:
		bool _compile() override;
		void _release() override;

	private:
		ConstString m_filename;
		ConstString m_folder;

		EmitterContainerInterface * m_container;

		struct ConstCharLess
			: public std::binary_function<const char *, const char *, bool>
		{
			bool operator () ( const char * _left, const char * _right ) const
			{
				return strcmp( _left, _right ) < 0;
			}
		};

		typedef std::map<const char *, ResourceImageDefault *, ConstCharLess> TMapImageEmitters;
		TMapImageEmitters m_mapImageEmitters;
	};
}
