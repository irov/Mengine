using System;
using System.Collections.Generic;
using System.Text;

namespace ProjectManager
{
    public class ResourceImageDefault
    {
        public ResourceImageDefault(String name, String type)
        {
            Name = name;
            Type = type;
        }

        public String GetName()
        {
            return Name;
        }

        public String GetType()
        {
            return Type;
        }

        public void AddFile(String file)
        {
            files.Add(file);
        }

        public String GetImagePath(int index)
        {
            if (index >= files.Count)
            {
                return null;
            }

            return files[index];
        }

        public int GetImageCount()
        {
            return files.Count;
        }

        public List<String> Files
        {
            get { return files; }
            set { files = value; }
        }

        private List<String> files = new List<String>();
        private String Name;
        private String Type;
    }
}
