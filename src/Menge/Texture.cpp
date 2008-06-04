
#	include "Texture.h"
#	include "HardwarePixelBuffer.h"
#	include "LogEngine.h"
#	include "Interface/TextureInterface.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Texture::Texture( const String& _name )
		: m_name( _name )
		, m_height( 512 )
		, m_width( 512 )
		, m_depth( 1 )
		, m_numRequestedMipmaps( 0 )
		, m_numMipmaps( 0 )
		, m_mipmapsHardwareGenerated( false )
		, m_gamma( 1.0f )
		//, m_textureType( TEX_TYPE_2D )
		, m_format( PF_UNKNOWN )
		, m_usage( TU_DEFAULT )
		, m_srcFormat( PF_UNKNOWN )
		, m_srcWidth( 0 )
		, m_srcHeight( 0 )
		, m_srcDepth( 0 )
		, m_desiredFormat( PF_UNKNOWN )
		, m_desiredIntegerBitDepth( 0 )
		, m_desiredFloatBitDepth( 0 )
		, m_treatLuminanceAsAlpha( false )
		, m_internalResourcesCreated( false )
		, m_restoring( false )
	{
		// TextureManager* tMgr = Holder<TextureManager>::hostage();
		// setNumMipmaps( tMgr->getDefaultNumMipmaps() );
		// setDesiredBitDepths( tMgr->getPreferredIntegerBitDepth(), tMgr->getPreferredFloatBitDepth() );
	}
	//////////////////////////////////////////////////////////////////////////
	Texture::~Texture()
	{
		//m_interface;

		if ( m_loadingState == LOADSTATE_LOADED )
		{
			unload(); 
		}
		else
		{
			freeInternalResources();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::loadRawData( DataStreamInterface* _stream, unsigned short _width, unsigned short _height, PixelFormat _format )
	{
		Image img;
		img.loadRawData( _stream, _width, _height, _format );
		loadImage( img );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::loadImage( const Image& _img )
	{
		// Scope lock over load status
		{
		//	OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				if( m_loadingState != LOADSTATE_UNLOADED )
				{
					// no loading to be done
					return;
				}
				m_loadingState = LOADSTATE_LOADING;
		}

		// Scope lock for actual loading
		try
		{
//			OGRE_LOCK_AUTO_MUTEX
				std::vector<const Image*> imagePtrs;
			imagePtrs.push_back( &_img );
			loadImages_( imagePtrs );

		}
		catch (...)
		{
			// Reset loading in-progress flag in case failed for some reason
//			OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				m_loadingState = LOADSTATE_UNLOADED;
			// Re-throw
			throw;
		}

		// Scope lock for loading progress
		{
//			OGRE_LOCK_MUTEX(mLoadingStatusMutex)

				// Now loaded
				m_loadingState = LOADSTATE_LOADED;
		}

		// Notify manager
		//if(mCreator)
		//	mCreator->_notifyResourceLoaded(this);

		// No deferred loading events since this method is not called in background
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::setFormat( PixelFormat _pf )
	{
		m_format = _pf;
		m_desiredFormat = _pf;
		m_srcFormat = _pf;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Texture::hasAlpha() const
	{
		return PixelUtil::hasAlpha( m_format );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::setDesiredIntegerBitDepth( unsigned short _bits )
	{
		m_desiredIntegerBitDepth = _bits;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned short Texture::getDesiredIntegerBitDepth() const
	{
		return m_desiredIntegerBitDepth;
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::setDesiredFloatBitDepth( unsigned short _bits )
	{
		m_desiredFloatBitDepth = _bits;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned short Texture::getDesiredFloatBitDepth() const
	{
		return m_desiredFloatBitDepth;
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::setDesiredBitDepths( unsigned short _integerBits, unsigned short _floatBits )
	{
		m_desiredIntegerBitDepth = _integerBits;
		m_desiredFloatBitDepth = _floatBits;
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::setTreatLuminanceAsAlpha( bool _asAlpha )
	{
		m_treatLuminanceAsAlpha = _asAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Texture::getTreatLuminanceAsAlpha() const
	{
		return m_treatLuminanceAsAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Texture::calculateSize_() const
	{
		return getNumFaces() * PixelUtil::getMemorySize( m_width, m_height, m_depth, m_format );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Texture::getNumFaces() const
	{
		return getTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::loadImages_( const TConstImageVector& _images )
	{
		assert( !_images.empty() && "Texture::loadImages -> Cannot load empty vector of images" );

		// Set desired texture size and properties from _images[0]
		m_srcWidth = m_width = _images[0]->getWidth();
		m_srcHeight = m_height = _images[0]->getHeight();
		m_srcDepth = m_depth = _images[0]->getDepth();

		// Get source image format and adjust if required
		m_srcFormat = _images[0]->getFormat();
		if( m_treatLuminanceAsAlpha && m_srcFormat == PF_L8 )
		{
			m_srcFormat = PF_A8;
		}

		if( m_desiredFormat != PF_UNKNOWN )
		{
			// If have desired format, use it
			m_format = m_desiredFormat;
		}
		else
		{
			// Get the format according with desired bit depth
			m_format = PixelUtil::getFormatForBitDepths( m_srcFormat, m_desiredIntegerBitDepth, m_desiredFloatBitDepth );
		}

		// The custom mipmaps in the image have priority over everything
		std::size_t imageMips = _images[0]->getNumMipmaps();

		if( imageMips > 0 ) 
		{
			m_numMipmaps = m_numRequestedMipmaps = _images[0]->getNumMipmaps();
			// Disable flag for auto mip generation
			m_usage &= ~TU_AUTOMIPMAP;
		}

		// Create the texture
		createInternalResources();
		// Check if we're loading one image with multiple faces
		// or a vector of images representing the faces
		std::size_t faces;
		bool multiImage; // Load from multiple images?
		if( !_images.empty() )
		{
			faces = _images.size();
			multiImage = true;
		}
		else
		{
			faces = _images[0]->getNumFaces();
			multiImage = false;
		}

		// Check wether number of faces in images exceeds number of faces
		// in this texture. If so, clamp it.
		if( faces > getNumFaces() )
		{
			faces = getNumFaces();
		}

		// Say what we're doing
		std::ostringstream str;
		str << "Texture: " << m_name << ": Loading " << faces << " faces"
			<< "(" << PixelUtil::getFormatName( _images[0]->getFormat() ) << "," <<
			_images[0]->getWidth() << "x" << _images[0]->getHeight() << "x" << _images[0]->getDepth() <<
			") with ";
		if ( !( m_mipmapsHardwareGenerated && m_numMipmaps == 0 ) )
		{
			str << m_numMipmaps;
		}
		if( m_usage & TU_AUTOMIPMAP )
		{
			if( m_mipmapsHardwareGenerated )
			{
				str << " hardware";
			}
			str << " generated mipmaps";
		}
		else
		{
			str << " custom mipmaps";
		}
		if( multiImage )
		{
			str << " from multiple Images.";
		}
		else
		{
			str << " from Image.";
		}
		// Scoped
		{
			// Print data about first destination surface
			HardwarePixelBufferInterface* buf = getBuffer( 0, 0 ); 
			str << " Internal format is " << PixelUtil::getFormatName( static_cast<PixelFormat>( buf->getFormat() ) ) << 
				"," << buf->getWidth() << "x" << buf->getHeight() << "x" << buf->getDepth() << ".";
		}
		MENGE_LOG( str.str().c_str() );

		// Main loading loop
		// imageMips == 0 if the image has no custom mipmaps, otherwise contains the number of custom mips
		for( std::size_t mip = 0; mip <= imageMips; ++mip )
		{
			for( std::size_t i = 0; i < faces; ++i )
			{
				PixelBox src;
				if( multiImage )
				{
					// Load from multiple images
					src = _images[i]->getPixelBox( 0, mip );
				}
				else
				{
					// Load from faces of images[0]
					src = _images[0]->getPixelBox( i, mip );
				}

				// Sets to treated format in case is difference
				src.format = m_srcFormat;

				if( m_gamma != 1.0f )
				{
					// Apply gamma correction
					// Do not overwrite original image but do gamma correction in temporary buffer
					//MemoryDataStreamPtr buf; // for scoped deletion of conversion buffer
					//buf.bind(new MemoryDataStream(
					//	PixelUtil::getMemorySize(
					//	src.getWidth(), src.getHeight(), src.getDepth(), src.format)));
					unsigned char* buf = new unsigned char[PixelUtil::getMemorySize( src.getWidth(), src.getHeight(), src.getDepth(), src.format )];

					PixelBox corrected = PixelBox(src.getWidth(), src.getHeight(), src.getDepth(), src.format, buf );
					PixelUtil::bulkPixelConversion( src, corrected );

					Image::applyGamma( static_cast<uint8*>( corrected.data ), m_gamma, corrected.getConsecutiveSize(), 
						static_cast<unsigned char>( PixelUtil::getNumElemBits( src.format ) ) );

					// Destination: entire texture. blitFromMemory does the scaling to
					// a power of two for us when needed
					getBuffer( i, mip )->blitFromMemory( corrected );
					delete[] buf;
				}
				else 
				{
					// Destination: entire texture. blitFromMemory does the scaling to
					// a power of two for us when needed
					getBuffer( i, mip )->blitFromMemory( src );
				}

			}
		}
		// Update size (the final size, not including temp space)
		m_size = getNumFaces() * PixelUtil::getMemorySize( m_width, m_height, m_depth, m_format );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::createInternalResources()
	{
		if( !m_internalResourcesCreated )
		{
			if( !m_restoring )
			{
				m_interface->createInternalResourcesImpl( m_textureType, m_usage, PixelUtil::hasAlpha( m_format ),
					m_srcWidth, m_srcHeight );
				// resolving hardware supporting format
				m_format = static_cast<PixelFormat>( m_interface->getPixelFormat() );
			}
			m_internalResourcesCreated = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::freeInternalResources()
	{
		if( m_internalResourcesCreated )
		{
			m_interface->freeInternalResourcesImpl();
			m_internalResourcesCreated = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::unloadImpl()
	{
		freeInternalResources();
	}
	//////////////////////////////////////////////////////////////////////////   
	void Texture::copyToTexture( Texture* _target )
	{
		assert( (_target->getNumFaces() == getNumFaces()) && "Texture::copyToTexture -> Texture types must match");

		std::size_t numMips = (std::min)( getNumMipmaps(), _target->getNumMipmaps() );
		if( ( m_usage & TU_AUTOMIPMAP ) || ( _target->getUsage() & TU_AUTOMIPMAP ) )
		{
			numMips = 0;
		}
		for( unsigned int face = 0; face < getNumFaces(); face++ )
		{
			for( unsigned int mip = 0; mip <= numMips; mip++ )
			{
				_target->getBuffer( face, mip )->blit( getBuffer( face, mip ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HardwarePixelBufferInterface* Texture::getBuffer( std::size_t face /*= 0*/, std::size_t _mipmap /*= 0 */ )
	{
		assert( ( face < getNumFaces() ) && "Texture::getBuffer -> A three dimensional cube has six faces" );
		assert( ( _mipmap <= getNumMipmaps() ) && "Texture::getBuffer -> Mipmap index out of range" );
		int idx = face * ( m_numMipmaps + 1 ) + _mipmap;

		return m_interface->getBuffer( idx );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::loadImage3D( const Image _imgs[] )
	{
		std::vector<const Image*> imagePtrs;
		imagePtrs.push_back(&_imgs[0]);
		imagePtrs.push_back(&_imgs[1]);
		imagePtrs.push_back(&_imgs[2]);
		imagePtrs.push_back(&_imgs[3]);
		imagePtrs.push_back(&_imgs[4]);
		imagePtrs.push_back(&_imgs[5]);
		loadImages_( imagePtrs );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::restoreFromLostDevice()
	{
		if( ( m_loadingState == LOADSTATE_LOADED ) && !( m_usage & TU_RENDERTARGET ) )
		{
			m_restoring = true;
			// Mark as unloaded even though we're not so we can reload content
			//mIsLoaded = false;
			m_loadingState = ELoadingState::LOADSTATE_UNLOADED;
			load();
			m_restoring = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::load()
	{
		// Early-out without lock (mitigate perf cost of ensuring loaded)
		// Don't load if:
		// 1. We're already loaded
		// 2. Another thread is loading right now
		// 3. We're marked for background loading and this is not the background
		//    loading thread we're being called by
		if ( m_loadingState != LOADSTATE_UNLOADED /*|| (mIsBackgroundLoaded && !background)*/ )
		{
			return;
		}

		// Scope lock over load status
		{
			//OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				// Check again just in case status changed (since we didn't lock above)
				if( m_loadingState != LOADSTATE_UNLOADED /*|| (mIsBackgroundLoaded && !background)*/)
				{
					// no loading to be done
					return;
				}
				m_loadingState = LOADSTATE_LOADING;
		}

		// Scope lock for actual loading
		try
		{

			/*OGRE_LOCK_AUTO_MUTEX
				preLoadImpl();

			if (mIsManual)
			{
				// Load from manual loader
				if (mLoader)
				{
					mLoader->loadResource(this);
				}
				else
				{
					// Warn that this resource is not reloadable
					LogManager::getSingleton().logMessage(
						"WARNING: " + mCreator->getResourceType() + 
						" instance '" + mName + "' was defined as manually "
						"loaded, but no manual loader was provided. This Resource "
						"will be lost if it has to be reloaded.");
				}
			}
			else*/
			{
				/*if (mGroup == ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)
				{
					// Derive resource group
					changeGroupOwnership(
						ResourceGroupManager::getSingleton()
						.findGroupContainingResource(mName));
				}*/
				loadImpl();
			}
			// Calculate resource size
			m_size = calculateSize_();

			//postLoadImpl();
		}
		catch (...)
		{
			// Reset loading in-progress flag in case failed for some reason
			//OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				m_loadingState = LOADSTATE_UNLOADED;
			// Re-throw
			throw;
		}

		// Scope lock for loading progress
		{
			//OGRE_LOCK_MUTEX(mLoadingStatusMutex)

				// Now loaded
				m_loadingState = LOADSTATE_LOADED;
		}

		// Notify manager
	//	if(mCreator)
	//		mCreator->_notifyResourceLoaded(this);

		// Fire (deferred) events
	//	if (mIsBackgroundLoaded)
	//		queueFireBackgroundLoadingComplete();
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::loadImpl()
	{
		if( m_textureType == TEX_TYPE_CUBE_MAP )
		{
			constructCubeFaceNames_( m_name );
			Image imgs[6];
			for ( int face = 0; face < 6; ++face )
			{
				imgs[face].load( m_cubeFaceNames[face] );
			}
			loadImage3D( imgs );
		}
		else if( m_textureType == TEX_TYPE_2D )
		{
			Image img;
			img.load( m_name );

			loadImage( img );
		}
		else
		{
			assert( 0 && "Texture::loadImpl -> Unknown texture type" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::constructCubeFaceNames_( const String& _name )
	{
		// the suffixes
		String suffixes[6] = {"_rt", "_lf", "_up", "_dn", "_fr", "_bk"};
		std::size_t pos = -1;

		String ext; // the extension
		String baseName; // the base name
		String fakeName = _name; // the 'fake' name, temp. holder

		// first find the base name
		pos = fakeName.find_last_of(".");
		assert( ( pos != -1 ) && "Texture::_constructCubeFaceNames -> Invalid cube texture base name" );

		baseName = fakeName.substr(0, pos);
		ext = fakeName.substr(pos);

		// construct the full 6 faces file names from the baseName, suffixes and extension
		for( std::size_t i = 0; i < 6; ++i )
		{
			m_cubeFaceNames[i] = baseName + suffixes[i] + ext;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::unload() 
	{ 
		// Early-out without lock (mitigate perf cost of ensuring unloaded)
		if( m_loadingState != LOADSTATE_LOADED )
		{
			return;
		}

		// Scope lock for loading status
		{
			//OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				if ( m_loadingState == LOADSTATE_LOADING )
				{
					assert( String( "Resource::unload -> Cannot unload resource " + m_name + " whilst loading is in progress!" ).c_str() );
				}
				if( m_loadingState != LOADSTATE_LOADED)
				{
					return; // nothing to do
				}

				m_loadingState = LOADSTATE_UNLOADING;
		}

		// Scope lock for actual unload
		{
			//OGRE_LOCK_AUTO_MUTEX
			//	preUnloadImpl();
			unloadImpl();
			//postUnloadImpl();
		}

		// Scope lock for loading status
		{
			//OGRE_LOCK_MUTEX(mLoadingStatusMutex)
				m_loadingState = LOADSTATE_UNLOADED;
		}

		// Notify manager
		//if(mCreator)
		//	mCreator->_notifyResourceUnloaded(this);

	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge