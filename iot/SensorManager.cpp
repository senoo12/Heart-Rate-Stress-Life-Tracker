#include "SensorManager.h"

void SensorManager::init() {
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    while (1);
  }
  sensor.setup();

  sensor.setPulseAmplitudeRed(0x0A);
  sensor.setPulseAmplitudeGreen(0);

  // Reset variabel
  reset();
}

void SensorManager::updateSensor() {
  long irValue = sensor.getIR();
  //long redValue = sensor.getRed();

  // Deteksi jari
  if (irValue < 50000) {
    if (fingerDetected) {
      fingerDetected = false;
      reset();
    }
    return;
  } else if (!fingerDetected) {
    fingerDetected = true;
    beatCount = 0;
    historyIndex = 0;
    measurementDone = false;
    doneNotified = false;
    frozen = false;
    finalStatusLabel = "MEMBACA...";
  }

  // Detak jantung
  if (!frozen && checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 40 && beatsPerMinute < 220) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) beatAvg += rates[i];
      beatAvg /= RATE_SIZE;

      if (intervalCount < INTERVAL_SIZE) intervals[intervalCount++] = delta;
      else {
        memmove(intervals, intervals + 1, (INTERVAL_SIZE - 1) * sizeof(long));
        intervals[INTERVAL_SIZE - 1] = delta;
      }
      beatCount++;
    }
  }

  spo2 = constrain(map(irValue, 50000, 300000, 90, 100), 90, 100);
}

void SensorManager::calculateHRV() {
  rmssd = 0;
  sdrr = 0;
  pnn50 = 0;

  if (intervalCount > 5 && !frozen) {
    // RMSSD
    float sumSq = 0;
    for (int i = 1; i < intervalCount; i++) {
      float diff = intervals[i] - intervals[i - 1];
      sumSq += diff * diff;
    }
    rmssd = sqrt(sumSq / (intervalCount - 1));
    rmssd = rmssd / 10;

    // SDRR
    float mean = 0;
    for (int i = 0; i < intervalCount; i++) mean += intervals[i];
    mean /= intervalCount;
    float var = 0;
    for (int i = 0; i < intervalCount; i++) var += sq(intervals[i] - mean);
    sdrr = sqrt(var / intervalCount);
    sdrr = sdrr / 10;

    // pNN50
    int nn50 = 0;
    for (int i = 1; i < intervalCount; i++) {
      if (abs(intervals[i] - intervals[i - 1]) > 50) nn50++;
    }
    pnn50 = (nn50 * 100) / (intervalCount - 1);
    pnn50 = pnn50 / 10;

  } else if (frozen) {
    rmssd = frozenRMSSD;
    sdrr = frozenSDRR;
    pnn50 = frozenPNN50;
    beatAvg = frozenBPM;
    spo2 = frozenSpO2;
  }
}

void SensorManager::checkStability() {
  if (!frozen && beatCount >= MIN_BEATS) {
    bpmHistory[historyIndex] = beatAvg;
    spo2History[historyIndex] = spo2;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;

    if (beatCount >= HISTORY_SIZE) {
      float sumB = 0, sumS = 0;
      for (int i = 0; i < HISTORY_SIZE; i++) {
        sumB += bpmHistory[i];
        sumS += spo2History[i];
      }
      float meanB = sumB / HISTORY_SIZE;
      float meanS = sumS / HISTORY_SIZE;
      float sdB = 0, sdS = 0;
      for (int i = 0; i < HISTORY_SIZE; i++) {
        sdB += sq(bpmHistory[i] - meanB);
        sdS += sq(spo2History[i] - meanS);
      }
      sdB = sqrt(sdB / HISTORY_SIZE);
      sdS = sqrt(sdS / HISTORY_SIZE);

      if (sdB < 5.0 && sdS < 2.0) measurementDone = true;
    }
  }
}

void SensorManager::freezeValues() {
  frozen = true;
  frozenBPM = beatAvg;
  frozenSpO2 = spo2;
  frozenRMSSD = rmssd;
  frozenSDRR = sdrr;
  frozenPNN50 = pnn50;
}

void SensorManager::reset() {
  beatAvg = 0;
  intervalCount = 0;
  beatCount = 0;
  historyIndex = 0;
  measurementDone = false;
  doneNotified = false;
  frozen = false;
  finalStatusLabel = "MEMBACA...";
}

int SensorManager::getBPM() {
  return frozen ? frozenBPM : beatAvg;
}

int SensorManager::getSpO2() {
  return frozen ? frozenSpO2 : spo2;
}

float SensorManager::getRMSSD() {
  return frozen ? frozenRMSSD : rmssd;
}

float SensorManager::getSDRR() {
  return frozen ? frozenSDRR : sdrr;
}

int SensorManager::getPNN50() {
  return frozen ? frozenPNN50 : pnn50;
}

bool SensorManager::isFingerDetected() {
  return fingerDetected;
}

bool SensorManager::isMeasurementDone() {
  return measurementDone;
}

bool SensorManager::isFrozen() {
  return frozen;
}

bool SensorManager::isDoneNotified() {
  return doneNotified;
}

void SensorManager::setDoneNotified(bool value) {
  doneNotified = value;
}

String SensorManager::getStatus() {
  if (frozen) {
    return finalStatusLabel;
  }
  return "Membaca...";
}