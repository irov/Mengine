
#include "Config/Config.h"
#include "MarmaladeApplication.h"

int main()
{
    Menge::MarmaladeApplication marApplication;

    if( marApplication.initialize() == false )
    {
        return 0;
    }

    marApplication.loop();

    marApplication.finalize();

    return 0;
}
