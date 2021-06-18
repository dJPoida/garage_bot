import React, { useContext } from 'react';

import { DeviceContext } from '../../providers/device.provider';

import { PageProps } from '../../types/page.props';

import { PageTitle } from '../page-title';

export const CalibrationPage: React.FC<PageProps> = (props) => {
  const {
    title,
    icon,
  } = props;

  const { sensorData } = useContext(DeviceContext);

  // TODO: calibration page
  return (
    <div className="page calibration">
      <PageTitle title={title} icon={icon} />
      <div className="sensor-data">
        {/* Headings */}
        <div />
        <div>Ambient</div>
        <div>Active</div>
        <div>Delta</div>
        <div>Detected</div>

        {/* Top Sensor */}
        <div>Top</div>
        <div>{sensorData.topIRSensorAverageAmbientReading}</div>
        <div>{sensorData.topIRSensorAverageActiveReading}</div>
        <div>
          {sensorData.topIRSensorAverageAmbientReading
            - sensorData.topIRSensorAverageActiveReading}
        </div>
        <div>{sensorData.topIRSensorDetected ? '✔' : '-'}</div>

        {/* Bottom Sensor */}
        <div>Bottom</div>
        <div>{sensorData.bottomIRSensorAverageAmbientReading}</div>
        <div>{sensorData.bottomIRSensorAverageActiveReading}</div>
        <div>
          {sensorData.bottomIRSensorAverageAmbientReading
            - sensorData.bottomIRSensorAverageActiveReading}
        </div>
        <div>{sensorData.bottomIRSensorDetected ? '✔' : '-'}</div>
      </div>
    </div>
  );
};
