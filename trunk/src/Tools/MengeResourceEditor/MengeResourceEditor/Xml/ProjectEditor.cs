using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace MengeResourceEditor
{
    class ProjectEditor
    {
        List<PackEditor> packs = new List<PackEditor>();
        string baseFolderPath;
        string appXmlPath;
        string startupXmlPath;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_folderPath"></param>
        public void Initialize(string _folderPath)
        {
            baseFolderPath = _folderPath;
            try
            {
                OpenApplicationXml();
                OpenStartupXml();
            }
            catch(Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        private void OpenApplicationXml()
        {
            appXmlPath = baseFolderPath + System.IO.Path.DirectorySeparatorChar + "Bin" + System.IO.Path.DirectorySeparatorChar + "application.xml";
            XmlDocument doc = new XmlDocument();
            doc.Load(appXmlPath);

            XmlNode node = doc.FirstChild.SelectSingleNode("GamePack");
            string folderName = node.Attributes.GetNamedItem("Path").Value;
            string fileName = node.Attributes.GetNamedItem("Description").Value;
            startupXmlPath = baseFolderPath + System.IO.Path.DirectorySeparatorChar + folderName + System.IO.Path.DirectorySeparatorChar + fileName;
        }
        /// <summary>
        /// 
        /// </summary>
        private void OpenStartupXml()
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(startupXmlPath);
            
            XmlNodeList packList = doc.FirstChild.SelectNodes("ResourcePack");
            foreach(XmlNode nodePack in packList)
            {
                string packFolder = nodePack.Attributes.GetNamedItem("Path").Value;
                string packResName = nodePack.Attributes.GetNamedItem("Description").Value;
                string fullPackPath = baseFolderPath + System.IO.Path.DirectorySeparatorChar + packFolder;
                PackEditor pack = new PackEditor(packFolder, fullPackPath, packResName);
                packs.Add(pack);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        public List<PackEditor> Packs
        {
            get
            {
                return packs;
            }
        }
    }

    

}
