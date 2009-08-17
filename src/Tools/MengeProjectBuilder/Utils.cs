using System;
using System.IO;

namespace MengeProjectBuilder
{
    static public class Utils
    {
        static public void copyDirectory(string _srcDir, string _dstDir)
        {
            copyDirectory(_srcDir, _dstDir, "");
        }
        static public void copyDirectory( string _srcDir, string _dstDir, string _matchPatter )
        {
            DirectoryInfo sourceDir = new DirectoryInfo( _srcDir );
            DirectoryInfo destDir = new DirectoryInfo( _dstDir );
            //System.Text.RegularExpressions.Regex rx = new System.Text.RegularExpressions.Regex(@_matchPatter);

            if (destDir.Exists == false)
            {
                destDir.Create();
            }

            foreach (FileInfo childFile in sourceDir.GetFiles())
            {
                //if (rx.IsMatch(childFile.Name))
                {
                    childFile.CopyTo(Path.Combine(destDir.FullName, childFile.Name), true);
                }
            }

            foreach (DirectoryInfo subDir in sourceDir.GetDirectories())
            {
                
                if (subDir.Exists == false)
                {
                    subDir.Create();
                }

                copyDirectory(subDir.FullName, Path.Combine(destDir.FullName, subDir.Name));
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

        static public bool checkPathRegister(string _path)
        {
            if (System.IO.File.Exists(_path) == true)
            {
                
                string fileName = System.IO.Path.GetFileName(_path);
                _path = System.IO.Path.GetDirectoryName(_path);
                string searchDir = _path;
                if( _path == "" )
                {
                    searchDir = System.IO.Directory.GetCurrentDirectory();
                }
                string realFileName = System.IO.Directory.GetFiles(searchDir, fileName)[0];
                realFileName = System.IO.Path.GetFileName(realFileName);
                if ( realFileName != fileName)
                {
                    return false;
                }
            }

            return checkFolderPathRegister(_path);
        }

        static bool checkFolderPathRegister(string _path)
        {
            while (_path != "")
            {
                if (System.IO.Directory.Exists(_path) == false)
                {
                    return false;
                }

                string dirName = System.IO.Path.GetFileName(_path);
                _path = System.IO.Path.GetDirectoryName(_path);
                string searchDir = _path;
                if (_path == "")
                {
                    searchDir = System.IO.Directory.GetCurrentDirectory();
                }
                string realDirName = System.IO.Directory.GetDirectories(searchDir, dirName)[0];
                realDirName = System.IO.Path.GetFileName(realDirName);
                if (realDirName != dirName)
                {
                    return false;
                }
            }
            return true;
        }
    }
}