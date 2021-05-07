const ConfigureAP = require("./pages/configureap");

/**
 * Fired when the document is loaded and ready
 */
$( document ).ready(function() {
    window.pageControl = new ConfigureAP();
});