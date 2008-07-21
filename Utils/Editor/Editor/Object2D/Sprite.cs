using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;
using System.Xml;

namespace Editor
{
    class Sprite : Node
    {
        Matrix Node.getTransform()
        {
            return M;
        }

        String Node.getName()
        {
            return Name;
        }

        void Node.setName(String name)
        {
            Name = name;
        }

        void Node.save(ref XmlTextWriter writer)
        {
            writer.WriteStartElement("Node");


            writer.WriteAttributeString("Name", Name);
            writer.WriteAttributeString("Type", "Sprite");

            writer.WriteStartElement("Transformation");
            String value = "1;0;0;1;" + Position.X.ToString() + ";" + Position.Y.ToString();
            writer.WriteAttributeString("Value", value);
            writer.WriteEndElement();

            if (resource != null)
            {
                writer.WriteStartElement("ImageMap");
                value = resource.GetName();
                writer.WriteAttributeString("Value", value);
                writer.WriteEndElement();
            }

            writer.WriteStartElement("CenterAlign");
            writer.WriteAttributeString("Value", Convert.ToInt32(m_isCenterAlign).ToString());
            writer.WriteEndElement();

            writer.WriteEndElement();
        }

        bool Node.isAligned()
        {
            return m_isCenterAlign;
        }

        void Node.load(ref ResourceManager resourceManager, XmlNodeList ChildNodes)
        {
            foreach (XmlNode values in ChildNodes)
            {
                String Value = values.Attributes.Item(0).Value;

                if (values.Name == "ImageMap")
                {
                    ResourceImageDefault resource = resourceManager.GetResource(Value);
                    setResource(ref resource);
                    continue;
                }

                if (values.Name == "Transformation")
                {
                    String[] coords = Value.Split(';');
                    Position.X = Convert.ToInt32(coords[4]);
                    Position.Y = Convert.ToInt32(coords[5]);
                    continue;
                }

                if (values.Name == "CenterAlign")
                {
                    bool isCenterAligned = Convert.ToBoolean(Convert.ToInt32(Value));
                    setCenterAlign(isCenterAligned);
                    continue;
                }
            }
        }

        void Node.setPosition(int x, int y)
        {
            Position.X = x;
            Position.Y = y;
        }

        void Node.draw(ref Graphics g)
        {
            // если нет ресурса, просто рисуем квадрат.
            if (resource == null)
            {
                g.DrawRectangle(DrawPen, m_alignOffset.X + Position.X, m_alignOffset.Y + Position.Y, DrawDebug.ImageNullSize, DrawDebug.ImageNullSize);
                return;
            }

            Image image = resource.GetImage(ImageIndex);

            g.DrawImage(
                    image,
                    new Rectangle(Position.X+m_alignOffset.X, Position.Y + m_alignOffset.Y, image.Width, image.Height),
                    new Rectangle(0, 0, image.Width, image.Height),
                    GraphicsUnit.Pixel);

            g.DrawRectangle(DrawPen, m_alignOffset.X + Position.X, m_alignOffset.Y + Position.Y, image.Width, image.Height);
        }

        void Node.move(int offsetX, int offsetY)
        {
            Position.X += offsetX;
            Position.Y += offsetY;
        }

        void Node.setPosX(int x)
        {
            Position.X = x;
        }

        void Node.setPosY(int y)
        {
            Position.Y = y;
        }

        int Node.getPosX()
        {
            return Position.X;
        }

        int Node.getPosY()
        {
            return Position.Y;
        }

        void Node.select()
        {
            DrawPen = DrawDebug.ImageSelectedSprite;
        }

        void Node.deselect()
        {
            DrawPen = DrawDebug.ImageNullSprite;            
        }

        public Sprite(String name)
        {
            Name = name;
        }

        bool Node.isHotSpot()
        {
            return false;
        }

        public void setImageIndex(int index)
        {
            if (resource != null)
            {
                if (index < resource.GetImageCount())
                {
                    ImageIndex = index;
                }
            }
        }

        public void updateAlign()
        {
            if (m_isCenterAlign == true)
            {
                if (resource == null) return;
                int width = resource.GetImage(0).Width;
                int height = resource.GetImage(0).Height;
                m_alignOffset = new Point(-width / 2, -height / 2);
            }
            else
            {
                m_alignOffset = new Point(0, 0);
            }
        }

        public void setCenterAlign(bool centerAlign)
        {
            m_isCenterAlign = centerAlign;
            updateAlign();
        }

        public void setResource(ref ResourceImageDefault _resource)
        {
            resource = _resource;
            updateAlign();
        }

        private ResourceImageDefault resource = null;
        private Matrix M = new Matrix(1, 0, 0, 1, 0, 0);
        private String Name = "";
        private Point Position = new Point(0, 0);
        private Point m_alignOffset = new Point(0, 0);
        private bool m_isCenterAlign = false;
        private int ImageIndex = 0;
        private Pen DrawPen = DrawDebug.ImageNullSprite;
    }
}
