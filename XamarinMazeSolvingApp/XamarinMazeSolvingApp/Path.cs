using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace XamarinMazeSolvingApp
{
    class Path
    {
        List<Turn> path;

        public Path(List<Turn> path)
        {
            this.path = path;
        }

        public Path GetSimplifiedPath()
        {
            var tempPath = new List<Turn>(path.Select(x => new Turn(x.direction, (int)x.time.TotalMilliseconds)));

            for (int i = 2; i < tempPath.Count; i++)
            {
                if (tempPath[i - 2].direction != Direction.backward)
                {
                    continue;
                }

                int totalAngle = 0;

                for (int j = 1; j <= 3; j++)
                {
                    switch (tempPath[i - j].direction)
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
                        tempPath[i - 3].direction = Direction.forward;
                        break;
                    case 90:
                        tempPath[i - 3].direction = Direction.right;
                        break;
                    case 180:
                        tempPath[i - 3].direction = Direction.backward;
                        break;
                    case 270:
                        tempPath[i - 3].direction = Direction.left;
                        break;
                }

                // The path is now two steps shorter.
                tempPath.RemoveAt(i - 1);
                tempPath.RemoveAt(i - 2);

                i -= 2;
            }

            return new Path(tempPath);
        }

        private static Direction GetNextDirection(Direction oldDirection, Direction relativeTurn)
        {
            return (Direction)(((int)oldDirection + (int)relativeTurn) % Enum.GetValues(typeof(Direction)).Length);
        }

        public void Draw(SKPaintSurfaceEventArgs paintSurface, SKColor color)
        {
            Vec2 pos = new Vec2(paintSurface.Info.Width / 2, paintSurface.Info.Height / 2);
            Vec2 nextPos = pos;

            const float stepSize = 0.1f;
            var pathPaint = new SKPaint { Color = color, IsStroke = true, StrokeWidth = 10 };
            var currentDirection = Direction.right;

            foreach (var turn in path)
            {
                currentDirection = GetNextDirection(currentDirection, turn.direction);

                var time = turn.time.TotalMilliseconds;

                switch (currentDirection)
                {
                    case Direction.left:
                        nextPos.X -= stepSize * time;
                        break;
                    case Direction.forward:
                        nextPos.Y -= stepSize * time;
                        break;
                    case Direction.right:
                        nextPos.X += stepSize * time;
                        break;
                    case Direction.backward:
                        nextPos.Y += stepSize * time;
                        break;

                }
                paintSurface.Surface.Canvas.DrawLine((float)pos.X, (float)pos.Y, (float)nextPos.X, (float)nextPos.Y, pathPaint);
                pos = nextPos;
            }
        }
    }
}
