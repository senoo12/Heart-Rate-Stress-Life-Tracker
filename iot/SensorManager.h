#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <math.h>
#include <WString.h>

class SensorManager {
private:
  MAX30105 sensor;

  // Variabel detak & HRV
  const byte RATE_SIZE = 4;
  byte rates[4];
  byte rateSpot = 0;
  long lastBeat = 0;
  float beatsPerMinute;
  int beatAvg;

  const int INTERVAL_SIZE = 30;
  long intervals[30];
  int intervalCount = 0;
  bool fingerDetected = false;

  // Perhitungan SpO2
  long redValue = 0;

  // Kestabilan & freeze
  int beatCount = 0;
  const int MIN_BEATS = 8;
  static const int HISTORY_SIZE = 3;
  int bpmHistory[HISTORY_SIZE];
  int spo2History[HISTORY_SIZE];
  int historyIndex = 0;

  bool measurementDone = false;
  bool doneNotified = false;
  bool frozen = false;
  int frozenBPM = 0;
  int frozenSpO2 = 0;
  float frozenRMSSD = 0;
  float frozenSDRR = 0;
  int frozenPNN50 = 0;

  // Variabel sementara
  int spo2;
  float rmssd, sdrr;
  int pnn50;
  String finalStatusLabel = "Membaca...";

public:
  void init();
  void updateSensor();
  void calculateHRV();
  void checkStability();
  void freezeValues();
  void reset();

  int getBPM();
  int getSpO2();
  float getRMSSD();
  float getSDRR();
  int getPNN50();
  bool isFingerDetected();
  bool isMeasurementDone();
  bool isFrozen();
  bool isDoneNotified();
  void setDoneNotified(bool value);
  String getStatus();

  void setFinalStatus(String label) {
    label.toUpperCase();
    finalStatusLabel = label;
  }
};

#endif