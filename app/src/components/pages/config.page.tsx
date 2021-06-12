import React, { useCallback, useContext, useEffect, useState } from 'react';
import { DeviceContext } from '../../providers/device.provider';
import { useIsDirty } from '../../react-hooks/use-is-dirty.hook';
import { usePreviousValue } from '../../react-hooks/use-previous-value.hook';
import { IConfig } from '../../types/config.interface';
import { FormField } from '../form-field';
import { FormFieldGroup } from '../form-field-group';

export const ConfigPage: React.FC = () => {
  const { config, configChecksum } = useContext(DeviceContext);

  const [isDirty, setDirty] = useIsDirty();
  const oldConfigChecksum = usePreviousValue(configChecksum);

  const [isSubmitting, setISubmitting] = useState<boolean>(false);
  const [submitSuccess, setSubmitSuccess] = useState<null | boolean>(null);
  const [submitErrorMessage, setSubmitErrorMessage] = useState<null | string>(null);

  const [configValues, setConfigValues] = useState<IConfig>(config);

  /**
   * Fired when the user clicks the submit button
   */
  const handleSubmit = useCallback(async () => {
    setISubmitting(true);

    try {
      const response = await fetch('/setwifi', {
        method: 'POST',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(configValues),
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
    setConfigValues(config);
  }, [config, setDirty]);

  /**
   * Update the local config whenever a new config is received from the device
   */
  useEffect(() => {
    if (configChecksum !== oldConfigChecksum) {
      setConfigValues(config);
    }
  }, [config, configChecksum, oldConfigChecksum]);

  // Render
  return (
    <div className="page card config">
      <h2>Config</h2>
      <p>Use this form to change the configuration of the Garage Bot</p>
      {/* Config Form */}
      {!isSubmitting && !submitSuccess && (
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
              ].join(' <br/>')}
            />

            {/* Network Device Name */}
            <FormField
              id="network_device_name"
              fieldName="network_device_name"
              type="text"
              label="Device Name"
              value={configValues.network_device_name}
              onChange={fieldChanged}
              toolTip={[
                'The device name to display',
                'to other devices on the network',
              ].join(' <br/>')}
            />
          </FormFieldGroup>

          {/* MQTT Config */}
          <FormFieldGroup legend="MQTT Config">
            {/* MQTT Enabled */}
            <FormField
              id="mqtt_enabled"
              fieldName="mqtt_enabled"
              label="MQTT Enabled"
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

                {/* TODO: MQTT Broker Port */}

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

                {/* MQTT Topic */}
                <FormField
                  id="mqtt_topic"
                  fieldName="mqtt_topic"
                  type="text"
                  label="MQTT Topic"
                  value={configValues.mqtt_topic}
                  onChange={fieldChanged}
                  toolTip={[
                    'The MQTT topic used for communicating',
                    'instructions (open / close etc)',
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
                  ].join(' <br/>')}
                />
              </>
            )}
          </FormFieldGroup>
          <div className="button-row">
            {/* Reset Button */}
            {isDirty && (
              <button type="button" className="secondary" onClick={handleReset}>
                Cancel
              </button>
            )}

            {/* Submit Button */}
            <button
              type="button"
              className="primary"
              onClick={handleSubmit}
              disabled={!isDirty}
            >
              Submit
            </button>
          </div>
        </form>
      )}

      {/* TODO: Submitting Spinner */}
      {isSubmitting && (
        <div>
          <p>Assigning WiFi Credentials</p>
          <div className="please-wait">
            <span className="spinner" />
            <span>Please wait...</span>
          </div>
        </div>
      )}

      {/* TODO: Submit Error */}
      {submitSuccess === false && (
        <div className="card fluid error">
          <h4>
            <span>Failed to set new WiFi Details</span>
            <br />
          </h4>
          <p>{submitErrorMessage}</p>
        </div>
      )}

      {/* TODO: Submit Success */}
      {submitSuccess === true && (
        <div className="card fluid success">
          <h4>
            <span>Successfully applied new WiFi details.</span>
          </h4>
          <p>
            <span>
              The device will now reboot. In 15 seconds, this page will attempt
              <br />
              to connect to it via the URL below. If it does not, you may need
              <br />
              to locate the IP address of the device using your router console.
              <br />
            </span>
          </p>
          <p>
            <a href="http://garagebot.local">http://garagebot.local</a>
          </p>
        </div>
      )}
    </div>
  );
};
