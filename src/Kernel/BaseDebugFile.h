#pragma once

#include "Interface/DebugFileInterface.h"

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)

namespace Mengine
{
    class BaseDebugFile
        : public DebugFileInterface
    {
    public:
        BaseDebugFile();
        ~BaseDebugFile() override;

    protected:
        void setDebugRelationPath( const FilePath & _relationPath );
        void setDebugFolderPath( const FilePath & _folderPath );
        void setDebugFilePath( const FilePath & _filePath );

    protected:
        const FilePath & getDebugRelationPath() const override;
        const FilePath & getDebugFolderPath() const override;
        const FilePath & getDebugFilePath() const override;
        
    private:
        FilePath m_debugRelationPath;
        FilePath m_debugFolderPath;
        FilePath m_debugFilePath;
    };
}

#endif