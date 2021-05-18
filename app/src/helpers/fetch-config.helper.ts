import { IConfig } from "../types/config.interface";

/**
 * Fetch the config.json from the device
 * @returns IConfig
 */
export const fetchConfig = (): Promise<IConfig> => {
  return new Promise(async (resolve, reject) => {
    const response = await fetch('/config.json', {
      headers: {
        'Content-Type': 'application/json',
      },
    });
    
    if (response && response.status === 200) {
      const jsonData = await response.json();
      resolve(jsonData);
    } else {
      reject (new Error('Failed to fetch config.json'));
    }
  })
}