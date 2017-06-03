using System;
using System.Collections.Generic;
using System.Text;

namespace XamarinMazeSolvingApp
{
    class Path
    {
        public static List<Direction> fullPath = new List<Direction>{
            Direction.forward,
            Direction.right,
            Direction.forward,
            Direction.left,
            Direction.left,
            Direction.forward,
            Direction.backward,
            Direction.left,
            Direction.left,
            Direction.backward,
            Direction.right,
            Direction.left,
            Direction.left,
            Direction.right,
            Direction.right,
            Direction.backward,
            Direction.left,
            Direction.left,
            Direction.forward,
            Direction.right,
            Direction.left,
            Direction.left,
            Direction.left
    };
    }
}
