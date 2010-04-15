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
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_path"></param>
        public File(string _path)
        {
            path = _path;
        }
        /// <summary>
        /// 
        /// </summary>
        public string Path
        {
            get{
                return path;
            }
            set{
                path = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public bool NoAtlas
        {
            get{
                return noAtlas;
            }
            set{
                noAtlas = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public bool NoJpeg
        {
            get{
                return noJpeg;
            }
            set{
                noJpeg = value;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString ()
        {
            return Path;
        }
    }
}
