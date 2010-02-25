using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace ProjectManager
{
    class FileSystem
    {
        public static void CopyDirectory(string src, string dst)
        {
            String[] files;

            if (dst[dst.Length - 1] != Path.DirectorySeparatorChar)
                dst += Path.DirectorySeparatorChar;

            if (!Directory.Exists(dst))
            {
                Directory.CreateDirectory(dst);
            }

            files = Directory.GetFileSystemEntries(src);

            foreach (string element in files)
            {
                if (element.EndsWith(".svn") || element.EndsWith(".db"))
                {
                    continue;
                }

                if (Directory.Exists(element))
                {
                    if (element + Path.DirectorySeparatorChar != dst)
                    {
                        CopyDirectory(element, dst + Path.GetFileName(element));
                    }
                }
                else
                {
                    File.Copy(element, dst + Path.GetFileName(element), true);
                }
            }
        }
    }
}