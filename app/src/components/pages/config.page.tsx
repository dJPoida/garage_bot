import React, { useCallback, useContext, useEffect, useState } from 'react';

import { globals } from '../../singletons/globals.singleton';

import { DeviceContext } from '../../providers/device.provider';

import { useIsDirty } from '../../react-hooks/use-is-dirty.hook';
import { usePreviousValue } from '../../react-hooks/use-previous-value.hook';

import { IConfig } from '../../types/config.interface';
import { PageProps } from '../../types/page.props';

import { FormField } from '../form-field';
import { FormFieldGroup } from '../form-field-group';
import { Modal } from '../modal';
import { PageTitle } from '../page-title';
import { MQTT_STATE } from '../../constants/mqtt-client-state.const';

export type ConfigTransport = Pick<IConfig,
  'mdns_name' |
  'device_name' |
  'mqtt_enabled' |
  'mqtt_broker_address' |
  'mqtt_broker_port' |
  'mqtt_device_id' |
  'mqtt_state_topic' |
  'mqtt_command_topic' |
  'mqtt_username' |
  'mqtt_password'
>;

const getConfigTransportFromConfig = (config: IConfig): ConfigTransport => ({
  mdns_name: config.mdns_name,
  device_name: config.device_name,
  mqtt_enabled: config.mqtt_enabled,
  mqtt_broker_address: config.mqtt_broker_address,
  mqtt_broker_port: config.mqtt_broker_port,
  mqtt_device_id: config.mqtt_device_id,
  mqtt_username: config.mqtt_username,
  mqtt_password: config.mqtt_password,
  mqtt_command_topic: config.mqtt_command_topic,
  mqtt_state_topic: config.mqtt_state_topic,
});

export const ConfigPage: React.FC<PageProps> = (props) => {
  const {
    title,
    icon,
  } = props;

  const { config, configChecksum, mqttClientError, mqttClientState } = useContext(DeviceContext);

  const [isDirty, setDirty] = useIsDirty();
  const oldConfigChecksum = usePreviousValue(configChecksum);

  const [isSubmitting, setISubmitting] = useState<boolean>(false);
  const [submitSuccess, setSubmitSuccess] = useState<null | boolean>(null);
  const [submitErrorMessage, setSubmitErrorMessage] = useState<null | string>(null);

  const [configValues, setConfigValues] = useState<ConfigTransport>(getConfigTransportFromConfig(config));

  const [confirmSubmitVisible, setConfirmSubmitVisible] = useState<boolean>(false);

  /**
   * Fired when the user clicks the submit button
   */
  const handleSubmit = useCallback(async () => {
    setISubmitting(true);
    setSubmitSuccess(null);
    setConfirmSubmitVisible(false);

    const payload = {
      ...configValues,

      // Don't submit the password mask as a new password value
      mqtt_password: configValues.mqtt_password === '********' ? null : configValues.mqtt_password,
    };

    try {
      const response = await fetch(`http://${globals.deviceAddress}/setconfig`, {
        method: 'POST',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      });

      if (response.status === 200) {
        setSubmitSuccess(true);
      } else {
        setSubmitSuccess(false);
        setSubmitErrorMessage(`${response.status} - ${response.statusText}`);
      }
    } catch (error) {
      setSubmitSuccess(false);
      setSubmitErrorMessage(error);
    } finally {
      setISubmitting(false);
    }
  }, [configValues]);

  /**
   * Fired when one of the form field values changes
   */
  const fieldChanged = (
    fieldName: string,
    newValue: null | string | number | boolean,
  ) => {
    setDirty();
    setConfigValues((currentConfigValues) => ({
      ...currentConfigValues,
      [fieldName]: newValue,
    }));
  };

  /**
   * Fired when the user clicks the reset button
   */
  const handleReset = useCallback(async () => {
    setDirty(false);
    setConfigValues(getConfigTransportFromConfig(config));
  }, [config, setDirty]);

  /**
   * Update the local config whenever a new config is received from the device
   */
  useEffect(() => {
    if (configChecksum !== oldConfigChecksum) {
      setConfigValues(getConfigTransportFromConfig(config));
    }
  }, [config, configChecksum, oldConfigChecksum]);

  // Render
  return (
    <div className="page card config">
      <PageTitle title={title} icon={icon} />
      <p>
        {`Use this form to change the configuration of the ${globals.appTitle}`}
      </p>
      {/* Config Form */}
      {!isSubmitting && !submitSuccess && (
        <>
          <form>
            <FormFieldGroup legend="Network Config">
              {/* MDNS Name */}
              <FormField
                id="mdns_name"
                fieldName="mdns_name"
                type="text"
                label="MDNS Name"
                value={configValues.mdns_name}
                onChange={fieldChanged}
                toolTip={[
                  'The name to use in the mdns address that clients',
                  'can use to connect to the device without the IP',
                  '(i.e. http://garagebot.local)',
                  'Default = "garagebot"',
                ].join(' <br/>')}
              />

              {/* Device Name */}
              <FormField
                id="device_name"
                fieldName="device_name"
                type="text"
                label="Device Name"
                value={configValues.device_name}
                onChange={fieldChanged}
                toolTip={[
                  'The device name to display',
                  'to other devices on the network.',
                  'This also changes the name of the app,',
                  'but you may need to refresh the page.',
                  'Default = "GarageBot"',
                ].join(' <br/>')}
              />
            </FormFieldGroup>

            {/* MQTT Config */}
            <FormFieldGroup legend="MQTT Config">

              {/* If the MQTT Client is in error, display it here to assist with correcting the config */}
              {mqttClientState === MQTT_STATE.CONFIG_ERROR && (
                <div className="card fluid error">
                  <h4>MQTT Client Error</h4>
                  <p>
                    <span>The device attempted to use the provided configuration but encountered the following error:</span>
                    <br />
                    <br />
                    <span>{mqttClientError}</span>
                  </p>
                </div>
              )}

              {/* MQTT Enabled */}
              <FormField
                id="mqtt_enabled"
                fieldName="mqtt_enabled"
                label="Enable MQTT"
                value={configValues.mqtt_enabled}
                onChange={fieldChanged}
                type="checkbox"
                toolTip={['Disable or Enable MQTT'].join(' <br/>')}
              />
              {/* All of the MQTT Config values */}
              {configValues.mqtt_enabled && (
              <>
                {/* MQTT Broker Address */}
                <FormField
                  id="mqtt_broker_address"
                  fieldName="mqtt_broker_address"
                  type="text"
                  label="MQTT Broker Address"
                  value={configValues.mqtt_broker_address}
                  onChange={fieldChanged}
                  toolTip={[
                    'The IP address or network name of the MQTT Broker',
                  ].join(' <br/>')}
                />

                {/* MQTT Broker Port */}
                <FormField
                  id="mqtt_broker_port"
                  fieldName="mqtt_broker_port"
                  type="number"
                  label="MQTT Broker Port"
                  value={configValues.mqtt_broker_port}
                  minValue={0}
                  maxValue={65535}
                  onChange={fieldChanged}
                  toolTip={[
                    'The Port Number to use to the MQTT Broker',
                    'Default = "1883"',
                  ].join(' <br/>')}
                />

                {/* MQTT Device ID */}
                <FormField
                  id="mqtt_device_id"
                  fieldName="mqtt_device_id"
                  type="text"
                  label="MQTT Device ID"
                  value={configValues.mqtt_device_id}
                  onChange={fieldChanged}
                  toolTip={[
                    'The Device ID to use when connecting to the MQTT Broker',
                    'Default = "Garage_Bot"',
                  ].join(' <br/>')}
                />

                {/* MQTT Username */}
                <FormField
                  id="mqtt_username"
                  fieldName="mqtt_username"
                  type="text"
                  label="MQTT Username"
                  value={configValues.mqtt_username}
                  onChange={fieldChanged}
                  toolTip={[
                    'The username when connecting to the MQTT broker',
                  ].join(' <br/>')}
                />

                {/* MQTT Password */}
                <FormField
                  id="mqtt_password"
                  fieldName="mqtt_password"
                  type="text"
                  label="MQTT Password"
                  value={configValues.mqtt_password}
                  onChange={fieldChanged}
                  toolTip={[
                    'The password when connecting to the MQTT broker',
                  ].join(' <br/>')}
                />

                {/* MQTT Command Topic */}
                <FormField
                  id="mqtt_command_topic"
                  fieldName="mqtt_command_topic"
                  type="text"
                  label="MQTT Command Topic"
                  value={configValues.mqtt_command_topic}
                  onChange={fieldChanged}
                  toolTip={[
                    'The MQTT command topic used for communicating',
                    'commands & instructions (open / close etc)',
                    'Default = "garage/door/command"',
                  ].join(' <br/>')}
                />

                {/* MQTT State Topic */}
                <FormField
                  id="mqtt_state_topic"
                  fieldName="mqtt_state_topic"
                  type="text"
                  label="MQTT State Topic"
                  value={configValues.mqtt_state_topic}
                  onChange={fieldChanged}
                  toolTip={[
                    'The MQTT topic used for communicating the state',
                    'of the door (opened / closed / etc)',
                    'Default = "garage/door/state"',
                  ].join(' <br/>')}
                />
              </>
              )}
            </FormFieldGroup>
          </form>
          <div className="button-row">
            {/* Reset Button */}
            {isDirty && (
            <button type="button" className="info" onClick={handleReset}>
              Cancel
            </button>
            )}

            {/* Submit Button */}
            <button
              type="button"
              className="primary"
              onClick={() => setConfirmSubmitVisible(true)}
              disabled={!isDirty}
            >
              Submit
            </button>
          </div>
        </>
      )}

      {/* Submitting Spinner */}
      {isSubmitting && (
        <div>
          <p>Submitting new configuration</p>
          <div className="please-wait">
            <span className="spinner" />
            <span>Please wait...</span>
          </div>
        </div>
      )}

      {/* Submit Error */}
      {submitSuccess === false && (
        <div className="card fluid error">
          <h4>
            <span>Failed to set new configuration</span>
            <br />
          </h4>
          <p>{submitErrorMessage}</p>
        </div>
      )}

      <Modal
        id="confirm_submit"
        visible={confirmSubmitVisible}
        confirmLabel="Update"
        confirmClass="primary"
        onClose={() => setConfirmSubmitVisible(false)}
        onConfirm={() => handleSubmit()}
        title="Update device config?"
      >
        <p>
          {`Submit the new configuration to the ${globals.appTitle}?`}
        </p>
        <p>
          This will reboot the device.
        </p>
      </Modal>

    </div>
  );
};
