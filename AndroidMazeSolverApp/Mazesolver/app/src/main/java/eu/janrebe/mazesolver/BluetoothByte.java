package eu.janrebe.mazesolver;

public enum BluetoothByte {
    // @formatter:off
    STARTING((byte) 255),
    LEFT((byte)     250,    Direction.LEFT),
    FORWARD((byte)  251,    Direction.FORWARD),
    RIGHT((byte)    252,    Direction.RIGHT),
    BACKWARD((byte) 253,    Direction.BACKWARD),
    FINISHED((byte) 254),
    OK((byte)       249);
    // @formatter:on
    
    byte byteValue;
    Direction direction;

    BluetoothByte(byte byteValue, Direction direction) {
        this.byteValue = byteValue;
        this.direction = direction;
    }

    BluetoothByte(byte byteValue) {
        this.byteValue = byteValue;
    }
}
