# cordova-plugin-unimagII-swiper
Cordova plugin to swipe cards using UniMag II Reader.


# Installing
This plugin follows the Cordova 3.0+ plugin spec, so it can be installed through the Cordova CLI in your existing Cordova project

# JavaScript Usage Example

•	Send custom text on printer

    onDeviceReady: function() {
        app.receivedEvent('deviceready');

        cordova.plugins.unimagIISwiper.initialize();
        cordova.plugins.unimagIISwiper.activate();

        cordova.plugins.unimagIISwiper.onErrorRecieved = function (error) {
            alert(error);
        }

        cordova.plugins.unimagIISwiper.onDataRecieved = function (data) {
            alert(data);
        }
    }
