package org.Mengine.Base;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public class MengineSensor implements SensorEventListener {
    private SensorManager m_sensorManager;
    private Sensor m_accelerometer;
    private Sensor m_linearAccelerometer;

    public MengineSensor(Context context) {
        m_sensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        if (m_sensorManager != null) {
            m_accelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            m_linearAccelerometer = m_sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        }
    }

    public void handlePause() {
        if (m_sensorManager != null) {
            m_sensorManager.unregisterListener(this, m_accelerometer);
            m_sensorManager.unregisterListener(this, m_linearAccelerometer);
        }
    }

    public void handleResume() {
        if (m_sensorManager != null) {
            m_sensorManager.registerListener(this, m_accelerometer, SensorManager.SENSOR_DELAY_GAME);
            m_sensorManager.registerListener(this, m_linearAccelerometer, SensorManager.SENSOR_DELAY_GAME);
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        int type = event.sensor.getType();

        switch (type) {
            case Sensor.TYPE_ACCELEROMETER: {
                /*
                MengineSurface.onNativeAccel(
                    event.values[0] / SensorManager.GRAVITY_EARTH,
                    event.values[1] / SensorManager.GRAVITY_EARTH,
                    event.values[2] / SensorManager.GRAVITY_EARTH);
                 */
            }break;
            case Sensor.TYPE_LINEAR_ACCELERATION: {
                /*
                MengineSurface.onNativeLinearAccel(
                    event.values[0] / SensorManager.GRAVITY_EARTH,
                    event.values[1] / SensorManager.GRAVITY_EARTH,
                    event.values[2] / SensorManager.GRAVITY_EARTH);
                 */
            }break;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        //ToDo: Handle accuracy changes here if needed
    }
}