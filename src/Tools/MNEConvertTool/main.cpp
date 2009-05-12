/*
 *	MNEConvertTool\main.cpp
 *
 *	Created by _Berserk_ on 16.4.2009
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
#	include "ImageEncoderJPEG.h"
#	include "ImageEncoderPNG.h"

typedef std::vector<std::string> TStringVector;

const std::streamsize stream_temp_size = 128;

void print_usage();
std::string getLine( Menge::DataStreamInterface* _stream,  bool _trimAfter = true );
void trim( std::string& str, bool left = true, bool right = true );

TStringVector mneConvert( const TStringVector& _images, int _quality );

Menge::FileSystemInterface* g_fileSystem = NULL;

static std::vector<unsigned char> g_dataBuffer;
static std::vector<unsigned char> g_alphaBuffer;

class MemoryOutStream
	: public Menge::OutStreamInterface
{
	public:
		void write( const void* _data, std::streamsize _count )
		{
			m_data.insert( m_data.end(), (unsigned char*)_data, (unsigned char*)_data + _count );
		}
		void write( const std::string& _str )
		{
		}
		void write( int _num )
		{
		}
		void write( float _num )
		{
		}
		void flush()
		{
		}
		
		size_t getSize()
		{
			return m_data.size();
		}

		unsigned char* getBuffer()
		{
			return &(m_data[0]);
		}
	private:
		std::vector<unsigned char> m_data;
};

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
			fprintf( stderr, "Error: while initializing file system interface\n" );
			return 1;
		}

		int quality = 100;
		std::string inputFileName;
		std::string outputFileName;
		for( int i = 1; i < argc; ++i )
		{
			if( argv[i][0] == '/' )
			{
				if( argv[i][1] == 'q' )
				{
					quality = atoi( argv[i] + 3 );
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
		TStringVector outputStrings = mneConvert( imagesVector, quality );
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
	printf("Usage: MNEConvert [/q:<jpeg_quality>] <input> <output>\n"
		"jpeg_quality - default 100\n"
		"input - file containing image filepaths to convert\n"
		"output - file containing converted image filepath\n");
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
TStringVector mneConvert( const TStringVector& _images, int _quality )
{
	Menge::ImageDecoderInterface* imageDecoder = NULL;
	TStringVector outputStrings;

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
				&& imageInfo->format != Menge::PF_R8G8B8 )
			{
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
		}
		else if( fileName.substr( dot_pos ) == ".jpg"
			|| fileName.substr( dot_pos ) == ".jpeg" )
		{
			imageDecoder = new Menge::ImageDecoderJPEG( imageFile, ".jpg" );
			if( imageDecoder->getCodecDataInfo() == NULL )
			{
				fprintf( stderr, "Invalid jpeg file %s\n", fileName.c_str() );
				imageDecoder->release();
				g_fileSystem->closeStream( imageFile );
				continue;
			}
			const Menge::ImageCodecDataInfo* imageInfo = static_cast<const Menge::ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
			if( imageInfo->format != Menge::PF_R8G8B8 
				|| imageInfo->quality <= _quality )
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

		size_t pitch = origWidth*4;
		std::string outputFileName = fileName.substr( 0, dot_pos );
		if( pf == Menge::PF_A8R8G8B8 )
		{
			outputFileName += ".mne";
			
			// prepare buffers
			g_alphaBuffer.resize( origWidth*origHeight );
			for( size_t i = 3, j = 0; i < g_dataBuffer.size(); i += 4, ++j )
			{
				g_alphaBuffer[j] = g_dataBuffer[i];
			}
			
			for( size_t i = 1; i < origWidth*origHeight; ++i )
			{
				std::copy( dataBuffer + i*4, dataBuffer + i*4 + 4, dataBuffer + i*3 );
			}
		}
		else
		{
			outputFileName += ".jpg";
			pitch = origWidth*3;
		}
		// write JPEG data into memory first
		MemoryOutStream* memStream = new MemoryOutStream();
		Menge::ImageEncoderInterface* jpegEncoder = new Menge::ImageEncoderJPEG( memStream, ".jpg" );
		Menge::ImageCodecDataInfo imageInfoEnc;
		imageInfoEnc.width = origWidth;
		imageInfoEnc.height = origHeight;
		imageInfoEnc.format = Menge::PF_R8G8B8;
		imageInfoEnc.quality = _quality;
		//jpegEncoder->setOptions( options );
		jpegEncoder->encode( dataBuffer, &imageInfoEnc );
		jpegEncoder->release();
		jpegEncoder = NULL;
		
		size_t jpegSize = memStream->getSize();
		Menge::OutStreamInterface* output = g_fileSystem->openOutStream( outputFileName, true );
		if( output == NULL )
		{
			fprintf( stderr, "Error: can't open file for writing '%s'\n", fileName.c_str() );
			delete memStream;
			continue;
		}
		if( pf == Menge::PF_A8R8G8B8 )
		{
			output->write( (void*)&jpegSize, sizeof( jpegSize ) );
		}
		// dump JPEG data into file
		output->write( (void*)memStream->getBuffer(), jpegSize );
		delete memStream;
		memStream = NULL;
				
		if( pf == Menge::PF_A8R8G8B8 )
		{
			Menge::ImageEncoderInterface* imageEncoder = new Menge::ImageEncoderPNG( output, ".png" );
			imageInfoEnc.width = origWidth;
			imageInfoEnc.height = origHeight;
			imageInfoEnc.format = Menge::PF_A8;
			imageEncoder->encode( &(g_alphaBuffer[0]), &imageInfoEnc );
			imageEncoder->release();
		}
		
		g_fileSystem->closeOutStream( output );
		outputStrings.push_back( fileName + " " + outputFileName );
		printf( "%s -> %s \n", fileName.c_str(), outputFileName.c_str() );
	}
	return outputStrings;

}
//////////////////////////////////////////////////////////////////////////
