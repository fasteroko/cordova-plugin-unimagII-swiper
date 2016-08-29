var exec = require('cordova/exec');

var Swiper = function() {};

Swiper.initialize = function(success, error) {
	exec(success, error, 'UniMagSwiper', 'initializeSwiper', []);
};

Swiper.activate = function(success, error) {
	exec(success, error, 'UniMagSwiper', 'activateSwiper', []);
};

Swiper.deactivate = function(success, error) {
	exec(success, error, 'UniMagSwiper', 'deactivateSwiper', []);
};

Swiper.swiperDidRecieveData = function (data) {
    // Data recieved event
    Swiper.onDataRecieved(data);
};

Swiper.swiperDidRecieveError = function (error) {
    // Error recieved event
    Swiper.onErrorRecieved(error);
};

Swiper.swiperDidAttach = function (attachStatus) {
    if (attachStatus == 0) {
        //alert('Device is plugged-out!');
        Swiper.didAttachDevice(false);
    } else {
        //alert('Device is plugged-in!');
        Swiper.didAttachDevice(true);
    }
};

Swiper.swiperDidConnect = function (connectStatus) {
    if (connectStatus == 0) {
        //alert('Device is disconnected!');
        Swiper.didConnectDevice(false);
    } else {
        //alert('Device is connected!');
        Swiper.didConnectDevice(true);
    }
};

Swiper.swiperDidStartSwipe = function () {
    // Did start swipe event
    Swiper.didStartSwipe();
};

module.exports = Swiper;
