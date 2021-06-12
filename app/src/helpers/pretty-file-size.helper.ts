import { commas } from './commas.helper';

/**
 * Convert bytes to whole number KB string with 1000 separator if needed
 * @see https://www.gbmb.org/mb-to-mib
 *
 * @example
 * sizeKB(123456789)
 *  // > '120,563'
 *
 * sizeKB(54321)
 *  // > '53'
 *
 * @param bytes
 * @returns Size in KB eg. `1,432`
 */
export const sizeKB = (bytes: number): string => commas(Math.floor(bytes * 0.001));

/**
 * Convert bytes to whole number MB string with 1000 separator
 * @see `sizeKB`
 *
 * @param bytes
 * @returns Size in MB eg. `1,432`
 */
export const sizeMB = (bytes: number): string => commas(Math.floor(bytes * 0.000001));

/**
 * Convert bytes to whole number GB string with 1000 separator
 * @see `sizeKB`
 *
 * @param bytes
 * @returns Size in GB eg. `1,432`
 */
export const sizeGB = (bytes: number): string => commas(Math.floor(bytes * 0.000000001));

/**
 * Convert bytes to bits string with 1000 separator
 * @see `sizeKB`
 *
 * @param bytes
 * @returns Size in bits eg. `16`
 */
export const sizeBits = (bytes: number): string => commas(Math.floor(bytes * 8));

/**
 * Convert a filesize into a user readable string
 *
 * @param bytes
 * @returns size in GB, MB, KB, bits or bytes
 */
export const prettyFileSize = (bytes?: number): string => {
  if (typeof bytes === 'undefined') return 'undefined';

  if (bytes > 10000000000) return `${sizeGB(bytes)} GB`;
  if (bytes > 10000000) return `${sizeMB(bytes)} MB`;
  if (bytes > 10000) return `${sizeKB(bytes)} KB`;
  if (bytes === 0) return '0 Bytes';
  if (bytes < 0.1) return `${sizeBits(bytes)} bits`;

  return `${commas(bytes)} Bytes`;
};
