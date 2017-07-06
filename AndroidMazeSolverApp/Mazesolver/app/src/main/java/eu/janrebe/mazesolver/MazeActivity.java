package eu.janrebe.mazesolver;

import android.Manifest;
import android.bluetooth.*;
import android.bluetooth.le.*;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.Collections;
import java.util.Timer;
import java.util.UUID;

public class MazeActivity extends AppCompatActivity {

    private final static String TAG = MazeActivity.class.getSimpleName();

    private final static UUID SERVICE_UUID = UUID.fromString("0000ffe0-0000-1000-8000-00805f9b34fb");
    private final static UUID CHARACTERISTIC_UUID = UUID.fromString("0000ffe1-0000-1000-8000-00805f9b34fb");
    private final static String VEHICLE_MAC_ADRESS = "50:F1:4A:50:A8:09";

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bleScanner;

    private BluetoothGatt vehicleGatt;
    private BluetoothGattService vehicleService;
    BluetoothGattCharacteristic vehicleCharacteristic;

    TextView textViewContent;
    TextView textViewState;

    MazeView mazeView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maze);

        mazeView = findViewById(R.id.mazeView);
        textViewState = findViewById(R.id.textViewState);
        textViewContent = findViewById(R.id.textViewContent);

        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);

        bluetoothAdapter = bluetoothManager.getAdapter();
        bleScanner = bluetoothAdapter.getBluetoothLeScanner();

        searchForDevices();

        setStateText(ConnectionState.SEARCHING);
    }


    private void searchForDevices() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, RESULT_CANCELED);
        }

        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, 0);
        }

        final ScanFilter scanFilter = new ScanFilter.Builder()
                .setDeviceAddress(VEHICLE_MAC_ADRESS)
                .build();

        final ScanSettings scanSettings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                .build();

        bleScanner.startScan(Collections.singletonList(scanFilter), scanSettings, scanCallback);
    }

    private ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            bleScanner.stopScan(scanCallback);

            BluetoothDevice vehicle = result.getDevice();

            vehicleGatt = vehicle.connectGatt(MazeActivity.this, true, connectedCallback);
            vehicleGatt.connect();


            setStateText(ConnectionState.CONNECTING);
        }

        @Override
        public void onScanFailed(int errorCode) {
            setTextViewText("Scan failed with error code " + errorCode, false);
        }
    };

    int receiveIndex;

    protected BluetoothGattCallback connectedCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            switch (newState) {
                case BluetoothProfile.STATE_CONNECTED:
                    gatt.discoverServices();

                    setStateText(ConnectionState.CONNECTED_NOT_LISTENING);
                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    setStateText(ConnectionState.DISCONNECTED);
                    break;
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            for (BluetoothGattService service : gatt.getServices()) {
                if (service.getUuid().equals(SERVICE_UUID)) {
                    vehicleService = service;
                }
            }
            vehicleCharacteristic = vehicleService.getCharacteristic(CHARACTERISTIC_UUID);

            vehicleGatt.setCharacteristicNotification(vehicleCharacteristic, true);
            setStateText(ConnectionState.CONNECTED_LISTENING);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            int[] receivedUnsignedBytes = getUnsigned(characteristic.getValue());

            if (receivedUnsignedBytes.length == 1 && receivedUnsignedBytes[0] == BluetoothByte.CLEAR_MAZE_VIEW_REQUEST.byteValue) {
                mazeView.mazePath.clear();
                mazeView.postInvalidate();
                return;
            } else if (receivedUnsignedBytes.length != 5) {
                return;
            }

            setTextViewText("received new byte array nr " + receiveIndex + " : " + Arrays.toString(receivedUnsignedBytes), true);
            sendToVehicle(new byte[]{(byte) BluetoothByte.OK.byteValue, (byte) receivedUnsignedBytes[1]});
            receiveIndex++;

            Turn turn = new Turn(Direction.getFromUnsignedByte(receivedUnsignedBytes[2]), receivedUnsignedBytes[3] * 50);

            mazeView.mazePath.addTurn(turn);
            mazeView.postInvalidate();

            setTextViewText(turn.toString(), true);
        }
    };

    private void setTextViewText(final String string, final boolean append) {
        textViewContent.post(new Runnable() {
            @Override
            public void run() {
                if (append) {
                    textViewContent.append(string + "\n");
                } else {
                    textViewContent.setText(string + "\n");
                }

                ScrollView scrollView = (ScrollView) textViewContent.getParent();
                scrollView.smoothScrollTo(0, textViewContent.getBottom());
            }
        });

        Log.i(TAG, string);
    }

    public static int getUnsigned(byte b) {
        return b & 0xFF;
    }

    public static int[] getUnsigned(byte[] bytes) {
        int[] tmpBytes = new int[bytes.length];

        for (int i = 0; i < bytes.length; i++) {
            tmpBytes[i] = getUnsigned(bytes[i]);
        }
        return tmpBytes;
    }

    void sendToVehicle(int value) {
        sendToVehicle(new byte[]{(byte) value});
    }

    void sendToVehicle(byte[] value) {
        if (state == ConnectionState.CONNECTED_LISTENING) {
            vehicleCharacteristic.setValue(value);
            vehicleGatt.writeCharacteristic(vehicleCharacteristic);
        } else {
            Toast.makeText(this, "not connected", Toast.LENGTH_SHORT).show();
        }
    }

    public void sendStart(View view) {
        sendToVehicle(BluetoothByte.START_DRIVING_REQUEST.byteValue);
    }

    public void sendStop(View view) {
        sendToVehicle(BluetoothByte.STOP_DRIVING_REQUEST.byteValue);
    }

    ConnectionState state;

    void setStateText(final ConnectionState state) {
        this.state = state;
        textViewState.post(new Runnable() {
            @Override
            public void run() {
                textViewState.setTextColor(state.color);
                textViewState.setText(getResources().getString(state.stringIdentifier));
            }
        });

        Log.i(TAG, state.toString());
    }
}
