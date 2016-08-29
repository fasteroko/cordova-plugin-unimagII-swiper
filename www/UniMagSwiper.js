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
	alert(data);
};

Swiper.swiperDidRecieveError = function (error) {
    alert(error);
};

Swiper.swiperDidAttach = function (attachStatus) {
    alert(attachStatus);
};

Swiper.swiperDidConnect = function (connectStatus) {
    alert(connectStatus);
};

Swiper.swiperDidStartSwipe = function () {
    //alert('Did Start Swipe');
};

module.exports = Swiper;
