package eu.janrebe.mazesolver;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import java.util.concurrent.ThreadLocalRandom;

public class MazeView extends View {

    int i;

    public MazePath mazePath = new MazePath();
    Matrix matrix = new Matrix();

    public MazeView(Context context) {
        super(context);
    }

    public MazeView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.TRANSPARENT);

        matrix.reset();
        mazePath.draw(canvas, Color.BLACK, false, matrix);
        mazePath.getSimplifiedPath().draw(canvas, Color.argb(123, 0, 255, 0), true, matrix);
    }
}
