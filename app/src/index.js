// @TODO: sockets. Ajax Sux
const ControlPanel = require("./pages/controlpanel");

/**
 * Fired when the document is loaded and ready
 */
$( document ).ready(() => {
    window.pageControl = new ControlPanel();
});