#	pragma once

#	include "Interface/GraveyardInterface.h"

#	include "Core/IntrusiveDuplexTree.h"

namespace Menge
{
	struct RenderTextureGraveEntry
		: public Factorable
		, public stdex::intrusive_duplex_tree_node<RenderTextureGraveEntry>
	{
		typedef ConstString key_first_type;
		typedef ConstString::less_type less_first_type;

		typedef FilePath key_second_type;
		typedef FilePath::less_type less_second_type;

		struct key_first_getter_type
		{
			const ConstString & operator()( const RenderTextureGraveEntry * _node ) const
			{
				return _node->category;
			}
		};

		struct key_second_getter_type
		{
			const FilePath & operator()( const RenderTextureGraveEntry * _node ) const
			{
				return _node->filePath;
			}
		};

		ConstString category;
		FilePath filePath;

		RenderImageInterfacePtr image;
		uint32_t mipmaps;
		uint32_t width;
		uint32_t height;
		uint32_t channels;
		float timing;
	};

	class Graveyard
		: public GraveyardInterface
	{
	public:
		Graveyard();
		~Graveyard();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void update( float _timing ) override;

	public:
		void clearTextures() override;

	public:
		void buryTexture( RenderTextureInterface * _texture ) override;
		RenderTextureInterfacePtr resurrectTexture( const ConstString& _pakName, const FilePath & _filePath ) override;

	protected:
		void updateTexture_( RenderTextureGraveEntry * _entry, float _timing );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		float m_graveyardTime;

		typedef IntrusiveDuplexTree<RenderTextureGraveEntry, 16> TMapTextureGrave;
		TMapTextureGrave m_textures;
	};
}