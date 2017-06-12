package eu.janrebe.mazesolver;

public enum Direction {
    forward(0),
    right(90),
    backward(180),
    left(270);

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
