using System;
using System.IO;

namespace MengeProjectBuilder
{
    static public class Utils
    {
        static public void copyDirectory(string _srcDir, string _dstDir)
        {
            copyDirectory(_srcDir, _dstDir, "*.*");
        }
        static public void copyDirectory( string _srcDir, string _dstDir, string _matchPatter )
        {
            DirectoryInfo sourceDir = new DirectoryInfo( _srcDir );
            DirectoryInfo destDir = new DirectoryInfo( _dstDir );
            System.Text.RegularExpressions.Regex rx = new System.Text.RegularExpressions.Regex(@_matchPatter);

            if (destDir.Exists == false)
            {
                destDir.Create();
            }

            foreach (FileInfo childFile in sourceDir.GetFiles())
            {
                if (rx.IsMatch(childFile.Name))
                {
                    childFile.CopyTo(Path.Combine(destDir.FullName, childFile.Name), true);
                }
            }

            foreach (DirectoryInfo subDir in sourceDir.GetDirectories())
            {
                
                /*if (subDir.Exists == false)
                {
                    subDir.Create();
                }
                                
                CopyDirectory(SubDir.FullName, Path.Combine(DestDir.FullName,SubDir.Name));*/
            }


        }

        static public FileInfo searchFile(DirectoryInfo _dirInfo, string _filename)
        {
            FileInfo[] files = _dirInfo.GetFiles();
            foreach (FileInfo file in files)
            {
                if (file.Name.IndexOf(_filename) > -1)
                {
                    return file;
                }
            }

            // search deeper
            DirectoryInfo[] dirs = _dirInfo.GetDirectories();
            foreach (DirectoryInfo subDir in dirs)
            {
                FileInfo file = searchFile(subDir, _filename);
                if (file != null)
                {
                    return file;
                }
            }

            return null;
        }

        static public string GetNodeAttribute(System.Xml.XmlDocument _doc, string _nodeName, string _attributeName)
        {
            System.Xml.XmlNodeList nodeList = _doc.GetElementsByTagName(_nodeName);
            if (nodeList.Count == 0)
            {
                return string.Empty;
            }
            System.Xml.XmlNode node = nodeList.Item(0);
            System.Xml.XmlNode attrib = node.Attributes.GetNamedItem(_attributeName);
            if (attrib == null)
            {
                return string.Empty;
            }
            return attrib.Value;
        }
    }
}