package eu.janrebe.mazesolver;

public enum Direction {
    FORWARD(0),
    RIGHT(90),
    BACKWARD(180),
    LEFT(270);

    public int degrees;

    Direction(int degrees) {
        this.degrees = degrees;
    }

    static Direction getNextDirection(Direction oldDirection, Direction relativeTurn) {
        return Direction.getWithDegrees((oldDirection.degrees + relativeTurn.degrees) % 360);
    }


    static Direction getWithDegrees(int degrees) {
        for (Direction direction : Direction.values()) {
            if (direction.degrees == degrees) {
                return direction;
            }
        }
        return null;
    }
}
