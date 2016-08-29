# cordova-plugin-unimagII-swiper
Cordova plugin to swipe cards using UniMag II Reader.


# Installing
This plugin follows the Cordova 3.0+ plugin spec, so it can be installed through the Cordova CLI in your existing Cordova project

# JavaScript Usage Example

•	Send custom text on printer

    onDeviceReady: function() {
        app.receivedEvent('deviceready');

        // Initialize swiper for using in app.
        cordova.plugins.unimagIISwiper.initialize();

        // Activate swiper if we need to read data.
        cordova.plugins.unimagIISwiper.activate();

        // Deactivate device if it not needed now.
        //cordova.plugins.unimagIISwiper.deactivate();

        // Handle recieving error whire reading data from card.
        cordova.plugins.unimagIISwiper.onErrorRecieved = function (error) {
            alert(error);
        }

        // Handle recieved data from card on success.
        cordova.plugins.unimagIISwiper.onDataRecieved = function (data) {
            alert(data);
        }

        // Handle start of swipe event.
        cordova.plugins.unimagIISwiper.didStartSwipe = function () {
            alert('Swipe Started');
        }

        // Handle Attach status changing.
        cordova.plugins.unimagIISwiper.didAttachDevice = function (attachStatus) {
            if (attachStatus) {
                alert('Device is plugged-in!');
            } else {
                alert('Device is plugged-out!');
            }
        }

        // Handle Connect status changing.
        cordova.plugins.unimagIISwiper.didConnectDevice = function (connectStatus) {
            if (connectStatus) {
                alert('Device is connected!');
            } else {
                alert('Device is disconnected!');
            }
        }
    }
