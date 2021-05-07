const Globals = require ('../lib/globals.js');

/**
 * @class ConfigureAP
 * The client side code that controls the AP Configuration Mode
 */
class ConfigureAP {
    
    /**
     * Constructor
     */
    constructor() {
        this.initialise();
    }



    /**
     * Initialise the Page
     */
    initialise = () => {
        console.log('initialise()');
        this.scrapeHTML();
        this.bindEvents();
    }    



    /**
     * Scrape the HTML for components we'll need as part of the application
     */
    scrapeHTML = () => {
        console.log('scrapeHTML()');
        this.$saveButton = $('#button_save');
        this.$newWiFiConfigForm = $('#form_new_wifi_config');
    }



    /**
     * Bind all of the events required to drive the control panel
     */
    bindEvents = () => {
        console.log('bindEvents()');

        // Save Button
        this.$saveButton.on('click', () => {
            this.submitWiFiDetails();
        });

        // Submit new WiFi Config Form
        this.$newWiFiConfigForm.on('submit', (e) => {
            e.preventDefault();
        })
    }



    /**
     * Submit the new WiFi details to the device
     */
    submitWiFiDetails = () => {
        console.log('submitWiFiDetails()');

        let route = '/setwifi';
        let data = {
            "wifiSSID": $('#input_wifiSSID').val(),
            "wifiPassword": $('#input_wifiPassword').val()
        };
        console.log(data);

        // What to do when the call is successful
        let onSuccess = () => {
            console.log('Successfully updated WiFi configuration');

            Globals.showModal('modal_scanning', true);

            this.findHallwayMonitor();
        }

        // What to do when the call fails
        let onError = () => {
            // @todo: Error Handling: Toast?
        }
        
        Globals.ajaxPost(route, data, 'GET', undefined, undefined, onSuccess, onError);
    }



    /**
     * Once the WiFi config has been set, we can assume that the Hotspot will disappear.
     * Typically, THIS device will then fallback onto the WiFi network that the user 
     * want the Hallway Monitor to connect to. Now, this isn't perfect, but it's 99%
     * reliable. Once we're assured we're connected to the local network (say after 15 seconds)
     * We can scan that network for the Hallway monitor by attempting to hit every IP
     * in the subnet for the `/marco` endpoint (which we expect to get a `polo`) response.
     * When we find one, that's the device's IP address on the current network. Forward
     * the browser to that IP Address.
     */
    findHallwayMonitor = () => {
        // Start by waiting 10000 ms for the device to re-boot and connect to the wifi network.
        setTimeout(() => {
            // Begin scanning for the new device IP address. When found - re-direct to that IP address
            Globals.scanForDevice((newIpAddress) => {
                window.location = "http://" + newIpAddress;
            });
        }, 10000);
    }
}

module.exports = ConfigureAP;