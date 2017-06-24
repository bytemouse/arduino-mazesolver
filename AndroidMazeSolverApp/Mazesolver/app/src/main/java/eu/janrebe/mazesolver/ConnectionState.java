package eu.janrebe.mazesolver;

import android.graphics.Color;

public enum ConnectionState {
    DISCONNECTED(Color.RED),
    SEARCHING(Color.RED),
    CONNECTING(Color.YELLOW),
    CONNECTED_NOT_LISTENING(Color.YELLOW),
    CONNECTED_LISTENING(Color.GREEN);

    int color;

    ConnectionState(int color) {
        this.color = color;
    }
}
