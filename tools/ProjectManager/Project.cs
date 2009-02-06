using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.Windows.Forms;
using System.IO;

namespace ProjectManager
{
    //СЕРИАЛИЗАЦИЯ
    [XmlRootAttribute("Game", Namespace = "", IsNullable = false)]
    public class GameProperties
    {
        [XmlAttribute("Title")]
        private String _Title;
        [XmlAttribute("ResourceResolution")]
        private String _ResourceResolution;
        [XmlAttribute("FixedContentResolution")]
        private String _FixedContentResolution;
        [XmlAttribute("PersonalityModule")]
        private String _PersonalityModule;
        [XmlAttribute("InitFunction")]
        private String _InitFunction;
        [XmlAttribute("UpdateFunction")]
        private String _UpdateFunction;
        [XmlAttribute("FinilizeFunction")]
        private String _FinilizeFunction;
        [XmlAttribute("DefaultArrow")]
        private String _DefaultArrow;
        [XmlAttribute("Width")]
        private int _Width;
        [XmlAttribute("Height")]
        private int _Height;
        [XmlAttribute("Bits")]
        private int _Bits;
        [XmlAttribute("Fullscreen")]
        private int _Fullscreen;

        public String Title
        {
            get { return _Title; }
            set { _Title = value; }
        }
        public String ResourceResolution
        {
            get { return _ResourceResolution; }
            set { _ResourceResolution = value; }
        }
        public String FixedContentResolution
        {
            get { return _FixedContentResolution; }
            set { _FixedContentResolution = value; }
        }
        public String PersonalityModule
        {
            get { return _PersonalityModule; }
            set { _PersonalityModule = value; }
        }
        public String InitFunction
        {
            get { return _InitFunction; }
            set { _InitFunction = value; }
        }
        public String UpdateFunction
        {
            get { return _UpdateFunction; }
            set { _UpdateFunction = value; }
        }
        public String FinilizeFunction
        {
            get { return _FinilizeFunction; }
            set { _FinilizeFunction = value; }
        }
        public String DefaultArrow
        {
            get { return _DefaultArrow; }
            set { _DefaultArrow = value; }
        }
        public int Width
        {
            get { return _Width; }
            set { _Width = value; }
        }
        public int Height
        {
            get { return _Height; }
            set { _Height = value; }
        }
        public int Bits
        {
            get { return _Bits; }
            set { _Bits = value; }
        }
        public int Fullscreen
        {
            get { return _Fullscreen; }
            set { _Fullscreen = value; }
        }
    }

    public class Project
    {
        public Project()
        {}

        //E:\\Cards
        public String GetProjectDirectory()
        {
            return ProjectDirectory;
        }   
        //Cards
        public String GetProjectName()
        {
            return ProjectName;
        }
        //E:\\Cards\\Bin\\Application.xml
        public String GetAppXMLPath()
        {
            return ApplicationXMLPath;
        }
        //Game\\Game.xml
        public String GetGameXMLPath()
        {
            return GameXMLPath;
        }
        //E:\\Cards\\Game\\Game.xml
        public String GetFullGameXMLPath()
        {
            return System.IO.Path.Combine(ProjectDirectory, GameXMLPath);
        }
        //Framework\\Game.xml
        public List<String> GetResourcesXMLPath()
        {
            return ResourcesXMLPath;
        }

        public GameProperties GetGamePropertiesInfo()
        {
            return GamePropertiesInfo;
        }

        public void Load(String appXMLPath)
        {
            ApplicationXMLPath = appXMLPath;

            SetProjectDirectory(ApplicationXMLPath);

            XmlDocument document = new XmlDocument();

            try
            {
                document.Load(ApplicationXMLPath);
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Ошибка загрузки XML");
                return;
            }

            string xpathApplication = "/Application";

            ResourcesXMLPath = new List<string>();
            //ResourcesPath = new List<string>();

            try
            {
                XmlNode gamenode =
                        document.SelectSingleNode(xpathApplication);

                foreach (XmlNode node in gamenode.ChildNodes)
                {
                    if (node.Name.ToLower() == "game")
                    {
                        GameXMLPath = node.Attributes[0].Value;
                    }

                    if (node.Name.ToLower() == "resource")
                    {
                        String FullResourcePath = System.IO.Path.Combine(ProjectDirectory, node.Attributes[0].Value);
                        ResourcesXMLPath.Add(FullResourcePath);
                    }
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Ошибка чтения XML");
                return;
            }

            String FullGameXMLPath = GetFullGameXMLPath();

            ParseGameXML(FullGameXMLPath);

            foreach (String resource in ResourcesXMLPath)
            {
                String resourcePath = ParseResourceXML(resource);
                if (resourcePath != "")
                {
                    ResourcePath = resourcePath;
                    //ResourcesPath.Add(resourcePath);
                }
            }
        }

        public String GetResourcePath()
        {
            return ResourcePath;
        }


        //private ResourceImageDefault

        public void Save()
        {
            XmlDocument document = new XmlDocument();

            XmlElement root = document.CreateElement("", "Game", "");
            document.AppendChild(root);

            XmlElement xmlelem = document.CreateElement("", "Title", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.Title);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "ResourceResolution", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.ResourceResolution);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "FixedContentResolution", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.FixedContentResolution);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "PersonalityModule", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.PersonalityModule);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "InitFunction", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.InitFunction);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "UpdateFunction", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.UpdateFunction);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "FinilizeFunction", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.FinilizeFunction);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "DefaultArrow", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.DefaultArrow);

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "Width", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.Width.ToString());

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "Height", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.Height.ToString());

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "Bits", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.Bits.ToString());

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            xmlelem = document.CreateElement("", "Fullscreen", "");
            xmlelem.SetAttribute("Value", GamePropertiesInfo.Fullscreen.ToString());

            document.ChildNodes.Item(0).AppendChild(xmlelem);

            document.Save(GameXMLPath);
        }

        private void SetProjectDirectory(String appXMLPath)
        {
            String BinDirectory = System.IO.Directory.GetParent(appXMLPath).FullName;

            ProjectDirectory = System.IO.Directory.GetParent(BinDirectory).FullName;
            ProjectName = System.IO.Directory.GetParent(BinDirectory).Name;
        }

        private String ParseResourceXML(String resourceXMLPath)
        {
            XmlDocument document = new XmlDocument();

            GamePropertiesInfo = new GameProperties();

            try
            {
                document.Load(resourceXMLPath);
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Ошибка загрузки XML");
                return "";
            }

            string xpathApplication = "/Resources";

            XmlNode gamenode =
                        document.SelectSingleNode(xpathApplication);

            String resourcePath = "";

            foreach (XmlNode node in gamenode.ChildNodes)
            {
                switch (node.Name.ToLower())
                {
                    case "resource":
                        {
                            String value = node.Attributes[0].Value;
                            String name = node.ChildNodes.Item(0).Name;
                            String vn = System.IO.Path.Combine(value, name);

                            String gamedir = GetFullGameXMLPath();
                            resourcePath = System.IO.Directory.GetParent(gamedir).FullName;
                            resourcePath = System.IO.Path.Combine(resourcePath, vn);
                            resourcePath += ".resource";

                            break;
                        }
                }
            }

            return resourcePath;
        }

        private void ParseGameXML(String gameXMLPath)
        {
            GameXMLPath = gameXMLPath;

            XmlDocument document = new XmlDocument();

            GamePropertiesInfo = new GameProperties();

            try
            {
                document.Load(gameXMLPath);
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Ошибка загрузки XML");
                return;
            }

            string xpathApplication = "/Game";

            try
            {
                XmlNode gamenode =
                        document.SelectSingleNode(xpathApplication);

                foreach (XmlNode node in gamenode.ChildNodes)
                {
                    String value = node.Attributes[0].Value;

                    switch (node.Name.ToLower())
                    {
                        case "title":
                            {
                                GamePropertiesInfo.Title = value;
                                break;
                            }
                        case "resourceresolution":
                            {
                                GamePropertiesInfo.ResourceResolution = value;
                                break;
                            }
                        case "fixedcontentresolution":
                            {
                                GamePropertiesInfo.FixedContentResolution = value;
                                break;
                            }
                        case "personalitymodule":
                            {
                                GamePropertiesInfo.PersonalityModule = value;
                                break;
                            }
                        case "initfunction":
                            {
                                GamePropertiesInfo.InitFunction = value;
                                break;
                            }
                        case "updatefunction":
                            {
                                GamePropertiesInfo.UpdateFunction = value;
                                break;
                            }
                        case "finilizefunction":
                            {
                                GamePropertiesInfo.FinilizeFunction = value;
                                break;
                            }
                        case "defaultarrow":
                            {
                                GamePropertiesInfo.DefaultArrow = value;
                                break;
                            }
                        case "width":
                            {
                                GamePropertiesInfo.Width = Convert.ToInt32(value);
                                break;
                            }
                        case "height":
                            {
                                GamePropertiesInfo.Height = Convert.ToInt32(value);
                                break;
                            }
                        case "bits":
                            {
                                GamePropertiesInfo.Bits = Convert.ToInt32(value);
                                break;
                            }
                        case "fullscreen":
                            {
                                GamePropertiesInfo.Fullscreen = Convert.ToInt32(value);
                                break;
                            }
                    }
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Ошибка чтения XML");
                return;
            }
        }

        private String ProjectName;
        private String ProjectDirectory;
        private String ApplicationXMLPath;
        private String GameXMLPath;
        private List<String> ResourcesXMLPath = null;
        private String ResourcePath;
        //private List<String> ResourcesPath = null;
        private GameProperties GamePropertiesInfo = null;
    }
}