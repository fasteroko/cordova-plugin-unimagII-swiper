//
//  UniMagHelper.h
//  OrderPort
//
//  Created by mak on 27.05.14.
//  Copyright (c) 2014 almighty technologies. All rights reserved.
//

typedef enum {
    UnimagTypeMag,
    UnimagTypePay
}UnimagType;

#import <Foundation/Foundation.h>

@protocol UniMagHelperDelegate <NSObject>

@optional
- (void)uniMagDidStatusChange:(BOOL)connected;
- (void)uniMagSwipeDataReceived:(NSDictionary *)swipeData;
- (void)uniMagGiftSwiperReceived:(NSDictionary *)data;
- (void)unimagSwipeStart;
- (void)uniMagSwipeError:(NSError *)error;
- (void)uniMagCmdReceived:(NSString *)text;
- (void)uniMagDidAttach:(BOOL)attached;
- (void)unipayDidReceiveMessage:(NSString *)messsage;
- (void)unipaySwipeDataReceived:(NSDictionary *)swipeData;
- (void)unipayMSRswipeDataReceived:(NSDictionary *)swipeData;
@end

@interface UniMagHelper : NSObject
+ (instancetype)shared;
- (void)updateDeviceStatus;
- (void)requestSwipe;
- (BOOL)isDeviceConnected;
- (BOOL)isDeviceAttached;
- (void)serialNum;
- (void)getSerial;
@property (nonatomic, weak) id<UniMagHelperDelegate> delegate;
@property (nonatomic, strong, readonly) NSString *serialNumber;
@property (nonatomic, strong, readonly) NSString *deviceName;
@property (nonatomic) UnimagType type;
- (void)reset;
- (void)cancelSwipe;

- (void)emvPowerOnICC;
- (void)emvPowerOffICC;
- (void)emvStart;
- (void)emvComplete;
- (void)emvCancel;
- (void)removeAID;
- (void)createTerminalFile:(id)sender;
-(void) getTerminalFile:(id)sender;
- (NSString *)hexComponentsToString:(NSString *)hexComponents;
@end
