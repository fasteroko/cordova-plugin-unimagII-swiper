//
//  UniMagSwiper.h
//  SwiperTest
//
//  Created by Vladyslav on 8/26/16.
//  Copyright © 2016 Vladyslav. All rights reserved.
//

#import <Cordova/CDV.h>

@interface UniMagSwiper : CDVPlugin

- (void)initializeSwiper:(CDVInvokedUrlCommand *)command;
- (void)activateSwiper:(CDVInvokedUrlCommand *)command;
- (void)deactivateSwiper:(CDVInvokedUrlCommand *)command;

@end
