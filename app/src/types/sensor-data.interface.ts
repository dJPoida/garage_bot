export interface ISensorData extends Record<string, null | boolean | number> {
  topIRSensorDetected: boolean;
  topIRSensorAverageAmbientReading: number;
  topIRSensorAverageActiveReading: number;
  bottomIRSensorDetected: boolean;
  bottomIRSensorAverageAmbientReading: number;
  bottomIRSensorAverageActiveReading: number;
}

export const mapPayloadToSensorData = (payload: Record<string, unknown>): ISensorData => ({
  topIRSensorDetected: payload.top_detected as boolean,
  topIRSensorAverageAmbientReading: payload.top_ambient as number,
  topIRSensorAverageActiveReading: payload.top_active as number,
  bottomIRSensorDetected: payload.bottom_detected as boolean,
  bottomIRSensorAverageAmbientReading: payload.bottom_ambient as number,
  bottomIRSensorAverageActiveReading: payload.bottom_active as number,
  availableMemory: payload.available_memory as number,
});
