import React, { useContext, useState } from 'react';
import { prettyFileSize } from '../../helpers/pretty-file-size.helper';
import { DeviceContext } from '../../providers/device.provider';
import { Modal } from '../modal';

export const AboutPage: React.FC = () => {
  const { config, sensorData, reboot } = useContext(DeviceContext);

  const [confirmRebootVisible, setConfirmRebootVisible] = useState<boolean>(false);


  return (
    <div className="page card about">
      <section>
        <img src="img/dJP.svg" alt="dJP" />
        <p>
          The garage bot is the brainchild of Peter Eldred (dJPoida) to smartenize
          and improve the functionality of a legacy up and down garage door. Check out&nbsp;
          <span><a href="https://github.com/dJPoida/garage_bot" target="_blank" rel="noreferrer">https://github.com/dJPoida/garage_bot</a></span>
          &nbsp;for all the details.
        </p>
      </section>

      <hr />

      <section>
        <h2>Device Status</h2>
        <div className="item-list">
          <div className="key">IP Address</div>
          <div className="value">{config.ip_address}</div>
          <div className="key">MAC Address</div>
          <div className="value">TODO</div>
          <div className="key">Available Memory</div>
          <div className="value">
            {prettyFileSize(sensorData.availableMemory as number)}
          </div>
        </div>
      </section>

      <hr />

      <section>
        <h2>Cool Commands</h2>
        <div>
          <button
            type="button"
            className="secondary"
            onClick={() => setConfirmRebootVisible(true)}
          >
            Reboot
          </button>
        </div>
      </section>

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
