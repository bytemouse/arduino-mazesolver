package eu.janrebe.mazesolver;

import android.graphics.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MazePath {

    // @formatter:off
    public List<Turn> examplePath = new ArrayList<>(Arrays.asList(
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
    ));
    // @formatter:on

    private List<Turn> path;

    public MazePath() {
        path = new ArrayList<>();
    }
    public MazePath(List<Turn> path) {
        this.path = path;
    }

    void addTurn(Turn turn) {
        path.add(turn);
    }

    void clear() {
        path.clear();
    }

    public MazePath getSimplifiedPath() {
        List<Turn> tempPath = new ArrayList<>();
        for (Turn turn : path) {
            tempPath.add(new Turn(turn.direction, turn.timeMillis));
        }

        for (int i = 2; i < tempPath.size(); i++) {
            if (tempPath.get(i - 2).direction != Direction.BACKWARD) {
                continue;
            }

            int totalAngle = 0;

            for (int j = 1; j <= 3; j++) {
                switch (tempPath.get(i - j).direction) {
                    case RIGHT:
                        totalAngle += 90;
                        break;
                    case LEFT:
                        totalAngle += 270;
                        break;
                    case BACKWARD:
                        totalAngle += 180;
                        break;
                }
            }

            // Get the angle as a number between 0 and 360 degrees.
            totalAngle = totalAngle % 360;

            // Replace all of those turns with a single one.
            switch (totalAngle) {
                case 0:
                    tempPath.get(i - 3).direction = Direction.FORWARD;
                    break;
                case 90:
                    tempPath.get(i - 3).direction = Direction.RIGHT;
                    break;
                case 180:
                    tempPath.get(i - 3).direction = Direction.BACKWARD;
                    break;
                case 270:
                    tempPath.get(i - 3).direction = Direction.LEFT;
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
        Direction currentDirection = Direction.FORWARD;

        Point posPixel = new Point();

        Path linePath = new Path();

        linePath.moveTo(posPixel.x, posPixel.y);

        for (Turn turn : path) {
            currentDirection = Direction.getNextDirection(currentDirection, turn.direction);

            int time = turn.timeMillis;

            switch (currentDirection) {
                case LEFT:
                    posPixel.x -= time;
                    break;
                case FORWARD:
                    posPixel.y -= time;
                    break;
                case RIGHT:
                    posPixel.x += time;
                    break;
                case BACKWARD:
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
