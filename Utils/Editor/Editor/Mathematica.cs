using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace Editor
{
        class Mathematica
        {
            public static float Magnitude(ref PointF Point1, ref PointF Point2)
            {
                PointF Vector = new PointF();
                Vector.X = Point2.X - Point1.X;
                Vector.Y = Point2.Y - Point1.Y;
                return (float)Math.Sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y);
            }

            public static PointF myGetClosestPoint(PointF pt1, PointF pt2, PointF p)
            {
                float u = ((p.X - pt1.X) * (pt2.X - pt1.X) + (p.Y - pt1.Y) * (pt2.Y - pt1.Y)) / ((pt2.X - pt1.X) * (pt2.X - pt1.X) + (pt2.Y - pt1.Y) * (pt2.Y - pt1.Y));

                if (u >= 1.0)
                        return pt2;
                    else if (u <= 0.0)
                        return pt1;

                float x = pt2.X * u + pt1.X * (1.0f - u);
                float y = pt2.Y * u + pt1.Y * (1.0f - u);

                return new PointF(x, y);
            }

            public static double distanceToClosestPoint(PointF pt1, PointF pt2, PointF p)
            {
                PointF pt0 = myGetClosestPoint(pt1, pt2, p);
                return Math.Sqrt((p.X - pt0.X) * (p.X - pt0.X) + (p.Y - pt0.Y) * (p.Y - pt0.Y));
            }
       }
}
