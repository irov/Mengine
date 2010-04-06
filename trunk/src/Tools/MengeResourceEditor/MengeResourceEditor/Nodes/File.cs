using System;
using System.Collections.Generic;
using System.Text;

namespace MengeResourceEditor.Nodes
{
    class File: Node
    {
        string path;
        bool noAtlas = false;
        bool noJpeg = false;

        public File(string _path)
        {
            path = _path;
        }
    }
}
