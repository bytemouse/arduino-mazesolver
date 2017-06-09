package eu.janrebe.mazesolver;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.*;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.ParcelUuid;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.util.*;

public class BluetoothActivity extends AppCompatActivity {

    final static String MAZESOLVER_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";

    BluetoothAdapter bluetoothAdapter;
    BluetoothLeScanner ble;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);


        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);

        bluetoothAdapter = bluetoothManager.getAdapter();
        ble = bluetoothAdapter.getBluetoothLeScanner();

        searchForDevices();
    }


    void searchForDevices() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, RESULT_CANCELED);
        }

        Log.d("TAHG", "OAKok");

        int REQUEST_ENABLE_BT = 0;
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        final ScanFilter scanFilter = new ScanFilter.Builder()
//                .setServiceUuid(ParcelUuid.fromString(MAZESOLVER_UUID))
                .build();

        final ScanSettings scanSettings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                .build();

        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                ble.startScan(Collections.singletonList(scanFilter), scanSettings, scanCallback);
            }
        });

    }

    ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);

            Log.d("TAG", result.getDevice().getName() + "; UUIDs: " + result.getScanRecord().getServiceUuids().toString());
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);

            Log.d("TAG", "BatchResults");
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
            Log.d("TAG", "Scan failed");
        }
    };
}
