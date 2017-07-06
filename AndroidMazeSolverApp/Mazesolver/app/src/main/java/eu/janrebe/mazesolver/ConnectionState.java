package eu.janrebe.mazesolver;

import android.graphics.Color;

public enum ConnectionState {
    DISCONNECTED(Color.RED, R.string.disconnected),
    SEARCHING(Color.RED, R.string.searching),
    CONNECTING(Color.YELLOW, R.string.connecting),
    CONNECTED_NOT_LISTENING(Color.YELLOW, R.string.connected_not_listening),
    CONNECTED_LISTENING(Color.GREEN, R.string.connected);

    int color;
    int stringIdentifier;

    ConnectionState(int color, int stringIdentifier) {
        this.color = color;
        this.stringIdentifier = stringIdentifier;
    }
}
