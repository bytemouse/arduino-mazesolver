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
        Path fullPath = new Path(new List<Direction>{
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
