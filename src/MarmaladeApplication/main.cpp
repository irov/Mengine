#   include "Config/Config.h"

#   include "MarmaladeApplication.h"

#   include "s3eDebug.h"
#   include "s3eConfig.h"

#	include <s3eDevice.h>

int main()
{
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!MENGINE START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	stdex_allocator_initialize();

	//s3eDeviceYield( 10000 );

	try
	{
		Menge::MarmaladeApplication marApplication;

		if( marApplication.initialize() == true )
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
