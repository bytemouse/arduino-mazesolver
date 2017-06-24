package eu.janrebe.mazesolver;

public enum BluetoothByte {
    // @formatter:off
    STARTING(              255),
    LEFT(                  250,    Direction.LEFT),
    FORWARD(               251,    Direction.FORWARD),
    RIGHT(                 252,    Direction.RIGHT),
    BACKWARD(              253,    Direction.BACKWARD),
    FINISHED(              254),
    OK(                    249),
    START_DRIVING_REQUEST( 247),
    STOP_DRIVING_REQUEST(  248);

    // @formatter:on
    
    int byteValue;
    Direction direction;

    BluetoothByte(int byteValue, Direction direction) {
        this.byteValue = byteValue;
        this.direction = direction;
    }

    BluetoothByte(int byteValue) {
        this.byteValue = byteValue;
    }
}
