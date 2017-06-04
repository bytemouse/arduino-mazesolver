using System;
using System.Collections.Generic;
using System.Text;

namespace XamarinMazeSolvingApp
{
    class Turn
    {
        public Direction direction;
        public TimeSpan time;

        public Turn(Direction direction, int millis)
        {
            this.direction = direction;
            time = TimeSpan.FromMilliseconds(millis);
        }
    }
}
