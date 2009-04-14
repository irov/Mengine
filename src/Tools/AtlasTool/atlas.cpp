
#	include "atlas.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageEncoderPNG.h"

#	include <sstream>
//#	include <algorithm>

#	include "Interface/FileSystemInterface.h"

static TImageFrameList g_solidFrames;
static TImageFrameList g_alphaFrames;

static size_t g_atlasSize = 0;
static size_t g_maxSquare = 0;
static size_t g_solidSquare = 0;
static size_t g_alphaSquare = 0;

static TStringVector g_outputStrings;

static size_t g_atlasCount = 0;

static unsigned char* g_atlasBuffer = NULL;

static std::string g_atlasName;

extern Menge::FileSystemInterface* g_fileSystem;
//////////////////////////////////////////////////////////////////////////
TStringVector build( const std::string& _atlasName, const TStringVector& _images, size_t _atlas_max_size, size_t _image_max_size )
{
	g_atlasSize = _atlas_max_size;
	g_maxSquare = _atlas_max_size * _atlas_max_size;
	g_atlasName = _atlasName;
	Menge::ImageDecoderInterface* imageDecoder = NULL;
	g_atlasBuffer = new unsigned char[g_maxSquare*4];
	for( TStringVector::const_iterator it = _images.begin(), it_end = _images.end();
		it != it_end;
		++it )
	{
		const std::string& fileName = (*it);
		std::string::size_type dot_pos = fileName.find_last_of('.');
		if( dot_pos == std::string::npos )
		{
			printf( "Unrecognized image filename %s\n", fileName.c_str() );
			continue;
		}
		// open file
		Menge::DataStreamInterface* imageFile = g_fileSystem->openFile( fileName );
		if( imageFile == NULL )
		{
			printf("Can't open image file %s\n", fileName.c_str() );
			continue;
		}

		if( fileName.substr( dot_pos ) == ".png" )
		{
			imageDecoder = new Menge::ImageDecoderPNG( imageFile, ".png" );
			if( imageDecoder->getCodecDataInfo() == NULL )
			{
				printf( "Invalid png file %s\n", fileName.c_str() );
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
		}
		else if( fileName.substr( dot_pos ) == ".jpg" )
		{
			imageDecoder = new Menge::ImageDecoderJPEG( imageFile, ".jpg" );
			if( imageDecoder->getCodecDataInfo() == NULL )
			{
				printf( "Invalid jpeg file %s\n", fileName.c_str() );
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
		}
		else
		{
			printf( "Can't find image decoder for file %s\n", fileName.c_str() );
			g_fileSystem->closeStream( imageFile );
			continue;
		}
		const Menge::ImageCodecDataInfo* imageInfo = static_cast<const Menge::ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
		if( imageInfo->width > _image_max_size
			|| imageInfo->height > _image_max_size )
		{
			// skip this image
			imageDecoder->release();
			g_fileSystem->closeStream( imageFile );
			continue;
		}
		TImageFrame frame;
		frame.width = imageInfo->width;
		frame.height = imageInfo->height;
		frame.imageDecoder = imageDecoder;
		frame.filename = fileName;
		bool alpha = false;
		if( imageInfo->format == Menge::PF_A8R8G8B8 
			|| imageInfo->format == Menge::PF_A8 )
		{
			alpha = true;
		}
		addFrame( frame, alpha );
	}
	// dump remaining
	std::stringstream sStream;
	std::string sCount;
	while( g_alphaFrames.empty() == false )
	{
		sStream.clear();
		sCount.clear();
		sStream << g_atlasCount;
		sStream >> sCount;
		dumpAtlas( g_alphaFrames, g_atlasName + sCount, true );
		++g_atlasCount;
	}
	while( g_solidFrames.empty() == false )
	{
		sStream.clear();
		sCount.clear();
		sStream << g_atlasCount;
		sStream >> sCount;
		dumpAtlas( g_solidFrames, g_atlasName + sCount, false );
		++g_atlasCount;
	}

	delete[] g_atlasBuffer;
	return g_outputStrings;
}
//////////////////////////////////////////////////////////////////////////
void addFrame( const TImageFrame& _frame, bool _alpha )
{
	std::string out;
	if( _alpha == true )
	{
		g_alphaFrames.push_back( _frame );
		/*g_alphaSquare += _frame.width * _frame.height;
		if( g_alphaSquare >= g_maxSquare )
		{
			std::stringstream sStream;
			sStream << g_atlasCount;
			std::string sCount;
			sStream >> sCount;
			dumpAtlas( g_alphaFrames, g_atlasName + sCount, true );
			++g_atlasCount;
		}*/
	}
	else
	{
		g_solidFrames.push_back( _frame );
		/*g_solidSquare += _frame.width * _frame.height;
		if( g_solidSquare >= g_maxSquare )
		{
			std::stringstream sStream;
			sStream << g_atlasCount;
			std::string sCount;
			sStream >> sCount;
			dumpAtlas( g_solidFrames, g_atlasName + sCount, false );
			++g_atlasCount;
		}*/
	}
	return;
}
//////////////////////////////////////////////////////////////////////////
void dumpAtlas( TImageFrameList& _frameList, const std::string& _filename, bool _alpha )
{
	if( _frameList.empty() == true )
	{
		return;
	}
	Atlas atlas( _alpha );
	int iter = 0;
	for( TImageFrameList::iterator it = _frameList.begin(), it_end = _frameList.end();
		it != it_end;
		/*++it*/ )
	{
		size_t x = 0, y = 0;
		size_t right = x + it->width;
		size_t bottom = y + it->height;
		size_t move_x = 0;
		size_t move_y = 1;
		do
		{
			size_t nmove_y = 0;
			move_x = atlas.insertFrame( x, y, (*it), nmove_y );
			move_y = std::min( move_y, nmove_y );
			x += move_x;
			right = x + it->width;
			if( right > g_atlasSize )
			{
				y += move_y;
				bottom = y + it->height;
				x = 0;
				right = x + it->width;	
			}
		} while( (bottom <= g_atlasSize) && (move_x > 0) );

		if( bottom > g_atlasSize || right > g_atlasSize )	// atlas is full
		{
			break;
		}
		else
		{
			it = _frameList.erase( it );
		}
	}
	atlas.writeAtlas( _filename );
}
//////////////////////////////////////////////////////////////////////////
static unsigned long firstPO2From( unsigned long n )
{
	--n;            
	n |= n >> 16;
	n |= n >> 8;
	n |= n >> 4;
	n |= n >> 2;
	n |= n >> 1;
	++n;
	return n;
}
//////////////////////////////////////////////////////////////////////////
Atlas::Atlas( bool _alpha )
: m_maxWidth( 0 )
, m_maxHeight( 0 )
, m_alpha( _alpha )
{

}
//////////////////////////////////////////////////////////////////////////
size_t Atlas::insertFrame( size_t _x, size_t _y, const TImageFrame& _frame, size_t& _move_y )
{
	TAtlasFrame newFrame = { _x, _y, _x + _frame.width, _y + _frame.height, TImageFrame( _frame ) };
	for( std::vector<TAtlasFrame>::iterator it = m_frames.begin(), it_end = m_frames.end();
		it != it_end;
		++it )
	{
		TAtlasFrame frame = (*it);
		if( frame.left > 1 )
			frame.left -= 2;
		if( frame.top > 1 )
			frame.top -= 2;
		frame.right += 2;
		frame.bottom += 2;
		if( intersect( newFrame, frame ) == true )
		{
			_move_y = (frame.bottom - newFrame.top);
			return (frame.right - newFrame.left);
		}
	}
	m_maxWidth = std::max( m_maxWidth, newFrame.right );
	m_maxHeight = std::max( m_maxHeight, newFrame.bottom );
	m_frames.push_back( newFrame );
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void Atlas::writeAtlas( const std::string& _filename )
{
	Menge::OutStreamInterface* output = g_fileSystem->openOutStream( _filename + ".png", true );
	if( output == NULL )
	{
		printf( "Error: can't open file for writing '%s'\n", _filename.c_str() );
		return;
	}

	// choose width and height
	size_t atlasWidth = m_maxWidth;//firstPO2From( m_maxWidth );
	size_t atlasHeight = m_maxHeight;//firstPO2From( m_maxHeight );

	// clear image buffer
	std::fill( g_atlasBuffer, g_atlasBuffer + g_maxSquare * 4, 0x00 );

	size_t atlasPitch = g_atlasSize * 4;
	size_t numBytesPerPixel = 3;
	if( m_alpha == true )
	{
		numBytesPerPixel = 4;
	}

	unsigned int options = Menge::DF_CUSTOM_PITCH;
	options |= (atlasPitch << 16);

	for( std::vector<TAtlasFrame>::iterator it = m_frames.begin(), it_end = m_frames.end();
		it != it_end;
		++it )
	{
		TAtlasFrame& frame = (*it);
		// add info string
		std::stringstream outputLine;
		outputLine << frame.imageFrame.filename << " " << _filename << ".png ";
		outputLine << static_cast<float>( frame.left ) / atlasWidth /*+ 0.5f / atlasWidth*/ << " ";
		outputLine << static_cast<float>( frame.top ) / atlasHeight /*+ 0.5f / atlasHeight*/ << " ";
		outputLine << static_cast<float>( frame.right ) / atlasWidth/* - 0.5f / atlasWidth*/ << " ";
		outputLine << static_cast<float>( frame.bottom ) / atlasHeight/* - 0.5f / atlasHeight*/;
		g_outputStrings.push_back( outputLine.str() );

		// copy image
		unsigned char* decodePoint = g_atlasBuffer + frame.top*atlasPitch + frame.left*numBytesPerPixel;
		frame.imageFrame.imageDecoder->setOptions( options );
		frame.imageFrame.imageDecoder->decode( decodePoint, g_maxSquare*4 );
		frame.imageFrame.imageDecoder->release();

		// make border
		if( frame.left > 0 )	// copy from left
		{
			for( size_t i = 0; i < frame.imageFrame.height; ++i )
			{
				std::copy( decodePoint + i*atlasPitch, decodePoint + (i*atlasPitch+numBytesPerPixel)
							, decodePoint + (i*atlasPitch-numBytesPerPixel) );
			}
		}
		if( frame.right < atlasWidth )	// copy right
		{
			for( size_t i = 0; i < frame.imageFrame.height; ++i )
			{
				std::copy( decodePoint + (i*atlasPitch+(frame.imageFrame.width-1)*numBytesPerPixel),
							decodePoint + (i*atlasPitch+frame.imageFrame.width*numBytesPerPixel),
							decodePoint + (i*atlasPitch+frame.imageFrame.width*numBytesPerPixel) );
			}
		}
		if( frame.top > 0 ) // copy top
		{
			std::copy( decodePoint, decodePoint + frame.imageFrame.width*numBytesPerPixel, decodePoint - atlasPitch );
		}
		if( frame.bottom < atlasHeight ) // copy bottom
		{
			size_t start = (frame.imageFrame.height-1)*atlasPitch;
			std::copy( decodePoint + start, decodePoint + start + frame.imageFrame.width*numBytesPerPixel, decodePoint + start + atlasPitch );
		}
	}

	// write atlas buffer to file
	Menge::ImageEncoderInterface* imageEncoder = new Menge::ImageEncoderPNG( output, ".png" );
	imageEncoder->setOptions( options );
	Menge::ImageCodecDataInfo imageInfo;
	imageInfo.width = atlasWidth;
	imageInfo.height = atlasHeight;
	if( m_alpha == true )
	{
		imageInfo.format = Menge::PF_A8R8G8B8;
	}
	else
	{
		imageInfo.format = Menge::PF_R8G8B8;
	}
	imageEncoder->encode( g_atlasBuffer, &imageInfo );

	imageEncoder->release();

	g_fileSystem->closeOutStream( output );
}
//////////////////////////////////////////////////////////////////////////
bool Atlas::intersect( const TAtlasFrame& _frame1, const TAtlasFrame& _frame2 )
{
	return ( _frame1.bottom > _frame2.top && _frame1.top < _frame2.bottom &&
		_frame1.right > _frame2.left && _frame1.left < _frame2.right );
}
//////////////////////////////////////////////////////////////////////////
