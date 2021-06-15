import React, { useContext, useState } from 'react';
import { prettyFileSize } from '../../helpers/pretty-file-size.helper';
import { DeviceContext } from '../../providers/device.provider';
import { Modal } from '../modal';

export const AboutPage: React.FC = () => {
  const { config, sensorData, reboot } = useContext(DeviceContext);

  const [confirmRebootVisible, setConfirmRebootVisible] = useState<boolean>(false);


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

      <div>
        <button
          type="button"
          className="secondary"
          onClick={() => setConfirmRebootVisible(true)}
        >
          Reboot
        </button>
      </div>

      <p>
        The garage bot is the brainchild of Peter Eldred (dJPoida) to smartenize
        and improve the functionality of a legacy up and down garage door.
      </p>

      <Modal
        id="confirm_reboot"
        visible={confirmRebootVisible}
        confirmLabel="Yes, Reboot"
        confirmClass="secondary"
        onClose={() => setConfirmRebootVisible(false)}
        onConfirm={reboot}
      >
        <p>
          This will reboot the GarageBot. Are you sure?
        </p>
      </Modal>
    </div>
  );
};
