import React, { useContext } from 'react';
import { prettyFileSize } from '../../helpers/pretty-file-size.helper';
import { DeviceContext } from '../../providers/device.provider';

export const AboutPage: React.FC = () => {
  const { config, sensorData } = useContext(DeviceContext);

  return (
    <div className="page about">
      <div className="config-list">
        <div className="item">
          <div className="key">IP Address</div>
          <div className="value">{config.ip_address}</div>
        </div>
        <div className="item">
          <div className="key">Available Memory</div>
          <div className="value">
            {prettyFileSize(sensorData.availableMemory as number)}
          </div>
        </div>
      </div>

      <p>
        The garage bot is the brainchild of Peter Eldred (dJPoida) to smartenize
        and improve the functionality of a legacy up and down garage door.
      </p>
    </div>
  );
};
