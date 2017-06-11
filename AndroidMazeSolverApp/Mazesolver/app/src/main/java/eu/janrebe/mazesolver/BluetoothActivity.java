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
import android.widget.ScrollView;
import android.widget.TextView;

import java.util.Collections;
import java.util.UUID;

public class BluetoothActivity extends AppCompatActivity {

    private final static String TAG = BluetoothActivity.class.getSimpleName();

    private final static UUID SERVICE_UUID = UUID.fromString("0000ffe0-0000-1000-8000-00805f9b34fb");
    private final static UUID CHARACTERISTIC_UUID = UUID.fromString("0000ffe1-0000-1000-8000-00805f9b34fb");
    private final static String VEHICLE_MAC_ADRESS = "50:F1:4A:50:A8:09";

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bleScanner;

    private BluetoothGatt vehicleGatt;
    private BluetoothGattService vehicleService;

    TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);

        textView = (TextView) findViewById(R.id.textView);

        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);

        bluetoothAdapter = bluetoothManager.getAdapter();
        bleScanner = bluetoothAdapter.getBluetoothLeScanner();

        searchForDevices();

        textView.setText("Searching for device...");
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

            vehicleGatt = vehicle.connectGatt(BluetoothActivity.this, true, connectedCallback);
            vehicleGatt.connect();


            setTextViewText("Connecting to device...", false);
        }

        @Override
        public void onScanFailed(int errorCode) {
            setTextViewText("Scan failed with error code " + errorCode, false);
        }
    };


    private BluetoothGattCallback connectedCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            switch (newState) {
                case BluetoothProfile.STATE_CONNECTED:
                    gatt.discoverServices();

                    setTextViewText("Connected", false);
                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    setTextViewText("disconnected", false);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            for (BluetoothGattService service : gatt.getServices()) {
                if (service.getUuid().equals(SERVICE_UUID)) {
                    vehicleService = service;
                }
            }
            BluetoothGattCharacteristic vehicleCharacteristic = vehicleService.getCharacteristic(CHARACTERISTIC_UUID);

            vehicleGatt.setCharacteristicNotification(vehicleCharacteristic, true);
            setTextViewText("listening to notifications started", false);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            setTextViewText("new state: " + characteristic.getStringValue(0), true);
        }
    };

    private void setTextViewText(final String string, final boolean append) {
        textView.post(new Runnable() {
            @Override
            public void run() {
                if (append) {
                    textView.append(string + "\n");
                } else {
                    textView.setText(string + "\n");
                }
            }
        });
    }
}
