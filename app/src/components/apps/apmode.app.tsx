import React, { useCallback, useState } from 'react';
import { AppFooter } from '../app-footer';
import { AppHeader } from '../app-header';

export const APModeApp: React.FC = () => {
  const [wifiSSID, setWifiSSID] = useState<string>('');
  const [wifiPassword, setWifiPassword] = useState<string>('');
  const [isSubmitting, setISubmitting] = useState<boolean>(false);
  const [submitSuccess, setSubmitSuccess] = useState<null | boolean>(null);
  const [submitErrorMessage, setSubmitErrorMessage] = useState<null | string>(null);

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
        body: JSON.stringify({
          wifiSSID,
          wifiPassword,
        }),
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
  }, [wifiPassword, wifiSSID]);

  // Render
  return (
    <div className="app ap-mode">
      <AppHeader />

      {/* WifiSSID and Password Form */}
      {!isSubmitting && !submitSuccess && (
        <form>
          <p>
            Your GarageBot was unable to connect to its previously configured
            WiFi hot-spot.
            <br />
            Provide the credentials of the WiFi network below:
          </p>
          <fieldset>
            <div className="input-group vertical">
              <label htmlFor="wifiSSID">SSID</label>
              <input
                id="wifiSSID"
                name="wifiSSID"
                type="text"
                value={wifiSSID}
                onChange={(e) => setWifiSSID(e.target.value)}
              />
            </div>
            <div className="input-group vertical">
              <label htmlFor="password">Password</label>
              <input
                id="password"
                name="password"
                value={wifiPassword}
                onChange={(e) => setWifiPassword(e.target.value)}
              />
            </div>
          </fieldset>
          <div className="button-row">
            <button type="button" className="primary" onClick={handleSubmit}>
              Submit
            </button>
          </div>
        </form>
      )}

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
            <a href="http://garagebot.local">http://garagebot.local</a>
          </p>
        </div>
      )}

      {/* Footer */}
      <hr />
      <AppFooter />
    </div>
  );
};
