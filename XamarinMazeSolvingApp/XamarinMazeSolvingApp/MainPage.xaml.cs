using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace XamarinMazeSolvingApp
{
    public partial class MainPage : ContentPage
    {
        Path fullPath = new Path(new List<Turn>{
            new Turn(Direction.forward,   500),
            new Turn(Direction.right,     500),
            new Turn(Direction.forward,   500),
            new Turn(Direction.left,      500),
            new Turn(Direction.left,      500),
            new Turn(Direction.forward,   500),
            new Turn(Direction.backward,  500),
            new Turn(Direction.left,      500),
            new Turn(Direction.left,      500),
            new Turn(Direction.backward,  500),
            new Turn(Direction.right,     500),
            new Turn(Direction.left,      500),
            new Turn(Direction.left,      500),
            new Turn(Direction.right,     500),
            new Turn(Direction.right,     500),
            new Turn(Direction.backward,  500),
            new Turn(Direction.left,      500),
            new Turn(Direction.left,      500),
            new Turn(Direction.forward,   500),
            new Turn(Direction.right,     500),
            new Turn(Direction.left,      500),
            new Turn(Direction.left,     1500),
            new Turn(Direction.left,      500)
        });

        public MainPage()
        {
            InitializeComponent();
        }

        private void OnPainting(object sender, SKPaintSurfaceEventArgs paintSurface)
        {
            paintSurface.Surface.Canvas.Clear();

            fullPath.Draw(paintSurface, SKColors.Black);
            fullPath.GetSimplifiedPath().Draw(paintSurface, SKColors.Green);
        }
    }
}
