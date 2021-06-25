import React, { useContext, useState } from 'react';

import { DeviceContext } from '../../providers/device.provider';

import { PageProps } from '../../types/page.props';

import { Modal } from '../modal';
import { PageTitle } from '../page-title';

import { prettyFileSize } from '../../helpers/pretty-file-size.helper';
import { AppFooter } from '../app-footer';
import { globals } from '../../singletons/globals.singleton';

export const AboutPage: React.FC<PageProps> = (props) => {
  const {
    title,
    icon,
  } = props;

  const { config, sensorData, reboot, forgetWiFi, resetToFactoryDefaults } = useContext(DeviceContext);

  const [confirmRebootVisible, setConfirmRebootVisible] = useState<boolean>(false);
  const [confirmForgetWiFiVisible, setConfirmForgetWiFiVisible] = useState<boolean>(false);
  const [confirmResetToFactoryDefaultsVisible, setConfirmResetToFactoryDefaultsVisible] = useState<boolean>(false);


  return (
    <div className="page card about">
      <PageTitle title={title} icon={icon} />

      {/* Footer */}
      <AppFooter />

      <section>
        <div className="item-list">
          <div className="key">Control Panel:</div>
          <div className="value"><a href={`http://${config.mdns_name}.local`}>{`http://${config.mdns_name}.local`}</a></div>
          <div className="key">IP Address:</div>
          <div className="value">{config.ip_address}</div>
          <div className="key">MAC Address:</div>
          <div className="value">{config.mac_address}</div>
          <div className="key">Available Memory:</div>
          <div className="value">
            {prettyFileSize(sensorData.availableMemory as number)}
          </div>
        </div>
      </section>

      <section>
        <div className="button-wrapper">
          <button
            type="button"
            className="warning"
            onClick={() => setConfirmRebootVisible(true)}
          >
            Reboot
          </button>
          <button
            type="button"
            className="warning"
            onClick={() => setConfirmForgetWiFiVisible(true)}
          >
            Forget WiFi
          </button>
          <button
            type="button"
            className="danger"
            onClick={() => setConfirmResetToFactoryDefaultsVisible(true)}
          >
            Reset to Factory Defaults
          </button>
        </div>
      </section>

      {/* Confirm Reboot Modal */}
      <Modal
        id="confirm_reboot"
        visible={confirmRebootVisible}
        confirmLabel="Yes, Reboot"
        confirmClass="warning"
        onClose={() => setConfirmRebootVisible(false)}
        onConfirm={reboot}
      >
        <p>
          {`This will reboot the ${globals.appTitle}. Are you sure?`}
        </p>
      </Modal>

      {/* Confirm Forget WiFi Modal */}
      <Modal
        id="confirm_forget_wifi"
        visible={confirmForgetWiFiVisible}
        confirmLabel="Yes, Forget WiFi"
        confirmClass="warning"
        onClose={() => setConfirmForgetWiFiVisible(false)}
        onConfirm={forgetWiFi}
        title="Forget WiFi Configuration?"
      >
        <p>
          This will reset the WiFi configuration and reboot the device.
        </p>
        <p>
          Once rebooted, you will need to connect to the
          <strong> garagebot </strong>
          wifi network to provide a new WiFi SSID and Password.
        </p>
        <p>
          Are you sure?
        </p>
      </Modal>

      {/* Confirm Reset to Factory Defaults */}
      <Modal
        id="confirm_reset_to_factory_defaults"
        visible={confirmResetToFactoryDefaultsVisible}
        confirmLabel="Yes, Reset to Factory Defaults"
        confirmClass="danger"
        onClose={() => setConfirmResetToFactoryDefaultsVisible(false)}
        onConfirm={resetToFactoryDefaults}
        title="Holy freakin' Shite! Are you sure?"
      >
        <p>
          {`You're about to completely remove all of your custom settings and reset the ${globals.appTitle} to it's factory defaults.`}
        </p>
        <p>
          This will erase all of your configuration and reboot the device.
        </p>
        <p>
          Once rebooted, you will need to connect to the
          <strong> garagebot </strong>
          wifi network to provide a new WiFi SSID and Password.
        </p>
        <p>
          Are you really, totally, positively sure?
        </p>
      </Modal>
    </div>
  );
};
