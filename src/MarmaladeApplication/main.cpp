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

	stdex_allocator_initialize();

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
