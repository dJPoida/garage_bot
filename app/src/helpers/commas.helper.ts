/**
 * @description
 * Convert number to string with 1000 separator if over 1000
 *
 * @example
 * commas(123456)
 * // > '123,456'
 */
export const commas = (num: number | string): string => {
  if (num) {
    return num.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
  }

  return num.toString();
};
