using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace XamarinMazeSolvingApp
{
    class Path
    {
        List<Direction> path;

        public Path(List<Direction> path)
        {
            this.path = path;
        }

        public Path GetSimplifiedPath()
        {
            var tempPath = new List<Direction>();
            tempPath.AddRange(path);

            for (int i = 2; i < tempPath.Count; i++)
            {
                if (tempPath[i - 2] != Direction.backward)
                {
                    continue;
                }


                int totalAngle = 0;

                for (int j = 1; j <= 3; j++)
                {
                    switch (tempPath[i - j])
                    {
                        case Direction.right:
                            totalAngle += 90;
                            break;
                        case Direction.left:
                            totalAngle += 270;
                            break;
                        case Direction.backward:
                            totalAngle += 180;
                            break;
                    }
                }

                // Get the angle as a number between 0 and 360 degrees.
                totalAngle = totalAngle % 360;

                // Replace all of those turns with a single one.
                switch (totalAngle)
                {
                    case 0:
                        tempPath[i - 3] = Direction.forward;
                        break;
                    case 90:
                        tempPath[i - 3] = Direction.right;
                        break;
                    case 180:
                        tempPath[i - 3] = Direction.backward;
                        break;
                    case 270:
                        tempPath[i - 3] = Direction.left;
                        break;
                }

                // The path is now two steps shorter.
                tempPath.RemoveAt(i - 1);
                tempPath.RemoveAt(i - 2);

                i -= 2;
            }
            
            return new Path(new List<Direction>(tempPath));
        }

        private static Direction GetNextDirection(Direction oldDirection, Direction relativeTurn)
        {
            return (Direction)(((int)oldDirection + (int)relativeTurn) % Enum.GetValues(typeof(Direction)).Length);
        }

        public void Draw(SKPaintSurfaceEventArgs paintSurface, SKColor color)
        {
            Vec2 pos = new Vec2(paintSurface.Info.Width / 2, paintSurface.Info.Height / 2);
            Vec2 nextPos = pos;

            const float stepSize = 40;
            var pathPaint = new SKPaint { Color = color, IsStroke = true, StrokeWidth = 10 };
            var currentDirection = Direction.right;

            foreach (Direction turn in path)
            {
                currentDirection = GetNextDirection(currentDirection, turn);
                switch (currentDirection)
                {
                    case Direction.left:
                        nextPos.X -= stepSize;
                        break;
                    case Direction.forward:
                        nextPos.Y -= stepSize;
                        break;
                    case Direction.right:
                        nextPos.X += stepSize;
                        break;
                    case Direction.backward:
                        nextPos.Y += stepSize;
                        break;

                }
                paintSurface.Surface.Canvas.DrawLine((float)pos.X, (float)pos.Y, (float)nextPos.X, (float)nextPos.Y, pathPaint);
                pos = nextPos;
            }
        }
    }
}
