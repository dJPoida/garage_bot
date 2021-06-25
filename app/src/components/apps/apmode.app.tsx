import React, { useCallback, useState } from 'react';
import { useIsDirty } from '../../react-hooks/use-is-dirty.hook';
import { globals } from '../../singletons/globals.singleton';
import { AppFooter } from '../app-footer';
import { AppTitle } from '../app-title';
import { FormField } from '../form-field';
import { FormFieldGroup } from '../form-field-group';
import { Modal } from '../modal';

export const APModeApp: React.FC = () => {
  const [configValues, setConfigValues] = useState<{
    wifiSSID: string,
    wifiPassword: string,
  }>({
    wifiSSID: '',
    wifiPassword: '',
  });

  const [confirmSubmitVisible, setConfirmSubmitVisible] = useState<boolean>(false);
  const [isSubmitting, setISubmitting] = useState<boolean>(false);
  const [submitSuccess, setSubmitSuccess] = useState<null | boolean>(null);
  const [submitErrorMessage, setSubmitErrorMessage] = useState<null | string>(null);
  const [isDirty, setDirty] = useIsDirty();

  /**
   * Fired when the user clicks the submit button
   */
  const handleSubmit = useCallback(async () => {
    setConfirmSubmitVisible(false);
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


  // Render
  return (
    <div className="app ap-mode">
      <div className="page ap-mode">
        <AppTitle />
        <hr />

        {/* Submitting Spinner */}
        {isSubmitting && (
          <div>
            <p>Assigning WiFi Credentials</p>
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
              <span>Failed to set new WiFi Details</span>
              <br />
            </h4>
            <p>{submitErrorMessage}</p>
          </div>
        )}

        {/* Submit Success */}
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
              {/* TODO this should be templated from the device to match the _config.h */}
              <a href="http://garagebot.local">http://garagebot.local</a>
            </p>
          </div>
        )}

        {/* WifiSSID and Password Form */}
        {!isSubmitting && !submitSuccess && (
          <>
            <div>
              <p>
                {`Your ${globals.appTitle} was unable to connect to its previously configured
                WiFi hot-spot or has recently been reset to factory defaults.`}
              </p>
              <p>
                {`If you want to connect the ${globals.appTitle} to your WiFi, provide the details below.`}
              </p>
            </div>

            <form>
              <FormFieldGroup>
                {/* SSID */}
                <FormField
                  id="wifiSSID"
                  fieldName="wifiSSID"
                  type="text"
                  label="SSID"
                  value={configValues.wifiSSID}
                  onChange={fieldChanged}
                  toolTip={[
                    'The SSID of your local WiFi network',
                  ].join(' <br/>')}
                />

                {/* Password */}
                <FormField
                  id="wifiPassword"
                  fieldName="wifiPassword"
                  type="text"
                  label="Password"
                  value={configValues.wifiPassword}
                  onChange={fieldChanged}
                  toolTip={[
                    'The password for the local WiFi Network',
                  ].join(' <br/>')}
                />
              </FormFieldGroup>
            </form>
            <div className="button-row">
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

        <hr />

        {/* Footer */}
        <AppFooter />

        <Modal
          id="confirm_submit"
          title="Submit WiFi Credz"
          visible={confirmSubmitVisible}
          confirmLabel="Ok, Ta!"
          confirmClass="primary"
          onClose={() => setConfirmSubmitVisible(false)}
          onConfirm={handleSubmit}
        >
          <p>
            {`After submitting your local WiFi credentials to the ${globals.appTitle} it will reboot and attempt to connect to your network.`}
          </p>
          <p>
            Once rebooted, you may attempt to connect to it on the following URL or access your local router to determine it&apos;s IP address.
          </p>
          <p>
            <a href="http://garagebot.local">http://garagebot.local</a>
          </p>
        </Modal>
      </div>
    </div>
  );
};
