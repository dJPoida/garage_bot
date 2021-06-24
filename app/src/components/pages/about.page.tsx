import React, { useContext, useState } from 'react';

import { DeviceContext } from '../../providers/device.provider';

import { PageProps } from '../../types/page.props';

import { Modal } from '../modal';
import { PageTitle } from '../page-title';

import { prettyFileSize } from '../../helpers/pretty-file-size.helper';
import { AppFooter } from '../app-footer';

export const AboutPage: React.FC<PageProps> = (props) => {
  const {
    title,
    icon,
  } = props;

  const { config, sensorData, reboot } = useContext(DeviceContext);

  const [confirmRebootVisible, setConfirmRebootVisible] = useState<boolean>(false);


  return (
    <div className="page card about">
      <PageTitle title={title} icon={icon} />

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
        <div>
          <button
            type="button"
            className="warning"
            onClick={() => setConfirmRebootVisible(true)}
          >
            Reboot
          </button>
        </div>
      </section>

      <hr />

      {/* Footer */}
      <AppFooter />

      <Modal
        id="confirm_reboot"
        visible={confirmRebootVisible}
        confirmLabel="Yes, Reboot"
        confirmClass="warning"
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
