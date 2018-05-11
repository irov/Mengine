#pragma once

#include "Interface/MemoryInterface.h"

#include "ResourceTestPick.h"

namespace Mengine
{
	class ResourceHIT
		: public ResourceTestPick
	{
		DECLARE_VISITABLE( ResourceTestPick );

	public:
		ResourceHIT();

    public:
        void setCodecType( const ConstString & _codec );
        const ConstString & getCodecType() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
        bool _isValid() const override;

	public:
		void setPath( const FilePath & _filePath );

	public:
		bool testPoint( const mt::vec2f & _point, float _minAlpha ) const override;
		bool testRadius( const mt::vec2f & _point, float _radius, float _minAlpha ) const override;

	public:
		uint32_t getWidth() const override;
        uint32_t getHeight() const override;

    public:
		uint8_t * getBuffer() const override;

    protected:
		uint8_t * getHitBuffer_( uint32_t _level ) const;

    protected:
		FilePath m_filePath;
        ConstString m_codecType;
        ConstString m_converterType;

		uint32_t m_width;
		uint32_t m_height;
        uint32_t m_mipmaplevel;

		MemoryInterfacePtr m_mipmap;
		uint32_t m_mipmapsize;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ResourceHIT> ResourceHITPtr;
}	
