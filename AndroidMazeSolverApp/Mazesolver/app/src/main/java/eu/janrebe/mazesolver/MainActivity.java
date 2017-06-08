package eu.janrebe.mazesolver;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.ArrayList;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // @formatter:off
    MazePath fullPath = new MazePath(new ArrayList<Turn>(Arrays.asList(
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
            new Turn(Direction.left,      800),
            new Turn(Direction.left,      500)
    )));
    // @formatter:on


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView surface = (SurfaceView) findViewById(R.id.surfaceView);
        surface.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Canvas canvas = holder.lockCanvas();

                Matrix matrix = new Matrix();
                canvas.drawColor(Color.WHITE);
                fullPath.draw(canvas, Color.BLACK, false, matrix);
                fullPath.getSimplifiedPath().draw(canvas, Color.GREEN, true, matrix);

                holder.unlockCanvasAndPost(canvas);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }
}
