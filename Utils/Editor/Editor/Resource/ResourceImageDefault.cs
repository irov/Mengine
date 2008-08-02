using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;
using System.IO;

namespace Editor
{
    struct ImageDesc
    {
        public Image image;
        public String path;
    }

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
            Image image = Image.FromFile(file);

            if (image == null)
            {
                return;
            }

            ImageDesc desc = new ImageDesc();

            desc.image = image;
            desc.path = file;

            files.Add(desc);
        }

        public Image GetImage(String name)
        {
            ImageDesc result = files.Find(delegate(ImageDesc desc) { return desc.path == name; });
            return result.image;
        }

        public Image GetImage(int index)
        {
            if (index >= files.Count)
            {
                return null;
            }

            return files[index].image;
        }

        public String GetImagePath(int index)
        {
            if (index >= files.Count)
            {
                return null;
            }

            return files[index].path;
        }

        public int GetImageCount()
        {
            return files.Count;
        }

        private List<ImageDesc> files = new List<ImageDesc>();
        private String Name;
        private String Type;
    }
}
