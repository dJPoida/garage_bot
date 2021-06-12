/**
 * These are the icon classes available in the Mini-CSS styling that the site uses
 * @see https://minicss.org/docs#icons
 */
export const ICON = {
  ALERT: 'icon-alert',
  BOOKMARK: 'icon-bookmark',
  CALENDAR: 'icon-calendar',
  CART: 'icon-cart',
  CREDIT: 'icon-credit',
  EDIT: 'icon-edit',
  HELP: 'icon-help',
  HOME: 'icon-home',
  INFO: 'icon-info',
  LINK: 'icon-link',
  LOCATION: 'icon-location',
  LOCK: 'icon-lock',
  MAIL: 'icon-mail',
  PHONE: 'icon-phone',
  RSS: 'icon-rss',
  SEARCH: 'icon-search',
  SETTINGS: 'icon-settings',
  SHARE: 'icon-share',
  UPLOAD: 'icon-upload',
  USER: 'icon-user',
  UP: 'icon-up',
  TOGGLE: 'icon-toggle',
  DOWN: 'icon-down',
} as const;
export type ICON = typeof ICON;
export type AN_ICON = ICON[keyof ICON];
