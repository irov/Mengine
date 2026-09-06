#include "MacOSFindPython3Service.h"

#include "Environment/POSIX/POSIXCreateProcess.h"
#include "Environment/POSIX/POSIXFileHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"

#include <dirent.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FindPython3Service, Mengine::MacOSFindPython3Service );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct ProjectPythonCandidateDesc
        {
            const Char * executable;
            const Char * source;
        };
        //////////////////////////////////////////////////////////////////////////
        struct PythonFrameworkVersionDesc
        {
            int32_t major;
            int32_t minor;
            String executable;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Vector<PythonFrameworkVersionDesc> VectorPythonFrameworkVersions;
        //////////////////////////////////////////////////////////////////////////
        static bool comparePythonFrameworkVersionDesc( const PythonFrameworkVersionDesc & _left, const PythonFrameworkVersionDesc & _right )
        {
            if( _left.major != _right.major )
            {
                return _left.major > _right.major;
            }

            return _left.minor > _right.minor;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool makeExecutablePath( const Char * _directory, size_t _directoryLength, const Char * _executable, Char * const _path )
        {
            if( _directoryLength == 0 )
            {
                _directory = ".";
                _directoryLength = 1;
            }

            if( _directoryLength >= MENGINE_MAX_PATH )
            {
                return false;
            }

            Path directory = {'\0'};
            StdString::strzcpy_safe( directory, _directory, _directoryLength, MENGINE_MAX_PATH );

            int32_t pathLength = MENGINE_SNPRINTF( _path, MENGINE_MAX_PATH, "%s/%s"
                , directory
                , _executable
            );

            if( pathLength <= 0 )
            {
                return false;
            }

            if( pathLength >= MENGINE_MAX_PATH - 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool readXcrunPython3Path( Char * const _path )
        {
            if( Helper::POSIXReadProcessOutput( "/usr/bin/xcrun --find python3 2>/dev/null", _path, MENGINE_MAX_PATH ) == false )
            {
                return false;
            }

            size_t pathLength = StdString::strlen( _path );

            while( pathLength != 0 )
            {
                Char tail = _path[pathLength - 1];

                if( StdString::strchr( "\n\r \t", tail ) == nullptr )
                {
                    break;
                }

                _path[--pathLength] = '\0';
            }

            bool successful = pathLength != 0;

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSFindPython3Service::MacOSFindPython3Service()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSFindPython3Service::~MacOSFindPython3Service()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSFindPython3Service::_initializeService()
    {
        if( this->findPython3_() == false )
        {
            LOGGER_WARNING( "macOS python3 >= 3.10 was not found" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSFindPython3Service::_finalizeService()
    {
        m_python3ExecutablePath[0] = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSFindPython3Service::getPython3ExecutablePath( Char * const _path ) const
    {
        StdString::strcpy_safe( _path, m_python3ExecutablePath, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSFindPython3Service::findPython3_()
    {
        Path currentDirectory = {'\0'};

        if( Helper::POSIXGetCurrentDirectory( currentDirectory, MENGINE_MAX_PATH ) == true )
        {
            size_t directoryLength = StdString::strlen( currentDirectory );
            Detail::ProjectPythonCandidateDesc projectCandidates[] = {
                {".runtime/python/bin/python3", "project runtime"},
                {".venv/bin/python3", "project .venv"},
                {"venv/bin/python3", "project venv"}
            };

            for( const Detail::ProjectPythonCandidateDesc & projectCandidate : projectCandidates )
            {
                Path projectPython = {'\0'};

                if( Detail::makeExecutablePath( currentDirectory, directoryLength, projectCandidate.executable, projectPython ) == false )
                {
                    continue;
                }

                if( this->tryPython3_( projectPython, projectCandidate.source ) == true )
                {
                    return true;
                }
            }
        }

        const Char * homebrewCandidates[] = {
            "/opt/homebrew/bin/python3",
            "/usr/local/bin/python3"
        };

        for( const Char * candidate : homebrewCandidates )
        {
            if( this->tryPython3_( candidate, "Homebrew" ) == true )
            {
                return true;
            }
        }

        if( this->tryPython3_( "/Library/Frameworks/Python.framework/Versions/Current/bin/python3", "Python.framework Current" ) == true )
        {
            return true;
        }

        Detail::VectorPythonFrameworkVersions frameworkVersions;

        DIR * versionsDirectory = ::opendir( "/Library/Frameworks/Python.framework/Versions" );

        if( versionsDirectory != nullptr )
        {
            for( dirent * entry = ::readdir( versionsDirectory ); entry != nullptr; entry = ::readdir( versionsDirectory ) )
            {
                int32_t major = 0;
                int32_t minor = 0;
                Char tail = '\0';

                if( MENGINE_SSCANF( entry->d_name, "%d.%d%c", &major, &minor, &tail ) != 2 )
                {
                    continue;
                }

                Path executable = {'\0'};
                int32_t executableLength = MENGINE_SNPRINTF( executable, MENGINE_MAX_PATH
                    , "/Library/Frameworks/Python.framework/Versions/%s/bin/python3"
                    , entry->d_name
                );

                if( executableLength <= 0 )
                {
                    continue;
                }

                if( executableLength >= MENGINE_MAX_PATH - 1 )
                {
                    continue;
                }

                Detail::PythonFrameworkVersionDesc version;
                version.major = major;
                version.minor = minor;
                version.executable.assign( executable );

                frameworkVersions.emplace_back( version );
            }

            ::closedir( versionsDirectory );
        }

        Detail::VectorPythonFrameworkVersions::iterator itBegin = frameworkVersions.begin();
        Detail::VectorPythonFrameworkVersions::iterator itEnd = frameworkVersions.end();

        StdAlgorithm::sort( itBegin, itEnd, Detail::comparePythonFrameworkVersionDesc );

        for( const Detail::PythonFrameworkVersionDesc & version : frameworkVersions )
        {
            const Char * executable = version.executable.c_str();

            if( this->tryPython3_( executable, "Python.framework version" ) == true )
            {
                return true;
            }
        }

        Path xcrunPython3 = {'\0'};

        if( Detail::readXcrunPython3Path( xcrunPython3 ) == true )
        {
            if( this->tryPython3_( xcrunPython3, "xcrun" ) == true )
            {
                return true;
            }
        }

        if( this->tryPython3_( "/usr/bin/python3", "Apple system" ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSFindPython3Service::tryPython3_( const Char * _path, const Char * _source )
    {
        if( ::access( _path, X_OK ) != 0 )
        {
            return false;
        }

        const Char * arguments[] = {
            _path,
            "-c",
            "import sys; raise SystemExit(0 if sys.version_info >= (3, 10) else 1)",
            nullptr
        };

        uint32_t exitCode = EXIT_FAILURE;

        if( Helper::POSIXCreateProcess( _path, arguments, &exitCode ) == false )
        {
            return false;
        }

        if( exitCode != EXIT_SUCCESS )
        {
            return false;
        }

        StdString::strcpy_safe( m_python3ExecutablePath, _path, MENGINE_MAX_PATH );

        LOGGER_MESSAGE( "found macOS python3 >= 3.10 executable '%s' source '%s'"
            , m_python3ExecutablePath
            , _source
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
