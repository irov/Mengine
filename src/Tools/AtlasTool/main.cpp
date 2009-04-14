

#	include <cstdio>
#	include <vector>
#	include <string>
#	include <fstream>

#	include "atlas.h"
#	include "Interface/FileSystemInterface.h"

const std::streamsize stream_temp_size = 128;

void print_usage();
std::string getLine( Menge::DataStreamInterface* _stream,  bool _trimAfter = true );
void trim( std::string& str, bool left = true, bool right = true );

Menge::FileSystemInterface* g_fileSystem = NULL;

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
			printf("Error: while initializing file system interface\n");
			return 1;
		}
		size_t atlas_max_size = 1024;
		size_t image_max_size = 256;
		std::string inputFileName;
		std::string outputFileName;
		std::string atlasName;
		for( int i = 1; i < argc; ++i )
		{
			if( argv[i][0] == '/' )
			{
				if( argv[i][1] == 'a' )
				{
					atlas_max_size = atoi( argv[i] + 2 );
				}
				else if( argv[i][1] == 'i' )
				{
					image_max_size = atoi( argv[i] + 2 );
				}
			}
			else
			{
				if( atlasName.empty() == true )
				{
					atlasName.assign( argv[i] );
				}
				else if( inputFileName.empty() == true )
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
			printf( "Can't open input file %s\n", inputFileName.c_str() );
			releaseInterfaceSystem( g_fileSystem );
			return 1;
		}
		Menge::OutStreamInterface* output = g_fileSystem->openOutStream( outputFileName, false );
		if( output == NULL )
		{
			printf( "Can't open output file %s\n", outputFileName.c_str() );
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
		TStringVector outputStrings = build( atlasName, imagesVector, atlas_max_size, image_max_size );
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
	printf("Usage: AtlasTool [/a:<atlas_max_size>] [/i:<image_max_size>] <atlas_name> <input> <output>\n"
		"atlas_max_size - default 1024\n"
		"image_max_size - default 256\n"
		"atlas_name - atlas filename begin with\n"
		"input - file containing image filepaths to make atlas\n"
		"output - file containing image filepath, atlas filepath, uv\n");
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
