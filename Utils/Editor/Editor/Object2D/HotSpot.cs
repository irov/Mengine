using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;
using System.Xml;

namespace Editor
{
    class HotSpot : Node
    {
        public HotSpot(String name)
        {
            Name = name;
            points.Add(new Point(0, 0));
            points.Add(new Point(DrawDebug.ImageNullSize, 0));
            points.Add(new Point(DrawDebug.ImageNullSize, DrawDebug.ImageNullSize));
            points.Add(new Point(0, DrawDebug.ImageNullSize));
        }

        public HotSpot(String name, bool empty)
        {
            Name = name;
            if(empty == true)
            {
                return;
            }
            points.Add(new Point(0, 0));
            points.Add(new Point(DrawDebug.ImageNullSize, 0));
            points.Add(new Point(DrawDebug.ImageNullSize, DrawDebug.ImageNullSize));
            points.Add(new Point(0, DrawDebug.ImageNullSize));
        }

        bool Node.isHotSpot()
        {
            return true;
        }

        bool Node.isAligned()
        {
            return false;
        }

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
            writer.WriteAttributeString("Type", "HotSpot");

            writer.WriteStartElement("Transformation");
            String value = "1;0;0;1;" + Position.X.ToString() + ";" + Position.Y.ToString();
            writer.WriteAttributeString("Value", value);
            writer.WriteEndElement();

            foreach (Point point in points)
            {
                writer.WriteStartElement("Point");
                value = point.X.ToString() + ";" + point.Y.ToString();
                writer.WriteAttributeString("Value", value);
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }

        void Node.load(ref XmlTextReader reader)
        {

        }

        void Node.setPosition(int x, int y)
        {
            Position.X = x;
            Position.Y = y;

            for (int i = 0; i < points.Count; i++)
            {
                Point pnt = points[i];
                pnt.X += Position.X;
                pnt.Y += Position.Y;
                points[i] = pnt;
            }
        }

        void Node.setPosX(int x)
        {
            for (int i = 0; i < points.Count; i++)
            {
                Point pnt = points[i];

                pnt.X -= Position.X;
                pnt.X += x;
                points[i] = pnt;
            }

            Position.X = x;
        }

        void Node.setPosY(int y)
        {
            for (int i = 0; i < points.Count; i++)
            {
                Point pnt = points[i];

                pnt.Y -= Position.Y;
                pnt.Y += y;
                points[i] = pnt;
            }

            Position.Y = y;
        }

        void Node.draw(ref Graphics g)
        {
            if(points.Count==1)
            {
                g.FillRectangle(DrawDebug.handleBrush, points[0].X - DrawDebug.handleSize / 2, points[0].Y - DrawDebug.handleSize / 2, DrawDebug.handleSize, DrawDebug.handleSize);
                g.DrawRectangle(DrawDebug.handlePen, points[0].X - DrawDebug.handleSize / 2, points[0].Y - DrawDebug.handleSize / 2, DrawDebug.handleSize, DrawDebug.handleSize);
                return;
            }

            g.DrawPolygon(DrawPen, points.ToArray());

            for (int i = 0; i < points.Count; i++)
            {
                g.FillRectangle(DrawDebug.handleBrush, points[i].X - DrawDebug.handleSize / 2, points[i].Y - DrawDebug.handleSize / 2, DrawDebug.handleSize, DrawDebug.handleSize);
                g.DrawRectangle(DrawDebug.handlePen, points[i].X - DrawDebug.handleSize / 2, points[i].Y - DrawDebug.handleSize / 2, DrawDebug.handleSize, DrawDebug.handleSize);
            }
        }

        void Node.move(int offsetX, int offsetY)
        {
            Position.X += offsetX;
            Position.Y += offsetY;

            for (int i = 0; i < points.Count; i++)
            {
                Point pnt = points[i];
                pnt.X += offsetX;
                pnt.Y += offsetY;
                points[i] = pnt;
            }
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
            DrawPen = DrawDebug.PolygonSelected;
        }

        public void AddPoint(Point pt)
        {
            points.Add(pt);
        }

        public void InsertPoint(Point pt)
        {
            double min = Double.MaxValue;

            int at = 0;

            for (int i = 0; i < points.Count; i++)
            {
                Point p1 = (Point)points[i];
                Point p2 = (Point)points[(i + 1) % points.Count];

                double dist = Mathematica.distanceToClosestPoint(p1, p2, pt);

                if (min > dist)
                {
                    min = dist;
                    at = (i+1) % points.Count;
                }
            }

            points.Insert(at, pt);
        }

        public void ErasePoint(int index)
        {
            points.RemoveAt(index);
        }

        public int SelectPoint(Point pt)
        {
            for (int i = 0; i < points.Count; i++)
            {
                Point rectPos = new Point(((Point)points[i]).X - DrawDebug.handleSize / 2, ((Point)points[i]).Y - DrawDebug.handleSize / 2);

                Rectangle rect = new Rectangle(rectPos, new Size(DrawDebug.handleSize, DrawDebug.handleSize));

                bool result = rect.Contains(pt);

                if (result == true)
                {
                    return i;
                }
            }

            return -1;
        }

        public bool Contains(Point rkP)
        {
            bool inside = false;

            for (int i = 0, j = points.Count - 1; i < points.Count; j = i++)
            {
                Point rkU0 = (Point)points[i];
                Point rkU1 = (Point)points[j];
                double fRHS = 0;
                double fLHS = 0;

                if (rkP.Y < rkU1.Y)
                {
                    if (rkU0.Y <= rkP.Y)
                    {
                        fLHS = (rkP.Y - rkU0.Y) * (rkU1.X - rkU0.X);
                        fRHS = (rkP.X - rkU0.X) * (rkU1.Y - rkU0.Y);
                        if (fLHS > fRHS)
                        {
                            inside = !inside;
                        }
                    }
                }
                else if (rkP.Y < rkU0.Y)
                {
                    fLHS = (rkP.Y - rkU0.Y) * (rkU1.X - rkU0.X);
                    fRHS = (rkP.X - rkU0.X) * (rkU1.Y - rkU0.Y);
                    if (fLHS < fRHS)
                    {
                        inside = !inside;
                    }
                }
            }
            return inside;
        }

        public void MovePoint(int i, Point newPos)
        {
            points[i] = newPos;
        }

        private Matrix M = new Matrix(1, 0, 0, 1, 0, 0);
        private String Name = "";
        private Point Position = new Point(0, 0);
        private List<Point> points = new List<Point>();
        private Pen DrawPen = DrawDebug.PolygonSelected;
    }
}
