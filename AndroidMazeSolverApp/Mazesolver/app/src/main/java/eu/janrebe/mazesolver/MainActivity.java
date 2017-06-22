package eu.janrebe.mazesolver;

import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.util.ArrayList;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    // @formatter:off
    MazePath fullPath = new MazePath(new ArrayList<>(Arrays.asList(
            new Turn(Direction.FORWARD,   500),
            new Turn(Direction.RIGHT,     500),
            new Turn(Direction.FORWARD,   500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.FORWARD,   500),
            new Turn(Direction.BACKWARD,  500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.BACKWARD,  500),
            new Turn(Direction.RIGHT,     500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.RIGHT,     500),
            new Turn(Direction.RIGHT,     500),
            new Turn(Direction.BACKWARD,  500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.FORWARD,   500),
            new Turn(Direction.RIGHT,     500),
            new Turn(Direction.LEFT,      500),
            new Turn(Direction.LEFT,      800),
            new Turn(Direction.LEFT,      500)
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

    public void startBluetoothActivity(View v) {
        Intent intent = new Intent(this, BluetoothActivity.class);
        startActivity(intent);
    }
}
