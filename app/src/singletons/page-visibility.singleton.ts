import events from 'events';

let pageActivityInstance: PageActivity;

/**
 * These are the events that the page visibility singleton can fire to any subscribed listeners
 */
export const PAGE_ACTIVITY_EVENT = {
  USER_ACTIVE_CHANGE: 'USER_ACTIVE_CHANGE',
  VISIBILITY_CHANGE: 'VISIBILITY_CHANGE',
} as const;
export type PAGE_ACTIVITY_EVENT = typeof PAGE_ACTIVITY_EVENT;
export type A_PAGE_ACTIVITY_EVENT = PAGE_ACTIVITY_EVENT[keyof PAGE_ACTIVITY_EVENT];

/**
 * A singleton class for detecting whether the user is active on the current browser page or not
 *
 * Useful when you want to shut down stuff that isn't required
 * unless the user is actively using the webpage
 *
 * @see https://www.igvita.com/2015/11/20/dont-lose-user-and-app-state-use-page-visibility/
 */
class PageActivity extends events.EventEmitter {
  // Whether the current page is visible or not
  private _pageVisible: boolean = document.visibilityState === 'visible';

  // Whether the user is considered active on the current page
  // (initialise to true if the current page is visible)
  private _userActive: boolean = document.visibilityState === 'visible';

  // The number of milliseconds of inactivity before a user is considered inactive
  private _inactivityThreshold = 10; // Default 2 mins

  private visibilityTimeout: null | ReturnType<typeof setTimeout> = null;

  /**
   * @constructor
   */
  constructor() {
    super();
    this.bindEvents();
    this.handleUserActivity();
  }

  /**
   * @property whether the page is currently in the foreground and active
   */
  public get pageVisible(): boolean {
    return this._pageVisible;
  }

  /**
   * @property whether the user is considered active on the current page
   */
  public get userActive(): boolean {
    return this._userActive;
  }

  /**
   * @property the number of seconds before a user is considered inactive
   */
  public get inactivityThreshold(): number {
    return this._inactivityThreshold;
  }
  public set inactivityThreshold(value: number) {
    this._inactivityThreshold = value;
    this.handleUserActivity();
  }

  /**
   * Create a new instance of the page visibility class if one does not already exist
   */
  static getInstance = (): PageActivity => {
    if (!pageActivityInstance) {
      pageActivityInstance = new PageActivity();
    }
    return pageActivityInstance;
  };

  /**
   * Bind the event listeners that make this class work
   */
  private bindEvents = () => {
    document.addEventListener('focus', this.handleUserActivity.bind(this));
    document.addEventListener('click', this.handleUserActivity.bind(this));

    // subscribe to visibility change events
    document.addEventListener('visibilitychange', () => {
      const oldVisible = this.pageVisible;

      this._pageVisible = document.visibilityState === 'visible';
      this.emit(PAGE_ACTIVITY_EVENT.VISIBILITY_CHANGE, this._pageVisible);

      // If the page became visible this counts as user activity
      if (this.pageVisible !== oldVisible) {
        this.handleUserActivity();
      }
    });
  };

  /**
   * Fired whenever an action that is considered "user activity"
   * needs to bump the inactivity timeout
   */
  private handleUserActivity = () => {
    const oldUserActive = this.userActive;
    this._userActive = true;

    // Notify listeners of a change
    if (this.userActive !== oldUserActive) {
      this.emit(PAGE_ACTIVITY_EVENT.USER_ACTIVE_CHANGE, true);
    }

    // Wait for a designated amount of time then consider the user inactive
    if (this.visibilityTimeout) {
      clearTimeout(this.visibilityTimeout);
    }
    this.visibilityTimeout = setTimeout(() => {
      this._userActive = false;
      this.visibilityTimeout = null;
      // Notify listeners of the change
      this.emit(PAGE_ACTIVITY_EVENT.USER_ACTIVE_CHANGE, false);
    }, this.inactivityThreshold * 1000);
  };
}

export const pageVisibility: PageActivity = PageActivity.getInstance();
