/**
 * Use WebRTC to get this device's IP Address
 */
export const getLocalIPAddress = (): Promise<string> => {
  return new Promise((resolve, reject) => {
    var rtc = new RTCPeerConnection();
    rtc.createDataChannel("");

    rtc.onicecandidate = function (evt) {
      if (evt.candidate) grepSDP("a=" + evt.candidate.candidate);
    };

    rtc.createOffer().then(
      function (offerDesc) {
        grepSDP(offerDesc.sdp);
        rtc.setLocalDescription(offerDesc);
      },
      function (e) {
        reject(e);
      }
    );

    function grepSDP(sdp?: String) {
      const line = sdp
        ?.split("\r\n")
        .find((line) => line.includes("a=candidate"));
      if (line) {
        var parts = line.split(" "),
          addr = parts[4],
          type = parts[7];
        if (type === "host") {
          const strAddr: string = addr.split(".").join("_");
          resolve(strAddr);
        }
      }
    }
  });
};
