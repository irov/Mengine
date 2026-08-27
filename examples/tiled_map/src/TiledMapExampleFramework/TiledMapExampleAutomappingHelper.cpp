#include "TiledMapExampleAutomappingHelper.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/String.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool makeTiledMapExternalPath( const FilePath & _ownerPath, const Char * _sourceBegin, const Char * _sourceEnd, FilePath * const _filePath )
            {
                size_t sourceSize = _sourceEnd - _sourceBegin;

                if( sourceSize == 0 || _sourceBegin[0] == '/' || _sourceBegin[0] == '\\' || (sourceSize > 1 && _sourceBegin[1] == ':') )
                {
                    return false;
                }

                Char path[MENGINE_MAX_PATH] = {'\0'};

                size_t pathSize = Helper::getFolderPath( _ownerPath, path );

                if( sourceSize >= MENGINE_MAX_PATH - pathSize )
                {
                    return false;
                }

                Helper::memoryCopy( path, pathSize, _sourceBegin, 0, sourceSize );

                path[pathSize + sourceSize] = '\0';

                Helper::pathCorrectBackslashA( path );

                const Char * pathBegin = path;
                const Char * pathEnd = pathBegin + pathSize + sourceSize;
                const Char * partBegin = pathBegin;
                String normalized;

                while( partBegin != pathEnd )
                {
                    const Char * delimiter = StdString::strstr( partBegin, "/" );
                    const Char * partEnd = delimiter != nullptr ? delimiter : pathEnd;
                    size_t partSize = partEnd - partBegin;

                    if( partSize != 0 && (partSize != 1 || partBegin[0] != '.') )
                    {
                        if( partSize == 2 && partBegin[0] == '.' && partBegin[1] == '.' )
                        {
                            if( normalized.empty() == true )
                            {
                                return false;
                            }

                            const Char * normalizedBegin = normalized.c_str();
                            const Char * previousDelimiter = StdString::strrchr( normalizedBegin, '/' );

                            if( previousDelimiter == nullptr )
                            {
                                normalized.clear();
                            }
                            else
                            {
                                normalized.erase( previousDelimiter - normalizedBegin );
                            }
                        }
                        else
                        {
                            if( normalized.empty() == false )
                            {
                                normalized += '/';
                            }

                            normalized.append( partBegin, partEnd );
                        }
                    }

                    if( delimiter == nullptr )
                    {
                        break;
                    }

                    partBegin = delimiter + 1;
                }

                if( normalized.empty() == true )
                {
                    return false;
                }

                *_filePath = Helper::stringizeFilePath( normalized );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool collectTiledMapAutomappingRuleMaps( const FileGroupInterfacePtr & _fileGroup, const FilePath & _ruleListPath, VectorFilePath * const _parsedRuleLists, VectorFilePath * const _ruleMaps )
        {
            VectorFilePath::const_iterator parsedIt = StdAlgorithm::find_if( _parsedRuleLists->begin(), _parsedRuleLists->end(), [&_ruleListPath]( const FilePath & _filePath )
            {
                return StdString::strcmp( _filePath.c_str(), _ruleListPath.c_str() ) == 0;
            } );

            if( parsedIt != _parsedRuleLists->end() )
            {
                return true;
            }

            _parsedRuleLists->emplace_back( _ruleListPath );

            MemoryInterfacePtr memory = Helper::createMemoryFileString( _fileGroup, _ruleListPath, false, false, MENGINE_DOCUMENT_FUNCTION );

            if( memory == nullptr )
            {
                return false;
            }

            const Char * contentBegin = memory->getBuffer();
            const Char * contentEnd = contentBegin + memory->getSize() - 1;
            const Char * lineBegin = contentBegin;

            while( lineBegin != contentEnd )
            {
                const Char * delimiter = StdString::strstr( lineBegin, "\n" );
                const Char * lineEnd = delimiter != nullptr ? delimiter : contentEnd;

                while( lineBegin != lineEnd && (*lineBegin == ' ' || *lineBegin == '\t' || *lineBegin == '\r') )
                {
                    ++lineBegin;
                }

                while( lineEnd != lineBegin && (lineEnd[-1] == ' ' || lineEnd[-1] == '\t' || lineEnd[-1] == '\r') )
                {
                    --lineEnd;
                }

                size_t lineSize = lineEnd - lineBegin;

                if( lineSize != 0 && lineBegin[0] != '#' && lineBegin[0] != '['
                    && (lineSize < 2 || lineBegin[0] != '/' || lineBegin[1] != '/') )
                {
                    FilePath referencePath;

                    if( Helper::Detail::makeTiledMapExternalPath( _ruleListPath, lineBegin, lineEnd, &referencePath ) == true )
                    {
                        const ConstString & extension = Helper::getFilePathExt( referencePath );

                        if( StdString::strcmp( extension.c_str(), "txt" ) == 0 )
                        {
                            if( _fileGroup->existFile( referencePath, true ) == true
                                && Helper::collectTiledMapAutomappingRuleMaps( _fileGroup, referencePath, _parsedRuleLists, _ruleMaps ) == false )
                            {
                                return false;
                            }
                        }
                        else if( StdString::strcmp( extension.c_str(), "tmx" ) == 0
                            || StdString::strcmp( extension.c_str(), "tmj" ) == 0
                            || StdString::strcmp( extension.c_str(), "json" ) == 0 )
                        {
                            VectorFilePath::const_iterator ruleMapIt = StdAlgorithm::find_if( _ruleMaps->begin(), _ruleMaps->end(), [&referencePath]( const FilePath & _filePath )
                            {
                                return StdString::strcmp( _filePath.c_str(), referencePath.c_str() ) == 0;
                            } );

                            if( ruleMapIt == _ruleMaps->end() )
                            {
                                _ruleMaps->emplace_back( referencePath );
                            }
                        }
                    }
                }

                if( delimiter == nullptr )
                {
                    break;
                }

                lineBegin = delimiter + 1;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
