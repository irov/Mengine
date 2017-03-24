#	pragma once

#   include "Interface/FileSystemInterface.h"

#   include "Core/ServantBase.h"

#   include "Factory/Factory.h"

namespace Menge
{
    class SDLFileGroupDirectory
        : public ServantBase<FileGroupInterface>
    {
    public:
        SDLFileGroupDirectory();
        ~SDLFileGroupDirectory();

    public:
        bool initialize( const FilePath & _path ) override;
        void finalize() override;

    public:
        bool isPacked() const override;

    public:
        const FilePath & getPath() const override;

    public:
        bool existFile( const FilePath & _fileName ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
        bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
        
    public:
        OutputStreamInterfacePtr createOutputFile() override;        
        bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

    protected:        
        FilePath m_path;

        FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
    };
}	// namespace Menge
