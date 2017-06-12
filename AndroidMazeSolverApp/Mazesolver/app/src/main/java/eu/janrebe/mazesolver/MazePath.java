package eu.janrebe.mazesolver;

import android.graphics.*;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class MazePath {
    List<Turn> path;

    public MazePath(List<Turn> path) {
        this.path = path;
    }

    public MazePath getSimplifiedPath() {
        List<Turn> tempPath = new ArrayList<>();
        for (Turn turn : path) {
            tempPath.add(new Turn(turn.direction, turn.timeMillis));
        }

        for (int i = 2; i < tempPath.size(); i++) {
            if (tempPath.get(i - 2).direction != Direction.backward) {
                continue;
            }

            int totalAngle = 0;

            for (int j = 1; j <= 3; j++) {
                switch (tempPath.get(i - j).direction) {
                    case right:
                        totalAngle += 90;
                        break;
                    case left:
                        totalAngle += 270;
                        break;
                    case backward:
                        totalAngle += 180;
                        break;
                }
            }

            // Get the angle as a number between 0 and 360 degrees.
            totalAngle = totalAngle % 360;

            // Replace all of those turns with a single one.
            switch (totalAngle) {
                case 0:
                    tempPath.get(i - 3).direction = Direction.forward;
                    break;
                case 90:
                    tempPath.get(i - 3).direction = Direction.right;
                    break;
                case 180:
                    tempPath.get(i - 3).direction = Direction.backward;
                    break;
                case 270:
                    tempPath.get(i - 3).direction = Direction.left;
                    break;
            }

            // The path is now two steps shorter.
            tempPath.remove(i - 1);
            tempPath.remove(i - 2);

            i -= 2;
        }

        return new MazePath(tempPath);
    }

    //https://forums.xamarin.com/discussion/77883/skiasharp-graphics-basics
    public void draw(Canvas canvas, int color, boolean isTransformMatrixSet, Matrix transformMatrix) {
        Direction currentDirection = Direction.right;

        Point posPixel = new Point();

        Path linePath = new Path();

        linePath.moveTo(posPixel.x, posPixel.y);

        for (Turn turn : path) {
            currentDirection = Direction.getNextDirection(currentDirection, turn.direction);

            int time = turn.timeMillis;

            switch (currentDirection) {
                case left:
                    posPixel.x -= time;
                    break;
                case forward:
                    posPixel.y -= time;
                    break;
                case right:
                    posPixel.x += time;
                    break;
                case backward:
                    posPixel.y += time;
                    break;
            }

            linePath.lineTo(posPixel.x, posPixel.y);
        }

        if (!isTransformMatrixSet) {
            RectF linePathRect = new RectF();
            linePath.computeBounds(linePathRect, false);

            Rect drawPathRect = new Rect(0, 0, Math.min(canvas.getWidth(), canvas.getHeight()), Math.min(canvas.getWidth(), canvas.getHeight()));

            float scalingFactor = drawPathRect.width() / linePathRect.width();

            transformMatrix.postTranslate(-linePathRect.left, -linePathRect.top);
            transformMatrix.postScale(scalingFactor, scalingFactor);
            transformMatrix.postTranslate(drawPathRect.left, drawPathRect.top);
        }

        linePath.transform(transformMatrix);

        Paint paint = new Paint();
        paint.setColor(color);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(30);
        canvas.drawPath(linePath, paint);
    }
}
