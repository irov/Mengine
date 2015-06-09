#   include "Config/Config.h"

#   include "MarmaladeApplication.h"

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

    Menge::MarmaladeApplication marApplication;

    if( marApplication.initialize( commandLine ) == false )
    {
        return 0;
    }

    marApplication.loop();

    marApplication.finalize();

    return 0;
}
