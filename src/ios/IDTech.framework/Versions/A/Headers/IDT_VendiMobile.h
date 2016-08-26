//
//  IDT_VendiMobile.h
//  IDTech
//
//  Created by Randy Palermo on 10/29/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IDTMSRData.h"
#import "IDT_Device.h"

/** Protocol methods established for IDT_VendiMobile class  **/
@protocol IDT_VendiMobile_Delegate <NSObject>
@optional
-(void) deviceConnected; //!<Fires when device connects.  If a connection is established before the delegate is established (no delegate to send initial connection notification to), this method will fire upon establishing the delegate.
-(void) deviceDisconnected; //!<Fires when device disconnects.
- (void) dataInOutMonitor:(NSData*)data  incoming:(BOOL)isIncoming; //!<All incoming/outgoing data going to the device can be monitored through this delegate.
//!< @param data The serial data represented as a NSData object
//!< @param isIncoming The direction of the data
//!<- <c>TRUE</c> specifies data being received from the device,
//!<- <c>FALSE</c> indicates data being sent to the device.

- (void) swipeMSRData:(IDTMSRData*)cardData;//!<Receives card data from MSR swipe.
//!< @param cardData Captured card data from MSR swipe

- (void) deviceMessage:(NSString*)message;//!<Receives messages from the framework
//!< @param message String message transmitted by framework

/**
 CTLS Transaction Data
 
 This protocol will receive results from IDT_Device::ActivateTransaction:timeout:()
 
 
 @param ctlsData CTLS Results Data.  MSR track data, clearning record, Result code, card type, encryption type, masked tags, encrypted tags, unencrypted tags and KSN
 
 @param status The return status code
 
 
 */
- (void) ctlsTransactionData:(CTLSResponse*)ctlsData statusCode:(Byte)status;

@end

/**
 Class to drive the IDT_VendiMobile device
 */
@interface IDT_VendiMobile : NSObject<IDT_Device_Delegate>{
    id<IDT_VendiMobile_Delegate> delegate;
}

@property(strong) id<IDT_VendiMobile_Delegate> delegate;  //!<- Reference to IDT_VendiMobile_Delegate.

/**
 * Set VendiMobile II
 *
 Must be called before the sharedController is called if Vendi-Mobile needs to be accessed.  Default device is Vendi-Mobile II
 
 @param isVM2 TRUE = is VendiMobileII,  FALSE = is VendiMobile
  */
+(void) setVendiMobileII:(BOOL)isVM2;

/**
 * SDK Version
 *
 Returns the current version of IDTech.framework
 
 @retval  Framework version
 */
+(NSString*) SDK_version;

/**
 * Singleton Instance
 *
 Establishes an singleton instance of IDT_VendiMobile class.
 
 @retval  Instance of IDT_VendiMobile
 */
+(IDT_VendiMobile*) sharedController;


/**
 * Cancel Connect To Audio Reader
 * @retval RETURN_CODE
 *
 Cancels a connection attempt to an IDTech MSR device connected via the audio port.
 
 */

-(RETURN_CODE) device_cancelConnectToAudioReader;
/**
 * Connect To Audio Reader
 * @retval RETURN_CODE
 *
 Attemps to recognize and connect to an IDTech MSR device connected via the audio port.
 
 */

-(RETURN_CODE) device_connectToAudioReader;


/**
 * Polls device for Firmware Version
 
 *
 * @param response Response returned of Firmware Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_UniPay::device_getResponseCodeString:()
 *
 */
-(RETURN_CODE) device_getFirmwareVersion:(NSString**)response;

/**
 * Is Audio Reader Connected
 
 *
 Returns value on device connection status when device is an audio-type connected to headphone plug.
 
 @retval BOOL True = Connected, False = Disconnected
 
 */

-(BOOL) device_isAudioReaderConnected;




/**
 * Set Volume To Audio Reader
 
 *
 Set the iPhone’s volume for command communication with audio-based readers. The the range of iPhone’s volume is from 0.1 to 1.0.
 
 @param val Volume level from 0.1 to 1.0
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: no response from reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR or ICC task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to IDT_UniPay::device_getResponseCodeString:()
 
 */
-(RETURN_CODE) device_setAudioVolume:(float)val;


/**
 * Get Reader Status
 *
 Returns the reader status
 
 @param readerStatus Pointer that will return with the ICCReaderStatus results.
 
 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE.  Values can be parsed with IDT_BTPay::device_getResponseCodeString:()
 
 @code
 ICCReaderStatus* readerStatus;
 RETURN_CODE rt = [[IDT_Device sharedController] getICCReaderStatus:&readerStatus];
 if(RETURN_CODE_DO_SUCCESS != rt){
    NSLog(@"Fail");
 }
 else{
    NSString *sta;
if(readerStatus->iccPower)
    sta =@"[ICC Powered]";
 else
    sta = @"[ICC Power not Ready]";
if(readerStatus->cardSeated)
    sta =[NSString stringWithFormat:@"%@,[Card Seated]", sta];
 else
    sta =[NSString stringWithFormat:@"%@,[Card not Seated]", sta];
 
 
 @endcode
 */

-(RETURN_CODE) icc_getICCReaderStatus:(ICCReaderStatus**)readerStatus;

/**
 * Power On ICC
 
 
 *
 * Power up the currently selected microprocessor card in the ICC reader
 *
 * @param response Response returned. If Success, ATR String. If Failure, ASCII encoded data of error string
 *
 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE. 
 
 
 
 
 */
-(RETURN_CODE) icc_powerOnICC:(NSData**)response;



/**
 * Power Off ICC
 
 
 *
 * Powers down the ICC
 
 * @param error Returns the error, if any
 *
 * @retval RETURN_CODE:  Return codes listed as typedef enum in IDTCommon:RETURN_CODE.
 
 If Success, empty
 If Failure, ASCII encoded data of error string
 */
-(RETURN_CODE) icc_powerOffICC:(NSString**)error;



@end
