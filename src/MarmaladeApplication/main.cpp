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

    Menge::MarmaladeApplication marApplication;
    
	try
	{
		if( marApplication.initialize() == true )
		{
			marApplication.loop();
		}
	}
	catch( const std::exception & se )
	{
		const char * se_what = se.what();

		s3eDebugErrorShow( S3E_MESSAGE_CONTINUE, se_what );
	}
    
    marApplication.finalize();

	stdex_allocator_finalize();

    return 0;
}
