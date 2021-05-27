import React, { useContext } from "react";
import { DeviceContext } from "../../providers/device.provider";

export const ConfigPage: React.FC = () => {
  const { config } = useContext(DeviceContext);

  return (
    <div>
      TODO: config page
      <div className="config-list">
        {Object.keys(config).map((configKey: string) => {
          return (
            <div className="item" key={configKey}>
              <div className="key">{`${configKey}:`}</div>
              <div className="value">{config[configKey] as string}</div>
            </div>
          );
        })}
      </div>
    </div>
  );
};
