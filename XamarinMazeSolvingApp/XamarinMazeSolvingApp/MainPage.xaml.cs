using SkiaSharp;
using SkiaSharp.Views.Forms;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace XamarinMazeSolvingApp
{
	public partial class MainPage : ContentPage
	{
		public MainPage()
		{
			InitializeComponent();
		}

        private void OnPainting(object sender, SKPaintSurfaceEventArgs paíntSurface)
        {
            paíntSurface.Surface.Canvas.Clear();
            PathDrawer.DrawPath(paíntSurface, Path.fullPath, SKColors.Black);

            var simplifiedPath = new List<Direction>();
            foreach (Direction turn in Path.fullPath)
            {
                simplifiedPath.Add(turn);
                simplifiedPath = PathSimplifier.SimplifyMaze(simplifiedPath);
            }

            PathDrawer.DrawPath(paíntSurface, simplifiedPath, SKColors.AliceBlue);
        }
    }
}
