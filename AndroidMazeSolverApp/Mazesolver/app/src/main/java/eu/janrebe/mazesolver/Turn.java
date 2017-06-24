package eu.janrebe.mazesolver;

public class Turn {
    public Direction direction;
    public int timeMillis;

    public Turn(Direction direction, int timeMillis)
    {
        this.direction = direction;
        this.timeMillis = timeMillis;
    }

    @Override
    public String toString() {
        return "Turn: direction: " + direction.toString() + ", time: " + timeMillis;
    }
}
