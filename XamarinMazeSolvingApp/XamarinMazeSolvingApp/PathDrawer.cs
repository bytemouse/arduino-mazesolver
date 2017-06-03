using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;

namespace XamarinMazeSolvingApp
{
    class PathDrawer
    {
        private static Direction GetNextDirection(Direction oldDirection, Direction relativeTurn)
        {
            return (Direction)(((int)oldDirection + (int)relativeTurn) % Enum.GetValues(typeof(Direction)).Length);
        }

        public static void DrawPath(SKPaintSurfaceEventArgs paintSurface, List<Direction> path, SKColor color)
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
