using System;
using System.Collections.Generic;
using System.Text;

namespace MengeSourceBuilder
{
    enum PackageType
    {
        Archive,
        Svn
    }

    struct Package
    {
        public PackageType m_type;
        public string m_url;
        public string m_path;
    }
}
