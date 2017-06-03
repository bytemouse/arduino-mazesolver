using System;
using System.Collections.Generic;
using System.Text;

namespace XamarinMazeSolvingApp
{
    class PathSimplifier
    {
        public static List<Direction> SimplifyMaze(List<Direction> path)
        {
            var tempPath = new List<Direction>();
            tempPath.AddRange(path);

            if (tempPath.Count < 3 || tempPath[tempPath.Count - 2] != Direction.backward)
            {
                return tempPath;
            }

            int totalAngle = 0;

            for (int i = 1; i <= 3; i++)
            {
                switch (tempPath[tempPath.Count - i])
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
                    tempPath[tempPath.Count - 3] = Direction.forward;
                    break;
                case 90:
                    tempPath[tempPath.Count - 3] = Direction.right;
                    break;
                case 180:
                    tempPath[tempPath.Count - 3] = Direction.backward;
                    break;
                case 270:
                    tempPath[tempPath.Count - 3] = Direction.left;
                    break;
            }

            // The path is now two steps shorter.
            tempPath.RemoveAt(tempPath.Count - 1);
            tempPath.RemoveAt(tempPath.Count - 1);

            return tempPath;
        }
    }
}
