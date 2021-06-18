import React, { useContext, useEffect, useState } from 'react';

import { DeviceContext } from '../../providers/device.provider';

import { PageProps } from '../../types/page.props';

import { PageTitle } from '../page-title';
import { VerticalSlider } from '../vertical-slider';

export const CalibrationPage: React.FC<PageProps> = (props) => {
  const {
    title,
    icon,
  } = props;

  const { config, sensorData, setSensorThreshold } = useContext(DeviceContext);

  const [topSensorThreshold, setTopSensorThreshold] = useState(config.top_ir_sensor_threshold);
  const [bottomSensorThreshold, setBottomSensorThreshold] = useState(config.bottom_ir_sensor_threshold);

  useEffect(() => {
    setTopSensorThreshold(config.top_ir_sensor_threshold);
    setBottomSensorThreshold(config.bottom_ir_sensor_threshold);
  }, [config.top_ir_sensor_threshold, config.bottom_ir_sensor_threshold]);

  // TODO: calibration page
  return (
    <div className="page card calibration">
      <PageTitle title={title} icon={icon} />

      <p>
        Make sure the garage door is closed then use these sliders to calibrate the door sensors.
      </p>

      <div className="sensors">

        <VerticalSlider
          label="Top"
          maxValue={2047}
          backgroundValue={sensorData.topIRSensorAverageAmbientReading - sensorData.topIRSensorAverageActiveReading}
          value={topSensorThreshold}
          onAfterChange={(newValue) => {
            setSensorThreshold('TOP', newValue);
            setTopSensorThreshold(newValue);
          }}
          passed={!!sensorData.topIRSensorDetected}
        />
        <VerticalSlider
          label="Bottom"
          maxValue={2047}
          backgroundValue={sensorData.bottomIRSensorAverageAmbientReading - sensorData.bottomIRSensorAverageActiveReading}
          value={bottomSensorThreshold}
          onAfterChange={(newValue) => {
            setSensorThreshold('BOTTOM', newValue);
            setBottomSensorThreshold(newValue);
          }}
          passed={!!sensorData.bottomIRSensorDetected}
        />
      </div>

      <p>
        The sensors should be calibrated to detect the door with a margin of error.
        It can also help to test them during the partial opening and closing of the door
        as well as in low and extreme sunlight conditions.
      </p>
    </div>
  );
};
