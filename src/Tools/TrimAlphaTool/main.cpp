/*
 *	TrimAlphaTool\main.cpp
 *
 *	Created by _Berserk_ on 15.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include <cstdio>
#	include <vector>
#	include <string>
#	include <fstream>
#	include <sstream>

#	include "Interface/FileSystemInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageEncoderPNG.h"

typedef std::vector<std::string> TStringVector;

const std::streamsize stream_temp_size = 128;

void print_usage();
std::string getLine( Menge::DataStreamInterface* _stream,  bool _trimAfter = true );
void trim( std::string& str, bool left = true, bool right = true );

TStringVector trimAlpha( const TStringVector& _images, size_t _alphaMaxValue, bool _alphaEdgeCorrection );

Menge::FileSystemInterface* g_fileSystem = NULL;

static std::vector<unsigned char> g_dataBuffer;

int main( int argc, char* argv[] )
{
	if( argc < 2 )
	{
		print_usage();
	}
	else
	{
		initInterfaceSystem( &g_fileSystem );
		if( g_fileSystem == NULL )
		{
			fprintf( stderr, "Error: while initializing file system interface\n");
			return 1;
		}

		size_t alphaMaxValue = 0;
		bool alphaEdgeCorrection = false;
		std::string inputFileName;
		std::string outputFileName;
		for( int i = 1; i < argc; ++i )
		{
			if( argv[i][0] == '/' )
			{
				if( argv[i][1] == 'a' )
				{
					alphaMaxValue = atoi( argv[i] + 3 );
				}
				else if( argv[i][1] == 'i' )
				{
					alphaEdgeCorrection = true;
				}
			}
			else
			{
				if( inputFileName.empty() == true )
				{
					inputFileName.assign( argv[i] );
				}
				else if( outputFileName.empty() == true )
				{
					outputFileName.assign( argv[i] );
				}
			}
		}

		if( inputFileName.empty() || outputFileName.empty() )
		{
			print_usage();
			releaseInterfaceSystem( g_fileSystem );
			return 0;
		}
		Menge::DataStreamInterface* input = g_fileSystem->openFile( inputFileName );
		if( input == NULL )
		{
			fprintf( stderr, "Can't open input file %s\n", inputFileName.c_str() );
			releaseInterfaceSystem( g_fileSystem );
			return 1;
		}
		Menge::OutStreamInterface* output = g_fileSystem->openOutStream( outputFileName, false );
		if( output == NULL )
		{
			fprintf( stderr, "Can't open output file %s\n", outputFileName.c_str() );
			releaseInterfaceSystem( g_fileSystem );
			return 1;
		}

		TStringVector imagesVector;
		while( input->eof() == false )
		{
			std::string imagePath = getLine( input );
			imagesVector.push_back( imagePath );
		}
		g_fileSystem->closeStream( input );
		input = NULL;
		TStringVector outputStrings = trimAlpha( imagesVector, alphaMaxValue, alphaEdgeCorrection );
		for( TStringVector::iterator it = outputStrings.begin(), it_end = outputStrings.end();
			it != it_end;
			++it )
		{
			output->write( (*it) + "\n" );
		}
		output->flush();
		/*if( output.bad() == true )
		{
		printf("Error while writing output into %s\n", outputFileName.c_str() );
		return 1;
		}*/
		g_fileSystem->closeOutStream( output );
		output = NULL;
		releaseInterfaceSystem( g_fileSystem );
		g_fileSystem = NULL;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void print_usage()
{
	printf("Usage: TrimAlphaTool [/a:<alpha_max_value>] [/i] <input> <output>\n"
		"alpha_max_value - default 0\n"
		"i - make edge alpha correction\n"
		"input - file containing image filepaths to make alpha trim\n"
		"output - file containing image filepath, offset, original size\n");
}
//////////////////////////////////////////////////////////////////////////
std::string getLine( Menge::DataStreamInterface* _stream, bool _trimAfter /*= true */ )
{
	char tmpBuf[stream_temp_size];
	std::string retString;
	std::size_t readCount;
	// Keep looping while not hitting delimiter
	while ( ( readCount = _stream->read( tmpBuf, ( stream_temp_size - 1 ) ) ) != 0 )
	{
		std::size_t term = readCount;
		// Terminate string
		tmpBuf[term] = '\0';

		char* p = strchr( tmpBuf, '\n' );
		if ( p != 0 )
		{
			// Reposition backwards
			_stream->skip( (long)( ( p + 1 - tmpBuf ) - readCount ) );
			*p = MENGE_TEXT('\0');
		}

		retString += tmpBuf;

		if ( p != 0 )
		{
			// Trim off trailing CR if this was a CR/LF entry
			if ( retString.length() && retString[retString.length()-1] == '\r' )
			{
				retString.erase( retString.length() - 1, 1 );
			}

			// Found terminator, break out
			break;
		}
	}

	if( _trimAfter )
	{
		trim( retString );
	}

	return retString;
}
//////////////////////////////////////////////////////////////////////////
void trim( std::string& str, bool left/* = true*/, bool right/* = true */)
{
	static const std::string delims = " \t\r";
	if(right)
	{
		str.erase(str.find_last_not_of(delims)+1); // trim right
	}
	if(left)
	{
		str.erase(0, str.find_first_not_of(delims)); // trim left
	}
}
//////////////////////////////////////////////////////////////////////////
TStringVector trimAlpha( const TStringVector& _images, size_t _alphaMaxValue, bool _alphaEdgeCorrection )
{
	Menge::ImageDecoder* imageDecoder = NULL;
	TStringVector outputStrings;
	std::stringstream strStream;

	for( TStringVector::const_iterator it = _images.begin(), it_end = _images.end();
		it != it_end;
		++it )
	{
		const std::string& fileName = (*it);
		std::string::size_type dot_pos = fileName.find_last_of('.');
		if( dot_pos == std::string::npos )
		{
			fprintf( stderr, "Unrecognized image filename %s\n", fileName.c_str() );
			continue;
		}
		// open file
		Menge::DataStreamInterface* imageFile = g_fileSystem->openFile( fileName );
		if( imageFile == NULL )
		{
			fprintf( stderr, "Can't open image file %s\n", fileName.c_str() );
			continue;
		}

		if( fileName.substr( dot_pos ) == ".png" )
		{
			imageDecoder = new Menge::ImageDecoderPNG( imageFile, ".png" );
			if( imageDecoder->getCodecDataInfo() == NULL )
			{
				fprintf( stderr, "Invalid png file %s\n", fileName.c_str() );
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
			const Menge::ImageCodecDataInfo* imageInfo = static_cast<const Menge::ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
			if( imageInfo->format != Menge::PF_A8R8G8B8
				&& imageInfo->format != Menge::PF_A8 )
			{
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
		}
		else
		{
			g_fileSystem->closeStream( imageFile );
			continue;
		}
		const Menge::ImageCodecDataInfo* imageInfo = static_cast<const Menge::ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
		g_dataBuffer.resize( imageInfo->size );
		unsigned char* dataBuffer = &(g_dataBuffer[0]);
		size_t origWidth = imageInfo->width;
		size_t origHeight = imageInfo->height;
		Menge::PixelFormat pf = imageInfo->format;
		imageDecoder->decode( dataBuffer, imageInfo->size );
		g_fileSystem->closeStream( imageDecoder->getStream() );
		imageDecoder->release();
		imageDecoder = NULL;
		size_t minX = origWidth, minY = origHeight;
		size_t maxX = 0, maxY = 0;
		size_t pixelBytes = 1;
		size_t alphaByte = 0;
		if( pf == Menge::PF_A8R8G8B8 )
		{
			pixelBytes = 4;
			alphaByte = 3;
		}
		for( size_t i = 0; i < origHeight; ++i )
		{
			for( size_t j = 0; j < origWidth; ++j )
			{
				if( dataBuffer[i*origWidth*pixelBytes+j*pixelBytes+alphaByte] > _alphaMaxValue )
				{
					if( j < minX ) minX = j;
					if( j > maxX ) maxX = j;
					if( i < minY ) minY = i;
					if( i > maxY ) maxY = i;
				}
			}
		}

		// this would remain at least 1 pixel in image
		if( minX > maxX ) minX = maxX;
		if( minY > maxY ) minY = maxY;

		size_t newWidth = maxX - minX + 1;
		size_t newHeight = maxY - minY + 1;

		if( origWidth == newWidth			// nothing to do here
			&& origHeight == newHeight )
		{
			continue;
		}

		for( size_t i = 0; i < newHeight; ++i )
		{
			std::copy( dataBuffer + (i+minY)*origWidth*pixelBytes + minX*pixelBytes
				, dataBuffer + (i+minY)*origWidth*pixelBytes + minX*pixelBytes + newWidth*pixelBytes
				, dataBuffer + i*origWidth*pixelBytes );
		}

		// make alpha correction
		if( _alphaEdgeCorrection == true
			&& pf == Menge::PF_A8R8G8B8 )
		{
			for( size_t i = 0; i < newHeight; ++i )
			{
				for( size_t j = 0; j < newWidth; ++j )
				{
					if( dataBuffer[i*origWidth*pixelBytes+j*pixelBytes+alphaByte] <= _alphaMaxValue )
					{
						unsigned int r = 0, g = 0, b = 0;
						size_t colorNum = 0;
						// left pixel
						if( j > 0
							&& dataBuffer[i*origWidth*pixelBytes+(j-1)*pixelBytes+alphaByte] > _alphaMaxValue )
						{
							++colorNum;
							r += dataBuffer[i*origWidth*pixelBytes+(j-1)*pixelBytes+0];
							g += dataBuffer[i*origWidth*pixelBytes+(j-1)*pixelBytes+1];
							b += dataBuffer[i*origWidth*pixelBytes+(j-1)*pixelBytes+2];
						}
						// top pixel
						if( i > 0
							&& dataBuffer[(i-1)*origWidth*pixelBytes+j*pixelBytes+alphaByte] > _alphaMaxValue )
						{
							++colorNum;
							r += dataBuffer[(i-1)*origWidth*pixelBytes+j*pixelBytes+0];
							g += dataBuffer[(i-1)*origWidth*pixelBytes+j*pixelBytes+1];
							b += dataBuffer[(i-1)*origWidth*pixelBytes+j*pixelBytes+2];
						}
						// right pixel
						if( j < newWidth - 1
							&& dataBuffer[i*origWidth*pixelBytes+(j+1)*pixelBytes+alphaByte] > _alphaMaxValue )
						{
							++colorNum;
							r += dataBuffer[i*origWidth*pixelBytes+(j+1)*pixelBytes+0];
							g += dataBuffer[i*origWidth*pixelBytes+(j+1)*pixelBytes+1];
							b += dataBuffer[i*origWidth*pixelBytes+(j+1)*pixelBytes+2];
						}
						// bottom pixel
						if( i < newHeight - 1
							&& dataBuffer[(i+1)*origWidth*pixelBytes+j*pixelBytes+alphaByte] > _alphaMaxValue )
						{
							++colorNum;
							r += dataBuffer[(i+1)*origWidth*pixelBytes+j*pixelBytes+0];
							g += dataBuffer[(i+1)*origWidth*pixelBytes+j*pixelBytes+1];
							b += dataBuffer[(i+1)*origWidth*pixelBytes+j*pixelBytes+2];
						}
						if( colorNum > 0 )
						{
							r /= colorNum;
							g /= colorNum;
							b /= colorNum;
							dataBuffer[i*origWidth*pixelBytes+j*pixelBytes+0] = r;
							dataBuffer[i*origWidth*pixelBytes+j*pixelBytes+1] = g;
							dataBuffer[i*origWidth*pixelBytes+j*pixelBytes+2] = b;
						}
					}
				}
			}
		}	// if( _alphaEdgeCorrection == true ...
		
		Menge::OutStreamInterface* output = g_fileSystem->openOutStream( fileName, true );
		if( output == NULL )
		{
			fprintf( stderr, "Error: can't open file for writing '%s'\n", fileName.c_str() );
			continue;
		}

		strStream.str("");
		strStream << fileName << " " 
			<< minX << " " << minY << " "
			<< origWidth << " " << origHeight;
		outputStrings.push_back( strStream.str() );
		// write image buffer to file
		Menge::ImageEncoder* imageEncoder = new Menge::ImageEncoderPNG( output, ".png" );
		Menge::ImageCodecDataInfo imageInfoEnc;
		imageInfoEnc.width = newWidth;
		imageInfoEnc.height = newHeight;
		imageInfoEnc.format = pf;
		unsigned int options = Menge::DF_CUSTOM_PITCH | (origWidth*pixelBytes << 16);
		imageEncoder->setOptions(options);
		imageEncoder->encode( dataBuffer, &imageInfoEnc );
		imageEncoder->release();
		g_fileSystem->closeOutStream( output );

		printf( "%s cut %d %%\n", fileName.c_str(), (int)((1.0f - static_cast<float>(newWidth*newHeight)/static_cast<float>(origWidth*origHeight))*100.0f) );
	}
	return outputStrings;
}
//////////////////////////////////////////////////////////////////////////
