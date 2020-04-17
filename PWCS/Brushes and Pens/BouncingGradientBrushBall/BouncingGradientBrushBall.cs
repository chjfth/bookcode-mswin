//--------------------------------------------------------
// BouncingGradientBrushBall.cs © 2001 by Charles Petzold
//--------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class BouncingGradientBrushBall: Bounce
{
	public new static void Main()
	{
		Application.Run(new BouncingGradientBrushBall());
	}
	public BouncingGradientBrushBall()
	{
		Text = "Bouncing Gradient Brush Ball";
	}
	protected override void DrawBall(Graphics grfx, Rectangle rect)
	{
		GraphicsPath path = new GraphicsPath();
		path.AddEllipse(rect);

		PathGradientBrush pgbrush = new PathGradientBrush(path);
		pgbrush.CenterPoint = new PointF((rect.Left + rect.Right) / 3,
			(rect.Top + rect.Bottom) / 3);
		pgbrush.CenterColor = Color.White;
		pgbrush.SurroundColors = new Color[] { Color.Red };

		grfx.FillRectangle(pgbrush, rect);
	}
}
