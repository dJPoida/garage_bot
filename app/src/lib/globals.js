module.exports = {
    /**
     * Override the Device URL if not in production mode
     */
    deviceURL: function () {
        // If you're attempting to access the device directly when using a PC to debug,
        // this IP address should be whatever you read out of the Arduino Serial Monitor
        // or your Router's attached devices readout. This makes it possible to modify and
        // test the application without having to upload to the device every time you make
        // a change.
        if (process.env.NODE_ENV != 'production') {
            return "http://192.168.0.31/";
        }
        return "/";
    },



    /**
     * Override the Access Point URL if not in production mode
     */
    apModeURL: function () {
        if (process.env.NODE_ENV != 'production') {
            return "/apmode.html";
        }
        return "/";
    },



    /**
     * The Hallway Monitor control panel uses MiniCSS which utilises checkboxes for showing modals.
     * This code progamattically checks / unchecks those checkboxes to show the modals.
     */
    showModal: function (modalId, state) {
        $('#' + modalId).prop('checked', state);
    },



    /**
     * simple wrapper to avoid having to use (typeof(value) === 'undefined') everywhere
     * 
     * @param {*} value
     * @returns {boolean}
     */
    isUndefined: function (value) {
        // Obtain `undefined` value that's
        // guaranteed to not have been re-assigned
        var undefined = void(0);
        return value === undefined;
    },



    /**
     * Generic method for ajax posts
     *
     * success call backs only fire if both the AJAX and Server Side operations were successful.
     *    onSuccess = function({object} data) {};
     *
     * error call backs fire if either the AJAX or Server Side operations failed
     *    onError = function({string} error, {object} data) {};
     *
     * complete call backs fire always
     *    onComplete = function() {};
     *
     * @param {string} route
     * @param {object} [data={}]
     * @param {string} [method="POST"]
     * @param {function|undefined} [baseDataCallback=undefined] pass an object function(){}; which returns a base data struct common to all ajax calls to remove duplication of base data
     * @param {function|undefined} [lockInterfaceCallback=undefined] pass a void function(boolean){}; which performs any interface locking requirements at the appropriate times
     * @param {function|undefined} [successCallback=undefined]
     * @param {function|undefined} [errorCallback=undefined]
     * @param {function|undefined} [completeCallback=undefined]
     * @param {boolean} [expectSuccessResponse=true]
     * @param {int} [timeout=3000] the default timeout for ajax calls
     */
    ajaxPost: function (route, data, method, baseDataCallback, lockInterfaceCallback, successCallback, errorCallback, completeCallback, expectSuccessResponse, timeout) {
        var self = this;
        try {
            var jsonData = {};
            if (!self.isUndefined(baseDataCallback)) {
                jsonData = baseDataCallback();
            }
            if (!self.isUndefined(data)) {
                $.extend(jsonData, data);
            }

            $.ajax({
                type: !self.isUndefined(method) ? method : 'POST',
                url: route,
                data: jsonData,
                dataType: 'json',
                timeout: timeout || 3000,

                beforeSend: function() {
                    if (!self.isUndefined(lockInterfaceCallback)) {
                        lockInterfaceCallback(true);
                    }
                },

                //Ajax Call succeeded but doesn't necessarily mean the server side operation completed
                success: function(data, textStatus, jqXHR) {
                    //Evaluate the data and call the error callback if it didn't work
                    if ((!self.isUndefined(expectSuccessResponse) && (expectSuccessResponse!==true)) || data['success']) {
                        if (!self.isUndefined(successCallback)){
                            successCallback(data);
                        }
                    } else {
                        if (!self.isUndefined(errorCallback)){
                            errorCallback(data['error'], data);
                        } else {
                            console.error('ajax call failed: "' + route + '" ' + JSON.stringify(data), 2);
                            console.error(' - error:' + data['error']);
                        }
                    }

                },

                error: function(jqXHR, textStatus, errorThrown) {
                    //On Error Callbacks
                    if (!self.isUndefined(errorCallback)){
                        if (!self.isUndefined(jqXHR['responseJSON']) && !self.isUndefined(jqXHR['responseJSON']['error'])) {
                            errorCallback(jqXHR['responseJSON']['error']);
                        } else {
                            errorCallback(textStatus);
                        }
                    } else {
                        console.error('ajax call failed: "' + route + '" ' + JSON.stringify(data), 2);
                        console.error(' - textStatus:' + textStatus);
                        console.error(' - errorThrown:' + errorThrown, 2);
                    }
                },

                complete: function(jqXHR, textStatus) {
                    if (!self.isUndefined(completeCallback)){
                        completeCallback();
                    }

                    if (!self.isUndefined(lockInterfaceCallback)) {
                        lockInterfaceCallback(false);
                    }
                }
            });
        } catch (ex) {
            console.error(ex, 2);

            if (!self.isUndefined(errorCallback)){
                errorCallback();
            }

            if (!self.isUndefined(completeCallback)){
                completeCallback();
            }
        }
    },



    /** 
     * Convert a set of RGB colors to HEX
     * @param {object} color provide color.r, color.g and color.b to convert to hex
     */
    rgbToHex: function (color) {
        var componentToHex = function (c) {
            var hex = c.toString(16);
            return hex.length === 1 ? "0" + hex : hex;
        };

        return "#" + componentToHex(color.r) + componentToHex(color.g) + componentToHex(color.b);
    },



    /**
     * Use WebRTC to get this device's IP Address
     */
    getLocalIPAddress: function () {
        return new Promise(function(resolve, reject) {
            // Compatibility for Firefox and chrome
            window.RTCPeerConnection = window.RTCPeerConnection || window.mozRTCPeerConnection || window.webkitRTCPeerConnection;

            var peerConnection = new RTCPeerConnection({iceServers:[]}), noop = function(){};
            
            // Create a bogus data channel
            peerConnection.createDataChannel('');
            
            // Create offer and set local description
            peerConnection.createOffer(peerConnection.setLocalDescription.bind(peerConnection), noop);

            // Wait for ICE Candidates
            peerConnection.onicecandidate = function(ice) {
                if (ice && ice.candidate && ice.candidate.candidate) {
                    var myIP = /([0-9]{1,3}(\.[0-9]{1,3}){3}|[a-f0-9]{1,4}(:[a-f0-9]{1,4}){7})/.exec(ice.candidate.candidate)[1];
                    peerConnection.onicecandidate = noop;
                    resolve(myIP);
                } else {
                    reject();
                }
            };
        });
    },



    /**
     * Check for the presence of the HallwayMonitor on a specific IP address
     * @param {string} ipAddress the Target IP address to query
     * @param {function} onError the callback to fire if the request was unsuccessful
     * @param {function} onSuccess the callback to fire if the request was successful
     */
    marcoPolo: function(ipAddress, onError, onSuccess) {
        let route = 'http://' + ipAddress + '/marco';

        onEndpointSuccess = function(response) {
            console.log(response);
            if (response.echo == "polo") {
                onSuccess();
            } else {
                onError();
            }
        };

        this.ajaxPost(route, {}, 'GET', undefined, undefined, onEndpointSuccess, onError, undefined, true, 250);
    },



    /**
     * Look for the device on the local network
     * @param {function} onSuccess a callback to call when the HallwayMonitor can be found
     */
    scanForDevice: function(onSuccess) {
        // Get the local device's IP address. We'll assume the Hallway Montitor is connected to
        // the Same Subnet
        this.getLocalIPAddress().then(
            (ipAddress) => {
                console.log('Local IP Address: ', ipAddress);

                let nextIP = 1;
                let attemptIPAddress = this.getIPAddressEndpoint(ipAddress, nextIP);

                // Iterate over 1..254 on the local subnet looking for the /marco response.
                let tryNextAddress = () => {
                    nextIP++;
                    if (nextIP > 254) {
                        nextIP = 2;
                    }
                    attemptIPAddress = this.getIPAddressEndpoint(ipAddress, nextIP);
                    console.log("Looking for HallwayMonitor at " + attemptIPAddress + "...");
                    this.marcoPolo(attemptIPAddress, () => {tryNextAddress();}, () => {onSuccess(attemptIPAddress);});
                }

                tryNextAddress();
            },
            () => {
                console.log('Failed to get local IP Address');
            }
        );
    },



    /**
     * Take an IP address, trim off the last part, and substitute it with the provided number 
     * @param {string} ipAddress 
     * @param {number} substituteAddress
     */
    getIPAddressEndpoint: function(ipAddress, substituteAddress) {
        return ipAddress.substring(0, ipAddress.lastIndexOf('.')) + "." + substituteAddress;
    }
}