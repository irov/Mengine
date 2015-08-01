#   include "Config/Config.h"

#   include "MarmaladeApplication.h"

#   include "s3eDebug.h"
#   include "s3eConfig.h"

#   include <IwGL.h>

#   include <s3eGL.h>

int main()
{
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!MENGINE START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    char commandLine[S3E_CONFIG_STRING_MAX] = {0};
    if( s3eConfigGetString( "MENGINE", "CommandLine", commandLine ) == S3E_RESULT_ERROR )
    {
		printf("s3eConfigGetString %s:%s return error '%s'\n"
			, "MENGINE"
			, "CommandLine"
			, s3eConfigGetErrorString()
			);
	}

	stdex_allocator_initialize();

	try
	{
		Menge::MarmaladeApplication marApplication;

		if( marApplication.initialize( commandLine ) == true )
		{
			marApplication.loop();
		}

		marApplication.finalize();
	}
	catch( const std::exception & se )
	{
		const char * se_what = se.what();

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, se_what );
	}

	stdex_allocator_finalize();

    return 0;
}
