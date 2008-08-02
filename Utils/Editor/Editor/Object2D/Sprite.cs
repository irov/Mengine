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
        public Sprite(String name)
        {
            Name = name;
        }

        Node m_parent;

        public override Point getLocalPosition()
        {
            return m_localPosition;
        }

        public override Point getWorldPosition()
	    {
		    if( m_parent == null )
		    {
			    return ((Node)this).getLocalPosition();
		    }

            Point parentWorldPosition = m_parent.getWorldPosition();

            Point worldPos = new Point(parentWorldPosition.X + m_localPosition.X,
                parentWorldPosition.Y + m_localPosition.Y);

            return worldPos;
	    }

        public override String getName()
        {
            return Name;
        }

        public override void setName(String name)
        {
            Name = name;
        }

        public override bool isAligned()
        {
            return m_isCenterAlign;
        }

        public override void save(ref XmlTextWriter writer)
        {
            writer.WriteStartElement("Node");


            writer.WriteAttributeString("Name", Name);
            writer.WriteAttributeString("Type", "Sprite");

            writer.WriteStartElement("Transformation");
            String value = "1;0;0;1;" + m_localPosition.X.ToString() + ";" + m_localPosition.Y.ToString();
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

        public override void load(ref ResourceManager resourceManager, XmlNodeList ChildNodes)
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
                    m_localPosition.X = Convert.ToInt32(coords[4]);
                    m_localPosition.Y = Convert.ToInt32(coords[5]);
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

        public override void setPosition(int x, int y)
        {
            m_localPosition.X = x;
            m_localPosition.Y = y;
        }

        public override void draw(ref Graphics g)
        {
            // если нет ресурса, просто рисуем квадрат.
            if (resource == null)
            {
                g.DrawRectangle(DrawPen, m_alignOffset.X + m_localPosition.X, m_alignOffset.Y + m_localPosition.Y, DrawDebug.ImageNullSize, DrawDebug.ImageNullSize);
                return;
            }

            Image image = resource.GetImage(ImageIndex);

            g.DrawImage(
                    image,
                    new Rectangle(m_localPosition.X+m_alignOffset.X, m_localPosition.Y + m_alignOffset.Y, image.Width, image.Height),
                    new Rectangle(0, 0, image.Width, image.Height),
                    GraphicsUnit.Pixel);

            g.DrawRectangle(DrawPen, m_alignOffset.X + m_localPosition.X, m_alignOffset.Y + m_localPosition.Y, image.Width, image.Height);
        }

        public override void translate(int offsetX, int offsetY)
        {
            m_localPosition.X += offsetX;
            m_localPosition.Y += offsetY;
        }

        public override void setPosX(int x)
        {
            m_localPosition.X = x;
        }

        public override void setPosY(int y)
        {
            m_localPosition.Y = y;
        }

        public override int getPosX()
        {
            return m_localPosition.X;
        }

        public override int getPosY()
        {
            return m_localPosition.Y;
        }

        public override void select()
        {
            DrawPen = DrawDebug.ImageSelectedSprite;
        }

        public override void deselect()
        {
            DrawPen = DrawDebug.ImageNullSprite;            
        }

        public override bool isHotSpot()
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
        private String Name = "";
        private Point m_localPosition = new Point(0, 0);
        private Point m_alignOffset = new Point(0, 0);
        private bool m_isCenterAlign = false;
        private int ImageIndex = 0;
        private Pen DrawPen = DrawDebug.ImageNullSprite;
    }
}
