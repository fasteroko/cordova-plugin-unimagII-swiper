//
//  UniMagSwiper.m
//  SwiperTest
//
//  Created by Vladyslav on 8/26/16.
//  Copyright © 2016 Vladyslav. All rights reserved.
//

typedef enum {
    UniMagSwiperEventTypeDataRecieved,
    UniMagSwiperEventTypeErrorRecieved,
    UniMagSwiperEventTypeDeviceAttached,
    UniMagSwiperEventTypeDeviceConnected,
    UniMagSwiperEventTypeSwipeStart
}UniMagSwiperEventType;

#import "UniMagSwiper.h"
#import "UniMagHelper.h"

@interface UniMagSwiper () <UniMagHelperDelegate>

@end

@implementation UniMagSwiper {
    BOOL _isDeviceWasActivated;
}

#pragma mark - UniMAgSwiper Cordova methods

- (void)initializeSwiper:(CDVInvokedUrlCommand *)command {
    
    [UniMagHelper shared].delegate = self;
    [[UniMagHelper shared] updateDeviceStatus];
    
    [self.commandDelegate sendPluginResult:[CDVPluginResult resultWithStatus:CDVCommandStatus_OK] callbackId:command.callbackId];
}

- (void)activateSwiper:(CDVInvokedUrlCommand *)command {
    if ([[UniMagHelper shared] delegate] != nil) {
        [[UniMagHelper shared] requestSwipe];
        [[UniMagHelper shared] updateDeviceStatus];
    }
    
    [self.commandDelegate sendPluginResult:[CDVPluginResult resultWithStatus:CDVCommandStatus_OK] callbackId:command.callbackId];
}

- (void)deactivateSwiper:(CDVInvokedUrlCommand *)command {
    
    [[UniMagHelper shared] cancelSwipe];
    [[UniMagHelper shared] updateDeviceStatus];
    
    [self.commandDelegate sendPluginResult:[CDVPluginResult resultWithStatus:CDVCommandStatus_OK] callbackId:command.callbackId];
}

#pragma mark - Private

- (void)fireEvent:(UniMagSwiperEventType)event withData:(id) data {
    
    NSString* jsCallString;
    NSString* eventMethod;
    NSString* dataArgument = data ? [NSString stringWithFormat: @"%@", data] : @"";
    
    switch (event) {
        case UniMagSwiperEventTypeDataRecieved:
            eventMethod = @"cordova.plugins.unimagIISwiper.swiperDidRecieveData";
            break;
        case UniMagSwiperEventTypeErrorRecieved:
            eventMethod = @"cordova.plugins.unimagIISwiper.swiperDidRecieveError";
            break;
        case UniMagSwiperEventTypeDeviceAttached:
            eventMethod = @"cordova.plugins.unimagIISwiper.swiperDidAttach";
            break;
        case UniMagSwiperEventTypeDeviceConnected:
            eventMethod = @"cordova.plugins.unimagIISwiper.swiperDidConnect";
            break;
        case UniMagSwiperEventTypeSwipeStart:
            eventMethod = @"cordova.plugins.unimagIISwiper.swiperDidStartSwipe";
            break;
            
        default:
            break;
    }
    
    jsCallString = [NSString stringWithFormat:
          @"%@('%@');", eventMethod, dataArgument];
    
    [self.commandDelegate evalJs:jsCallString];
}

#pragma mark - UniMagHelperDelegate

- (void)uniMagDidStatusChange:(BOOL)connected {
    if (connected){
        _isDeviceWasActivated = connected;
        [self.commandDelegate evalJs:@"alert('Swiper is ACTIVATED')"];
        [[UniMagHelper shared] requestSwipe];
        
        // Notify Cordova about Event
        [self fireEvent:UniMagSwiperEventTypeDeviceConnected withData:@YES];
    } else {
        if (_isDeviceWasActivated) {
            [self.commandDelegate evalJs:@"alert('Swiper is DEACTIVATED')"];
            
            // Notify Cordova about Event
            [self fireEvent:UniMagSwiperEventTypeDeviceConnected withData:@NO];
        }
    }
}

- (void)uniMagDidAttach:(BOOL)attached {
    if (attached) {
        [self.commandDelegate evalJs:@"alert('Device is plugged-IN')"];
        [[UniMagHelper shared] requestSwipe];
        
        // Notify Cordova about Event
        [self fireEvent:UniMagSwiperEventTypeDeviceAttached withData:@YES];
    } else {
        [self.commandDelegate evalJs:@"alert('Device is plugged-OUT')"];
        
        // Notify Cordova about Event
        [self fireEvent:UniMagSwiperEventTypeDeviceAttached withData:@NO];
    }
}

- (void)unimagSwipeStart {
    // Notify Cordova about Event
    [self fireEvent:UniMagSwiperEventTypeSwipeStart withData:nil];
}

- (void)uniMagSwipeDataReceived:(NSDictionary *)swipeData {
    [[UniMagHelper shared] requestSwipe];
    NSString *maskedTracks = [swipeData objectForKey:@"MaskedTracks"];
    
    NSRange rangeFirstUpperSybmol = [maskedTracks rangeOfString:@"^"];
    NSRange rangeSecondUpperSybmol = [maskedTracks rangeOfString:@"^" options:NSBackwardsSearch];
    
    NSString *cardNumber = [maskedTracks substringWithRange:NSMakeRange(2, rangeFirstUpperSybmol.location - 2)];
    NSString *cardName = [maskedTracks substringWithRange:NSMakeRange(rangeFirstUpperSybmol.location + 1, rangeSecondUpperSybmol.location - rangeFirstUpperSybmol.location - 1)];
    NSString *expYear = [maskedTracks substringWithRange:NSMakeRange(rangeSecondUpperSybmol.location + 1, 2)];
    expYear = [NSString stringWithFormat:@"20%@", expYear];
    NSString *expMonth = [maskedTracks substringWithRange:NSMakeRange(rangeSecondUpperSybmol.location + 3, 2)];
    
    NSString *cardType = @"";
    
    char firstChar = [cardNumber characterAtIndex:0];
    if (firstChar == '3'){
        cardType = @"AmericanExpress";
    }
    if (firstChar == '4'){
        cardType = @"Visa";
    }
    if (firstChar == '5'){
        cardType = @"MasterCard";
    }
    if (firstChar == '6'){
        cardType = @"Discover";
    }
    
    NSDictionary *cardInfoDictionary = @{
                                         @"cardName" : cardName,
                                         @"cardNumber" : cardNumber,
                                         @"expMonth" : expMonth,
                                         @"expYear" : expYear,
                                         @"cardType" : cardType
                                         };
    
    NSLog(@"%@",cardInfoDictionary);
    
    // Notify Cordova about Event
    [self fireEvent:UniMagSwiperEventTypeDataRecieved withData:cardNumber];
}

- (void)uniMagSwipeError:(NSError *)error {
    [[UniMagHelper shared] requestSwipe];
    
    // Notify Cordova about Event
    [self fireEvent:UniMagSwiperEventTypeErrorRecieved withData:error.localizedDescription];
}

@end
