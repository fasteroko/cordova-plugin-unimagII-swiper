//
//  BTPay.h
//  
//  BTPay SDK
//  V1.01.006
//  ceated by Xinhu Li on 14-1-9. IDTECH. 
//

#import <Foundation/Foundation.h>

#ifdef __x86_64__
    #import <Cocoa/Cocoa.h>
    #import <IOBluetooth/objc/IOBluetoothDevice.h>
    #import <IOBluetooth/objc/IOBluetoothSDPUUID.h>
    #import <IOBluetooth/objc/IOBluetoothRFCOMMChannel.h>
    #import <IOBluetoothUI/objc/IOBluetoothDeviceSelectorController.h>

#else
    #import <ExternalAccessory/ExternalAccessory.h>
    #import <UIKit/UIKit.h>
#endif

#import "BTPay.h"
#import "IDTMSRData.h"
#import "IDTCommon.h"
#import "APDUResponse.h"



/** Protocol methods established for BTPay class  **/
@protocol BTPay_Delegate <NSObject>

@optional
-(void) deviceConnected; //!<Fires when BTPay200 connects.  If a connection is established before the delegate is established (no delegate to send initial connection notification to), this method will fire upon establishing the delegate.
-(void) deviceDisconnected; //!<Fires when BTPay200 disconnects.
- (void) dataInOutMonitor:(NSData*)data  incoming:(BOOL)isIncoming; //!<All incoming/outgoing data going to the device can be monitored through this delegate.
//!< @param data The serial data represented as a NSData object
//!< @param isIncoming The direction of the data
//!<- <c>TRUE</c> specifies data being received from the device,
//!<- <c>FALSE</c> indicates data being sent to the device.

- (void) swipeMSRData:(IDTMSRData*)cardData;//!<Receives card data from MSR swipe.
//!< @param cardData Captured card data from MSR swipe

/**
 All responses with their raw data (if any) will be returned to this delegate.
 
 @param code Return code
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 @param response Response data
 */
- (void) commandResponse:(RETURN_CODE)code respData:(NSData*)response;



/**
 Pinpad data delegate protocol
 
 Receives data from pinpad methods
 
  @param value encrypted data returned from BTPay::getEncryptedData:minLength:maxLength:encryptedDisplayMessage:(), or encrypted account number returned from BTPay::getCardAccount:max:line1:line2:(). String value returned from BTPay::getAmount:maxLength:encryptedDisplayMessage:() or BTPay::getNumeric:minLength:maxLength:encryptedDisplayMessage:(). PINblock returned from BTPay::getEncryptedPIN:keyType:line1:line2:line3:()
  @param KSN Key Serial Number returned from BTPay::getEncryptedPIN:keyType:line1:line2:line3:(), BTPay::getCardAccount:max:line1:line2:() or BTPay::getEncryptedData:minLength:maxLength:encryptedDisplayMessage:()
  @param event EVENT_PINPAD_Types PINpad event that solicited the data capture
 
 @code
 typedef enum{
    EVENT_PINPAD_UNKNOWN = 11,
    EVENT_PINPAD_ENCRYPTED_PIN,
    EVENT_PINPAD_NUMERIC,
    EVENT_PINPAD_AMOUNT,
    EVENT_PINPAD_ACCOUNT,
    EVENT_PINPAD_ENCRYPTED_DATA,
    EVENT_PINPAD_CANCEL,
    EVENT_PINPAD_TIMEOUT,
    EVENT_PINPAD_DATA_ERROR
 }EVENT_PINPAD_Types;
 @endcode
 */
- (void) pinpadData:(NSData*)value keySN:(NSData*)KSN event:(EVENT_PINPAD_Types)event;


@end
/**
 Class to drive the IDTech BTPay200
 */

#ifdef __x86_64__

@interface BTPay : NSObject{
    id<BTPay_Delegate> delegate;
    IOBluetoothDevice *mBluetoothDevice;
}
#else
@interface BTPay : NSObject<EAAccessoryDelegate, NSStreamDelegate>{
    id<BTPay_Delegate> delegate;
    
}
#endif


@property(strong) id<BTPay_Delegate> delegate;  //!<- Reference to BTPay_Delegate.


/**
 * Disable Blocking
 *
 Disables blocking of API methods.  Methods immediately return result code RETURN_CODE_BLOCKING_DISABLED 0xFFFF. Must wait for result to be returned in delegate - (void)commandResponse:(RETURN_CODE)code respData:(NSData*)response before another command is executed
 
 @param  isDisabled  TRUE = disable blocking, FALSE = enable blocking
 */
+(void) disableBlocking:(BOOL)isDisabled;


/**
 * Get Blocking Status
 *
 Returns the blocking status of the framework
 
 @retval  Blocking status: TRUE = blocking is disabled, FALSE = blocking enabled
 */
+(BOOL) isBlockingDisabled;

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
 Establishes an singleton instance of BTPay class.
 
 @retval  Instance of BTPay
 */
+(BTPay*) sharedController;

/**
 * Sets the IDTech Device Type
 *
 Tells the framework what device type to configure for, in addition to connection type (BlueTooth or USB-HID) when applicable.  Default is BTPay 200 over iOS
 
 @param deviceType  Select from the IDT_DEVICE_Types enumeration
 
 @code
 typedef enum{
    IDT_DEVICE_BTPAY_IOS = 0,
    IDT_DEVICE_BTPAY_OSX_BT,
    IDT_DEVICE_BTPAY_OSX_USB,
    IDT_DEVICE_UNIPAY,
    IDT_DEVICE_UNIPAY_OSX_USB,
    IDT_DEVICE_UNIPAYII,
    IDT_DEVICE_UNIPAYII_OSX_USB
 }IDT_DEVICE_Types;

 @endcode
 

 */
+(void) setDeviceType:(IDT_DEVICE_Types)deviceType;

//common part
-(id)init; //!< Initialize class. DEPRECATED: Use singleton instance [BTPay sharedConroller];
-(BOOL)open;  //!< Opens connection to device. DEPRECATED: Use singleton instance [BTPay sharedConroller] to automatically open/close device
-(void)close;  //!< Closes connection to device. DEPRECATED: Use singleton instance [BTPay sharedConroller] to automatically open/close device
-(void)destroy;  //!< Destroys connection to device. DEPRECATED: Use singleton instance [BTPay sharedConroller] to automatically open/close device



/**
 * Polls BTPay200 for EMV Kernel Version
 *
 * @param response Response returned of Kernel Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()

 *
 */
-(RETURN_CODE) getEMVKernelVersion:(NSString**)response;



/**
 * Start EMVT ransaction Request
 *
 Authorizes the EMV transaction amounts for an ICC card
 
 The tags will be returned in the NSMutableDictionary response parameter, with the KEY being NSString tag name, and the object being NSData tag value.
 
 @param amount Transaction amount value  (tag value 9F02)
 @param amtOther Other transaction amount value  (tag value 9F03)
 @param code Indicates the type of financial transaction, represented by the first two digits of ISO8583, Processing Code (tag value 9C)
 @param timeout Timeout value.
 @param tags Any other optional tags to be included in the request.  Passed as a mutable dictionary.
 @param response Response returned as a NSMutableDictionary with the Tag/Value resuls.  Key is NSString, Value is NSData.  Example:  to get the value for the key 8A:
 @code
 NSData* tagValue = [response objectForKey:@"8A"];
 @endcode
 
 
 * @retval RETURN_CODE:
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Go Online - RETURN_CODE_EMV_GO_ONLINE
 - 0xFF01: Offline Approved - RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF02: Offline Declined - RETURN_CODE_EMV_OFFLINE_DECLINED
 - 0xFF03: Approved - RETURN_CODE_EMV_APPROVED
 - 0xFF04: Declined - RETURN_CODE_EMV_DECLINED
 - 0xFF05: Fallback - RETURN_CODE_EMV_FALLBACK
 - 0xFF06: New Selection - RETURN_CODE_EMV_NEW_SELECTION
 - 0xFF07: Not Accepted - RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF08: Canceled - RETURN_CODE_EMV_CANCEL
 - 0xFF09: System Error - RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF0A: Failed - RETURN_CODE_EMV_FAILED
 - 0xFF0C: Uknown EMV error - RETURN_CODE_EMV_OTHER_ERROR
 - 0x0100 through 0xEFFF refer to BTPay::getResponseCodeString:()
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 95 | 05 | 0000008000
 9B | 02 | E800
 9F27 | 01 | 01
 8A | 02 | 5A33
 9F26 | 08 | 0C552B9364D55CE5
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"0000008000" forKey:@"95"];
 [emvTags setObject:@"E800" forKey:@"9B"];
 [emvTags setObject:@"01" forKey:@"9F27"];
 [emvTags setObject:@"5A33" forKey:@"8A"];
 [emvTags setObject:@"0C552B9364D55CE5" forKey:@"9F26"];
 
 return emvTags;
 
 }
 @endcode
 
 */
-(RETURN_CODE) startEMVTransaction:(float)amount otherAmount:(float)amtOther timeout:(int)timeout processingCode:(int)code additionalTags:(NSMutableDictionary *)tags returnTags:(NSMutableDictionary**)response;

/**
 * Complete EMV Transaction Online Request
 *
 Completes an online EMV transaction request by the card
 
 The tags will be returned in the NSMutableDictionary response parameter, with the KEY being NSString tag name, and the object being NSData tag value.
 
 @param isSuccess Determines if connection to host was successful:
 - TRUE: Online processing with the host (issuer) was completed
 - FALSE: Online processing could not be completed due to connection error with the host (issuer). No further data (tags) required.
 @param tags Host response tag:
 Tag | Length | Description
 ----- | ----- | -----
 8A | 2 | Data element Authorization Response Code. Mandatory
 91 | 8-16 | Issuer Authentication Data. Optional
 71 | 0-256 | Issuer Scripts. Optional
 72 | 0-256 | Issuer Scripts. Optional
 
 @param response Response returned as a NSMutableDictionary with the Tag/Value resuls.  Key is NSString, Value is NSData.  Example:  to get the value for the key 8A:
 @code
 NSData* tagValue = [response objectForKey:@"8A"];
 @endcode
 
 
 *  @retval RETURN_CODE:
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0xFF00: Go Online - RETURN_CODE_EMV_GO_ONLINE
 - 0xFF01: Offline Approved - RETURN_CODE_EMV_OFFLINE_APPROVED
 - 0xFF02: Offline Declined - RETURN_CODE_EMV_OFFLINE_DECLINED
 - 0xFF03: Approved - RETURN_CODE_EMV_APPROVED
 - 0xFF04: Declined - RETURN_CODE_EMV_DECLINED
 - 0xFF05: Fallback - RETURN_CODE_EMV_FALLBACK
 - 0xFF06: New Selection - RETURN_CODE_EMV_NEW_SELECTION
 - 0xFF07: Not Accepted - RETURN_CODE_EMV_NOT_ACCEPTED
 - 0xFF08: Canceled - RETURN_CODE_EMV_CANCEL
 - 0xFF09: System Error - RETURN_CODE_EMV_SYSTEM_ERROR
 - 0xFF0A: Failed - RETURN_CODE_EMV_FAILED
 - 0xFF0C: Uknown EMV error - RETURN_CODE_EMV_OTHER_ERROR
 - 0x0100 through 0xEFFF refer to BTPay::getResponseCodeString:()
 
 
 
 \par Converting TLV to NSMutableDictionary
 
 EMV data is  received in TLV (Tag, Length, value) format:
 `950500000080009B02E8009F2701018A025A339F26080C552B9364D55CE5`
 
 This data contains the following EMV tags/values:
 
 Tag | Length | Value
 ----- | ----- | -----
 95 | 05 | 0000008000
 9B | 02 | E800
 9F27 | 01 | 01
 8A | 02 | 5A33
 9F26 | 08 | 0C552B9364D55CE5
 
 An example how to create an NSMutableDictionary with these values follows.
 
 @code
 -(NSMutableDictionary*) createTLVDict{
 
 NSMutableDictionary *emvTags = [[NSMutableDictionary alloc] initWithCapacity:0];
 
 [emvTags setObject:@"0000008000" forKey:@"95"];
 [emvTags setObject:@"E800" forKey:@"9B"];
 [emvTags setObject:@"01" forKey:@"9F27"];
 [emvTags setObject:@"5A33" forKey:@"8A"];
 [emvTags setObject:@"0C552B9364D55CE5" forKey:@"9F26"];
 
 return emvTags;
 
 }
 @endcode
 
 */
-(RETURN_CODE) completeOnlineEMVTransaction:(BOOL)isSuccess hostResponseTags:(NSMutableDictionary *)tags returnTags:(NSMutableDictionary**)response;



/**
 * Set Certificate Revocation List Entry
 *
 Sets the CRL entry as specified by the crlEntry structure
 
 @param key crlEntry containing the RID, Index, and serial number to set
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setCRL:(crlEntry)key;

/**
 * Remove Certificate Revocation List RID
 *
 Removes all crlEntry as specified by the RID and Index passed as a parameter in the crlEntry structure
 
 @param key crlEntry containing the RID and Index  of the of the entries to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCRL:(crlEntry)key;

/**
 * Remove Certificate Revocation List unit
 *
 Removes a single crlEntry as specified by the RID/Index/Serial Number passed as a parameter in the crlEntry structure
 
 @param key crlEntry containing the RID, Index and serial number of the of the entry to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCRLUnit:(crlEntry)key;

/**
 * Retrieve the Certificate Revocation List specific to RID and index
 *
 Returns all the serial numbers for a specific RID/Index on the CRL.
 
  @param key crlEntry containing the RID and Index of the certificate to search for
  @param response Response returned as an NSArray of NSData  objects for each Serial Number found
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) retrieveCRLforRIDIndex:(crlEntry)key response:(NSMutableArray**)response;

/**
 * Retrieve the Certificate Revocation List
 *
 Returns all the RID in the CRL.
 @param response Response returned as an NSArray of NSData 5-byte objects for each RID
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
  */
-(RETURN_CODE) retrieveCRL:(NSMutableArray**)response;



/**
 * Retrieve Certificate Authority Public Key
 *
 Retrieves the CAPK as specified by the RID/Index  passed as a parameter in the caKey structure.  The CAPK will be in the response parameter
 
 @param key caKey containing the RID and Index of the key to retrieve
 @param response Response returned as a caKey
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 

 */
-(RETURN_CODE) retrieveCAPK:(caKey)key response:(caKey*)response;

/**
 * Remove Certificate Authority Public Key
 *
 Removes the CAPK as specified by the RID/Index passed as a parameter in the caKey structure
 
 @param key caKey containing the RID and Index of the key to remove
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeCAPK:(caKey)key;

/**
 * Set Certificate Authority Public Key
 *
 Sets the CAPK as specified by the caKey structure
 
 @param key caKey containing the RID, Index, and key data to set
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setCAPK:(caKey)key;

/**
 * Retrieve the Certificate Authority Public Key list
 *
 Returns all the CAPK RID and Index.  Populates response parameter with an array of NSData items, 6 bytes each, bytes 1-5 RID, byte 6 index
 
 @param response Response returned contains an NSArray of NSData 6-bytes objects:  First five bytes are the RID, and sixth byte is the index
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
*/
-(RETURN_CODE) retrieveCAPKList:(NSMutableArray**)response;



/**
 * Retrieve Terminal Data
 *
 Retrieves the Terminal Data.  The data will be in the response parameter
 
 @param response Response returned as a terminalFile
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) retrieveTerminalData:(terminalFile*)response;

/**
 * Remove Terminal Data
 *
 Removes the Terminal Data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeTerminalData;

/**
 * Set Terminal Data
 *
 Sets the Terminal Data as specified by the terminalFile structure passed as a parameter
 
 @param data terminalFile configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setTerminalData:(terminalFile)data;


/**
 * Retrieve Application Data by AID
 *
 Retrieves the Application Data as specified by the AID name passed as a parameter.  The AID will be in the response parameter responseAID
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 @param responseAID  The response returned from the method
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 

 
 */
-(RETURN_CODE) retrieveApplicationData:(NSString*)AID response:(applicationID*)responseAID;

/**
 * Remove Application Data by AID
 *
 Removes the Application Data as specified by the AID name passed as a parameter
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) removeApplicationData:(NSString*)AID;

/**
 * Set Application Data by AID
 *
 Sets the Application Data as specified by the applicationID structure passed as a parameter
 
 @param data applicationID configuration file
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setApplicationData:(applicationID)data;

/**
 * Retrieve AID list
 *
 Returns all the AID name/length on the inserted ICC.  Populates response parameter with an dictionary with Keys of AID Names (NSData*), and values of AID Lengths (NSData*)
 
 @param response Returns a dictionary of AID Names/Length
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) retrieveAIDList:(NSMutableDictionary**)response;



/**
 * Keep Accessory Connected
 *
 * Framework attempts to disconnect from accessory whever application goes to backround.  Setting this value to TRUE will disable framework disconnect attempts.  NOTE:  ExternalAccessory may still disconnect device when going to background by default.  If you want to stay connected to device, you must also set the .pist "Required Background Modes" to "App communicates using CoreBluetooth", "App communicates with an accessory", and "App shares data using CoreBluetooth"
 *
 * @param stayConnected  TRUE = stay connected while in background (assuming .plist is properly configured)
 */
-(void)stayConnected:(BOOL)stayConnected;


/**
 * Exchange APDU
 *
 * Sends an APDU packet to the ICC.  If successful, response is returned in APDUResult class instance in response parameter.
 
 @param dataAPDU  APDU data packet
 @param response Unencrypted/encrypted parsed APDU response
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */

-(RETURN_CODE) exchangeAPDU:(NSData*)dataAPDU response:(APDUResponse**)response;

/**
 * Power Off ICC
 *
 * Powers down the ICC
 
 * @param error Returns the error, if any
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 If Success, empty
 If Failure, ASCII encoded data of error string
 */
-(RETURN_CODE) powerOffICC:(NSString**)error;


/**
 * Power On ICC
 *
 * Power up the currently selected microprocessor card in the ICC reader
 *
 * @param response Response returned. If Success, ATR String. If Failure, ASCII encoded data of error string
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 
 
 */
-(RETURN_CODE) powerOnICC:(NSData**)response;

/**
 * Power On ICC with Options
 *
 * Power up the currently selected microprocessor card in the ICC reader, specifying IFS/pps options.
 
 @param options ATR Options
* @param response Response returned. If Success, ATR String. If Failure, ASCII encoded data of error string
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 
 */
-(RETURN_CODE) powerOnICC:(powerOnStructure)options response:(NSData**)response;


/**
 * Command Timout
 *
 * Sets the amount of seconds to wait before a timeout error occurs
 *
 * @param nSecond  Timout value.  Valid range 1 - 60 seconds
 
 * @retval Success flag.  Determines if value was set and in range.
 */
-(BOOL) setCmdTimeOutDuration: (int) nSecond;


/**
 * Send a NSData object to BTPay200
 *
 * Sends a command represented by the provide NSData object to the BTPay200 through the accessory protocol.
 *
 * @param cmd NSData representation of command to execute
 * @param lrc If <c>TRUE</c>, this will wrap command with start/length/lrc/sum/end:  '<STX><Len_Low><Len_High> data <CheckLRC> <CheckSUM> <ETX>'
   @param response Response data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 */
-(RETURN_CODE) sendDataCommand:(NSData*)cmd calcLRC:(BOOL)lrc response:(NSData**)response;


/**
 * Get Reader Status
 *
 Returns the reader status
 
 @param readerStatus Pointer that will return with the iccReaderStatus results.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
@code
    iccReaderStatus readerStatus;
    RETURN_CODE rt = [[BTPay sharedController] getICCReaderStatus:&readerStatus];
    if(RETURN_CODE_DO_SUCCESS != rt){
        NSLog(@"Fail");
    }
    else{
        NSString *sta;
        if(readerStatus.iccPower)
            sta =@"[ICC Powered]";
        else
            sta = @"[ICC Power not Ready]";
        if(readerStatus.cardSeated)
            sta =[NSString stringWithFormat:@"%@,[Card Seated]", sta];
        else
            sta =[NSString stringWithFormat:@"%@,[Card not Seated]", sta];
 
        NSLog(@"Card Status = %@",sta);
 }
@endcode
*/

-(RETURN_CODE) getICCReaderStatus:(iccReaderStatus*)readerStatus;

/**
 * Get Secure Message
 *
 Request a secure message.  Polls the framework bundle for a resource matching the requested message.  Secure message may be referenced by their clear text (case sensitive), or by their reference number.  Example, to request the message to request a PO number (in French), sending @"1007", or @"No COMMANDE" will retrieve the correct message data
 
 @param reference Message ID or Message Text (case sensitive).  Example "28" or "ENTER FLEET NO" produce same results.
 
 * @retval Secure message RSA 2048 encoded for passing to:
 - BTPay::getEncryptedData:minLength:maxLength:encryptedDisplayMessage:()
 - BTPay::getNumeric:minLength:maxLength:encryptedDisplayMessage:()
 - BTPay::getAmount:maxLength:encryptedDisplayMessage:()
 
 \par Secure Messages
 Secure messages to be used with General Prompts commands
 For French Msg ID, add 1000.  Example "ENTREZ" id 1001.
 For Spanish Msg ID, add 2000. Example "INGRESE" id 2001.
 
 Msg Id |English Prompt |French Prompt |Spanish Prompt
 ---------- | ---------- | ----------  | ----------
 1|"ENTER" |"ENTREZ" |"INGRESE"
 2|"REENTER" |"RE-ENTREZ" |"REINGRESE"
 3|"ENTER YOUR" |"ENTREZ VOTRE" |"INGRESE SU"
 4|"REENTER YOUR" |"RE-ENTREZ VOTRE" |"REINGRESE SU"
 5|"PLEASE ENTER" |"SVP ENTREZ" |"POR FAVOR INGRESE"
 6|"PLEASE REENTER" |"SVP RE-ENTREZ" |"POR FAVO REINGRESE"
 7|"PO NUMBER" |"No COMMANDE" |"NUMERO PO"
 8|"DRIVER ID" |"ID CONDUCTEUR" |"LICENCIA"
 9|"ODOMETER" |"ODOMETRE" |"ODOMETRO"
 10|"ID NUMBER" |"No IDENT" |"NUMERO ID"
 11|"EQUIP CODE" |"CODE EQUIPEMENT" |"CODIGO EQUIP"
 12|"DRIVERS ID" |"ID CONDUCTEUR" |"ID CONDUCTOR"
 13|"JOB NUMBER" |"No TRAVAIL" |"NUMERO EMP"
 14|"WORK ORDER" |"FICHE TRAVAIL" |"ORDEN TRABAJO"
 15|"VEHICLE ID" |"ID VEHICULE" |"ID VEHICULO"
 16|"ENTER DRIVER" |"ENTR CONDUCTEUR" |"INGRESE CONDUCTOR"
 17|"ENTER DEPT" |"ENTR DEPARTEMNT" |"INGRESE DEPT"
 18|"ENTER PHONE" |"ENTR No TELEPH" |"INGRESE TELEFONO"
 19|"ENTER ROUTE" |"ENTREZ ROUTE" |"INGRESE RUTA"
 20|"ENTER FLEET" |"ENTREZ PARC AUTO" |"INGRESE FLOTA"
 21|"ENTER JOB ID" |"ENTR ID TRAVAIL" |"INGRESE ID TRABAJO"
 22|"ROUTE NUMBER" |"No ROUTE" |"RUTA NUMERO"
 23|"ENTER USER ID"|"ID UTILISATEUR" |"INGRESE ID USUARIO"
 24|"FLEET NUMBER"|"No PARC AUTO"|"FLOTA NUMERO"
 25|"ENTER PRODUCT"|"ENTREZ PRODUIT"|"INGRESE PRODUCTO"
 26|"DRIVER NUMBER"|"No CONDUCTEUR"|"CONDUCTOR NUMERO"
 27|"ENTER LICENSE"|"ENTREZ PERMIS"|"INGRESE LICENCIA"
 28|"ENTER FLEET NO"|"ENT No PARC AUTO"|"INGRESE NRO FLOTA"
 29|"ENTER CAR WASH"|"ENTREZ LAVE-AUTO"|"INGRESE LAVADO"
 30|"ENTER VEHICLE"|"ENTREZ VEHICULE"|"INGRESE VEHICULO"
 31|"ENTER TRAILER"|"ENTREZ REMORQUE"|"INGRESE TRAILER"
 32|"ENTER ODOMETER"|"ENTREZ ODOMETRE"|"INGRESE ODOMETRO"
 33|"DRIVER LICENSE"|"PERMIS CONDUIRE"|"LICENCIA CONDUCTOR"
 34|"ENTER CUSTOMER"|"ENTREZ CLIENT"|"INGRESE CLIENTE"
 35|"VEHICLE NUMBER"|"No VEHICULE" | "VEHICULO NUMERO"
 36|"ENTER CUST DATA"|"INFO CLIENT"|"INGRESE INFO CLIENTE"
 37|"REENTER DRIVID"|"RE-ENTR ID COND"|"REINGRESE ID CHOFER"
 38|"ENTER USER DATA"|"INFO UTILISATEUR"|"INGRESE INFO USUARIO"
 39|"ENTER CUST CODE"|"ENTR CODE CLIENT"|"INGRESE COD. CLIENTE"
 40|"ENTER EMPLOYEE"|"ENTREZ EMPLOYE"|"INGRESE EMPLEADO"
 41|"ENTER ID NUMBER"|"ENTREZ No ID"|"INGRESE NUMERO ID"
 42|"ENTER DRIVER ID"|"No CONDUCTEUR"|"INGRESE ID CONDUCTOR"
 43|"ENTER FLEET PIN"|"NIP PARC AUTO"|"INGRESE PIN DE FLOTA"
 44|"ODOMETER NUMBER"|"No ODOMETRE"|"ODOMETRO NUMERO"
 45|"ENTER DRIVER LIC"|"PERMIS CONDUIRE"|"INGRESE LIC CONDUCTOR"
 46|"ENTER TRAILER NO"|"ENT No REMORQUE"|"INGRESE NRO TRAILER"
 47|"REENTER VEHICLE"|"RE-ENTR VEHICULE"|"REINGRESE VEHICULO"
 48|"ENTER VEHICLE ID"|"ENTR ID VEHICULE"|"INGRESE ID VEHICULO"
 49|"ENTER BIRTH DATE"|"ENT DT NAISSANCE"|"INGRESE FECHA NAC"
 50|"ENTER DOB MMDDYY"|"NAISSANCE MMJJAA"|"INGRESE FDN MMDDAA"
 51|"ENTER FLEET DATA"|"INFO PARC AUTO"|"INGRESE INFO DE FLOTA"
 52|"ENTER REFERENCE"|"ENTREZ REFERENCE"|"INGRESE REFERENCIA"
 53|"ENTER AUTH NUMBR"|"No AUTORISATION"|"INGRESE NUMERO AUT"
 54|"ENTER HUB NUMBER"|"ENTREZ No NOYAU"|"INGRESE NRO HUB"
 55|"ENTER HUBOMETER"|"COMPTEUR NOYAU"|"INGRESE MEDIDO DE HUB"
 56|"ENTER TRAILER ID"|"ENT ID REMORQUE"|"INGRESE ID TRAILER"
 57|"ODOMETER READING"|"LECTURE ODOMETRE"|"LECTURA ODOMETRO"
 58|"REENTER ODOMETER"|"RE-ENT ODOMETRE"|"REINGRESE ODOMETRO"
 59|"REENTER DRIV. ID"|"RE-ENT ID CONDUC"|"REINGRESE ID CHOFER"
 60|"ENTER CUSTOMER ID"|"ENTREZ ID CLIENT"|"INGRESE ID CLIENTE"
 61|"ENTER CUST. ID"|"ENTREZ ID CLIENT"|"INGRESE ID CLIENTE"
 62|"ENTER ROUTE NUM"| "ENT No ROUTE"|"INGRESE NUM RUTA"
 63|"ENTER FLEET NUM"| "ENT No PARC AUTO"|"INGRESE NUM FLOTA"
 64|"FLEET PIN"|"NIP PARC AUTO"|"PIN DE FLOTA"
 65|"DRIVER #"|"CONDUCTEUR"|"CONDUCTOR #"
 66|"ENTER DRIVER #"|"ENT # CONDUCTEUR"|"INGRESE CONDUCTOR #"
 67|"VEHICLE #"|"# VEHICULE"|"VEHICULO #"
 68|"ENTER VEHICLE #"|"ENT # VEHICULE"|"INGRESE VEHICULO #"
 69|"JOB #"|"# TRAVAIL"|"TRABAJO #"
 70|"ENTER JOB #"|"ENTREZ # TRAVAIL"|"INGRESE TRABAJO #"
 71|"DEPT NUMBER"|"No DEPARTEMENT"|"NUMERO DEPTO"
 72|"DEPARTMENT #"|"DEPARTEMENT"|"DEPARTAMENTO #"
 73|"ENTER DEPT #"|"ENT# DEPARTEMENT"|"INGRESE DEPTO #"
 74|"LICENSE NUMBER"|"No PERMIS"|"NUMERO LICENCIA"
 75|"LICENSE #"|"# PERMIS"|"LICENCIA #"
 76|"ENTER LICENSE #"|"ENTREZ # PERMIS"|"INGRESE LICENCIA #"
 77|"DATA"|"INFO"|"INFO"
 78|"ENTER DATA"|"ENTREZ INFO"|"INGRESE INFO"
 79|"CUSTOMER DATA"|"INFO CLIENT"|"INFO CLIENTE"
 80|"ID #"|"# ID"|"ID #"
 81|"ENTER ID #"|"ENTREZ # ID"|"INGRESE ID #"
 82|"USER ID"|"ID UTILISATEUR"|"ID USUARIO"
 83|"ROUTE #"|"# ROUTE"|"RUTA #"
 84|"ENTER ROUTE #"|"ENTREZ # ROUTE"|"INGRESE RUTA #"
 85|"ENTER CARD NUM"|"ENTREZ NO CARTE"|"INGRESE NUM TARJETA"
 86|"EXP DATE(YYMM)"|"DATE EXPIR(AAMM)"|"FECHA EXP (AAMM)"
 87|"PHONE NUMBER"|"NO TEL"|"NUMERO TELEFONO"
 88|"CVV START DATE"|"CVV DATE DE DEBUT"|"CVV FECHA INICIO"
 89|"ISSUE NUMBER"|"NO DEMISSION"|"NUMERO DE EMISION"
 90|"START DATE (MMYY)"|"DATE DE DEBUT-AAMM"|"FECHA INICIO (AAMM)"
 
 */

-(NSData*) getSecureMessage:(NSString*)reference;

/**
 * Set Bluetooth Address
 *
 Sets the Bluetooth address of the device. 6 bytes, example F0DE07CCA03F.
 
 @param address 6 Byte address represented by a 12-character HEX string
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */

-(RETURN_CODE) setBluetoothAddress:(NSString*)address;


/**
 * Reboot Device
 *
 Executes a command to restart the device.
 *
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */

-(RETURN_CODE) rebootDevice;

/**
 * Sends a Beep Value
 *
 Executes a beep on the BT200.  The complete beep may be defined as a multiple of single beep tones.
 *
 @param beep Unsigned short array containing freq1,dur1,freq2,dur2,. . . freq#,dur#.  Frequency is in Hz and must be in the range 2000-4000. Duration is in milliseconds.
 @param num Number of tones in the beep array.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 Example Code-
 @code
 unsigned short beep[] = {0xb00,0x400,0x800,0x300};
 RETURN_CODE rt = [[BTPay sharedController] sendBeep:beep numberOfTones:2];
 NSLog(@"\nControl Beep Return Status Code %i ",  rts);
 @endcode
 
 */
-(RETURN_CODE) sendBeep:(unsigned short*)beep numberOfTones:(int)num;

/**
 * DisplayMessage
 *
 Display up to 4 lines of text in the device LCD.
 
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 @param line4 Display line 4, up to 16 characters
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) displayMessage:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3 line4:(NSString*)line4;

/**
 * Set Enter Sleep Mode Time
 *
Sets seconds of idle that must pass before entering sleep mode
 
 @param seconds  Amount of time (in seconds) that must pass during idle before unit goes to sleep
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setIdleTimeForSleep: (int) seconds;

/**
 * Put BTPay200 to sleep
 *
 Set device to enter  sleep mode. In sleep mode, LCD display and backlight is off. It can be waked up by key press or sending commands
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) enterSleepMode;

/**
 * Polls BTPay200 for Firmware Version
 *
 * @param response Response returned of Firmware Version
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
  *
 */
-(RETURN_CODE) getFirmwareVersion:(NSString**)response;

/**
 * Polls BTPay200 for Model Number
 *
 * @param response  Returns Model Number
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 *
 */
-(RETURN_CODE) getModelNumber:(NSString**)response;

/**
 * Polls BTPay200 for Serial Number
 *
 * @param response  Returns Serial Number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 *
 */
-(RETURN_CODE) getSerialNumber:(NSString**)response;

/**
 * Set Serial Number
 *
 Set device's serial number and Bluetooth name, then reboots device. Bluetooth name will be set as BTPay + Space + Serial number
 *
 @param strSN Device serial number
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) setSerialNumber:(NSString*)strSN;

/**
 * Polls BTPay200 for current Date/Time
 *
 * @param response Response returned as ASCII Data of Date  YYMMDDhhmmss. Example 140215171628 = Feb. 15, 2014, 28 seconds into 5:16pm.
 *
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 

 
 @code
 NSString* response;
 RETURN_CODE rt = [[BTPay sharedController] getDateTime:&response];
    if (RETURN_CODE_DO_SUCCESS == rt)
    {
        NSLog* (@"Date Time (YYMMDDhhmmss) = %@",response);
    }
 @endcode
 *
 */
-(RETURN_CODE) getDateTime:(NSString**)response;

/**
 * Set BTPay200 device Date/Time
 *
 Set device's date/time
 *
 @param date Device date represented by a YYMMDDhhmmss.  Example March 12, 2014, 6:30pm (and 12 seconds) = 140312183012
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setDateTime:(NSString*)date;


/**
 * Cancel PIN Command
 *
 
 This command can cancel BTPay:getEncryptedPIN:keyType:line1:line2:line3:() and BTPay::getNumeric:minLength:maxLength:encryptedDisplayMessage:() and BTPay::getAmount:maxLength:encryptedDisplayMessage:() and BTPay::getCardAccount:max:line1:line2:() and BTPay::getEncryptedData:minLength:maxLength:encryptedDisplayMessage:() */
-(RETURN_CODE) cancelPin;


/**
 * Set PIN Length
 *
 Sets the encrypted PIN length.
 
 @param minLength Minimum PIN length at least 4
 @param maxLength Maximum PIN length not to exceed 12
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) setPinLength:(int)minLength maxLength:(int)maxLength;

/**
 Get PIN Length
 
 * Returns encrypted PIN Length
 *
 @param response Returned Value of Minimum/Maximum PIN length:
  - response[0] = minimum length
  - response[1] = maximum length
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 @code
    NSData* res;
    RETURN_CODE rt = [[BTPay sharedController] getPinLength:&res];
    uint8_t b[res.length];
    [data getBytes:b];
    if(RETURN_CODE_DO_SUCCESS == rt && res.length>1){
    NSLog(@"GetPinLength: min=%d max=%d", b[0], b[1]);
    }
 @endcode
 */
-(RETURN_CODE) getPinLength:(NSData**)response;

/**
 Get PINPad Status
 
 * Returns status of PINpad
 *
 @param response Pinpad status. response[0]:
 - 0x01: PINpad is inactivate.
 - 0x02: PINpad  has been activated, but Public Key is not loaded.
 - 0x03: Public key has been loaded, but Firmware Key, Numeric Key and Check Value is not loaded.
 - 0x10: PINpad  normal work status.
 - 0x30: PINpad  suspend status if password input error.
 - 0x31: PINpad  suspend status if get PIN(MKSK)120 times in one hours.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 @code
    Byte *b = (Byte*)malloc(1);
    RETURN_CODE rt = [[BTPay sharedController] getPINpadStatus];
    if(RETURN_CODE_DO_SUCCESS == rt){
    [self appendMessageToResults:[NSString stringWithFormat:@"PINPad Status: %d", b[0]]];
 
 }
 @endcode
 }

 */
-(RETURN_CODE) getPINpadStatus:(NSData**)response;

/**
 * Set encrypted MSR Data Output Format
 *
 * Sets how data will be encrypted, with either Data Key or PIN key (if MSR DUKPT key loaded)
 *
 @param encryption Encryption Type
  - 00: Encrypt with Data Key
  - 01: Encrypte with PIN Key
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 @code
    NSData* res;
    RETURN_CODE rt = [[BTPay sharedController] getPINpadStatus:&res];
    uint8_t b[res.length];
    [data getBytes:b];
    if(RETURN_CODE_DO_SUCCESS == rt && res.length>0){
    NSLog(@"getPINpadStatus:%d", b[0]);
    }
 @endcode
 
 
 */
-(RETURN_CODE) setEncryptMSRFormat:(int)encryption;

/**
 * Get encrypted MSR Data Output Format
 *
 * Specifies if data will be encrypted with Data Key or PIN key (if MSR DUKPT key loaded)
 *
 * @param response Response returned from method:
 - '0': Encrypted card data with Data Key if MSR DUKPT Key had been loaded.(default)
 - '1': Encrypted card data with PIN Key if MSR DUKPT Key had been loaded.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()

 */
-(RETURN_CODE) getEncryptMSRFormat:(NSString**)response;

/**
 * Display Message and Get Encrypted PIN online
 *
 Prompts the user with up to 3 lines of text. Returns pinblock/ksn of entered PIN value in BTPay200Delegate::pinpadData:keySN:event:() with event MessageID_PINEntry
 
 @param account Card account number
 @param type Encryption Key Type:
 - 0x00: External Account Key Plain Text PIN_KEY_TDES_MKSK_extp
 - 0x01: External Account Key Plain Text PIN_KEY_TDES_DUKPT_extp
 - 0x10: External Account Key Cipher PIN_KEY_TDES_MKSK_extc
 - 0x11: External Account Key Cipher PIN_KEY_TDES_DUKPT_extc
 - 0x20: Internal Account Key PIN_KEY_TDES_MKSK_intl
 - 0x21: Internal Account Key PIN_KEY_TDES_DUKPT_intl
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 -
 
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered PIN key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA format .
 - When press Cancel key, the entered PIN  key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getEncryptedPIN:(NSString*)account keyType:(PIN_KEY_Types)type line1:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3;


/**
 * Display Message and Enable MSR Swipe
 *
 Prompts the user with up to 3 lines of text. Enables MSR, waiting for swipe to occur. Returns IDTMSRData instance to BTPay200Delegate::swipeMSRData:()
 
 During waiting for swiping card, it will receive all commands except BTPay::getEncryptedPIN:keyType:line1:line2:line3:() and BTPay::getNumeric:minLength:maxLength:encryptedDisplayMessage:() and BTPay::getAmount:maxLength:encryptedDisplayMessage:() and BTPay::getCardAccount:max:line1:line2:() and BTPay::getEncryptedData:minLength:maxLength:encryptedDisplayMessage:()
 
 
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 @param line3 Display line 3, up to 16 characters
 -
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */
-(RETURN_CODE) startMSRSwipe:(NSString*)line1 line2:(NSString*)line2 line3:(NSString*)line3;

/**
 * Disable MSR Swipe
 
 Cancels MSR swipe request.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 */
-(RETURN_CODE) cancelMSRSwipe;

/**
 * Get Response Code String
 *
 Interpret a BTPay response code and return string description.
 
 @param errorCode Error code, range 0x0000 - 0xFFFF, example 0x0300

 
 * @retval Verbose error description
 
 HEX VALUE | Description
  ------- | -------
 0x0000 | No error, beginning task
 0x0001 | Not connect with reader
 0x0002 | Invalid response data
 0x0003 | Time out for task or CMD
 0x0004 | Wrong parameter
 0x0005 | SDK is doing MSR task
 0x0006 | SDK is doing PINPad task
 0x0007 | SDK is doing Other task
 0x0300 | Key Type(TDES) of Session Key is not same as the related Master Key.
 0x0400 | Related Key was not loaded.
 0x0500 | Key Same.
 0x0702 | PAN is Error Key.
 0x0D00 | This Key had been loaded.
 0x0E00 | Base Time was loaded.
 0x1800 | Send “Cancel Command” after send “Get Encrypted PIN” &”Get Numeric “& “Get Amount”
 0x1900 | Press “Cancel” key after send “Get Encrypted PIN” &”Get Numeric “& “Get Amount”
 0x30FF | Security Chip is not connect
 0x3000 | Security Chip is deactivation & Device is In Removal Legally State.
 0x3101 | Security Chip is activation &  Device is In Removal Legally State.
 0x5500 | No Admin DUKPT Key.
 0x5501 | Admin  DUKPT Key STOP.
 0x5502 | Admin DUKPT Key KSN is Error.
 0x5503 | Get Authentication Code1 Failed.
 0x5504 | Validate Authentication Code Error.
 0x5505 | Encrypt or Decrypt data failed.
 0x5506 | Not Support the New Key Type.
 0x5507 | New Key Index is Error.
 0x5508 | Step Error.
 0x550F | Other Error.
 0x6000 | Save or Config Failed / Or Read Config Error.
 0x6200 | No Serial Number.
 0x6900 | Invalid Command - Protocol is right, but task ID is invalid.
 0x6A00 | Unsupported Command - Protocol and task ID are right, but command is invalid.
 0x6B00 | Unknown parameter in command - Protocol task ID and command are right, but parameter is invalid.
 0x7200 | Device is suspend (MKSK suspend or press password suspend).
 0x7300 | PIN DUKPT is STOP (21 bit 1).
 0x7400 | Device is Busy.
 0xE100 | Can not enter sleep mode.
 0xE200 | File has existed.
 0xE300 | File has not existed.
 0xE400 | Open File Error.
 0xE500 | SmartCard Error.
 0xE600 | Get MSR Card data is error.
 0xE700 | Command time out.
 0xE800 | File read or write is error.
 0xE900 | Active 1850 error!
 0xEA00 | Load bootloader error.
 0xEF00 | Protocol Error- STX or ETX or check error.
 0xEB00 | Picture is not exist.
 0x2C06 | no card seated to request ATR
 0x2D01 | Card Not Supported,
 0x2D03 | Card Not Supported, wants CRC
 0x690D | Command not supported on reader without ICC support
 0x8100 | ICC error time out on power-up
 0x8200 | invalid TS character received
 0x8500 | pps confirmation error
 0x8600 | Unsupported F, D, or combination of F and D
 0x8700 | protocol not supported EMV TD1 out of range
 0x8800 | power not at proper level
 0x8900 | ATR length too long
 0x8B01 | EMV invalid TA1 byte value
 0x8B02 | EMV TB1 required
 0x8B03 | EMV Unsupported TB1 only 00 allowed
 0x8B04 | EMV Card Error, invalid BWI or CWI
 0x8B06 | EMV TB2 not allowed in ATR
 0x8B07 | EMV TC2 out of range
 0x8B08 | EMV TC2 out of range
 0x8B09 | per EMV96 TA3 must be > 0xF
 0x8B10 | ICC error on power-up
 0x8B11 | EMV T=1 then TB3 required
 0x8B12 | Card Error, invalid BWI or CWI
 0x8B13 | Card Error, invalid BWI or CWI
 0x8B17 | EMV TC1/TB3 conflict*
 0x8B20 | EMV TD2 out of range must be T=1
 0x8C00 | TCK error
 0xA304 | connector has no voltage setting
 0xA305 | ICC error on power-up invalid (SBLK(IFSD) exchange
 0xE301 | ICC error after session star
 0xFF00 | EMV: Request to go online
 0xFF01 | EMV: Accept the offline transaction
 0xFF02 | EMV: Decline the offline transaction
 0xFF03 | EMV: Accept the online transaction
 0xFF04 | EMV: Decline the online transaction
 0xFF05 | EMV: Application may fallback to magstripe technology
 0xFF06 | EMV: ICC detected that the conditions of use are not satisfied
 0xFF07 | EMV: ICC didn't accept transaction
 0xFF08 | EMV: Transaction was cancelled
 0xFF09 | EMV: Application was not selected by kernel or ICC format error or ICC missing data error
 0xFF0A | EMV: Transaction is terminated
 0xFF0B | EMV: Other EMV Error

 */
-(NSString *) getResponseCodeString: (int) errorCode;



/**
 * Display Message and Get Card Account
 *
 Show message on LCD and get card account number from keypad, then return encrypted card account number. Returns encryptedData of entered account in BTPay200Delegate::pinpadData:keySN:event:() with event EVENT_PINPAD_ACCOUNT
 
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param line1 Display line 1, up to 12 characters
 @param line2 Display line 2, up to 16 characters
 -
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 \par Notes
 - If  there is no any enter in 3 minutes, this command time out.
 - If  there is no any enter in 20 seconds, the entered account numbers will be cleared.
 - When press Enter key, it will end this command and respond package with NGA format.
 - When press Cancel key, the entered account numbers will be cleared and if press Cancel key again, this command terminated.
 - Cancel command can terminate this command.
 */
-(RETURN_CODE) getCardAccount:(int)minLength max:(int)maxLength line1:(NSString*)line1 line2:(NSString*)line2;


/**
 * Display Message and Get Encrypted Data
 *
 Decrypt and display message on LCD. Prompts the user with up to 2 lines of text. Returns value of encrypted data (using MSR DUKPT key) and KSN to BTPay200Delegate::pinpadData:keySN:event:() with event EVENT_PINPAD_ENCRYPTED_DATA
 
 @param lastPackage Last package flag
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param encData Encrypted secure message.  Default secure messages can be retrieved from BTPay::getSecureMessage:(). Alternately, secure messages not in the default list may be provided by a trusted source.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 \par Notes
 
 -  If  there is no any enter in 3 minutes, this command time out.
 -  If  there is no any enter in 20 seconds, the entered account numbers will be cleared.
 -  When press Enter key, it will end this command and respond package with NGA format.
 -  When press Cancel key, the entered account numbers will be cleared and if press Cancel key again, this command terminated.
 -  Cancel command can terminate this command.
 -  Maximum pattern number allowed is 10. If any error or invalid command is sent, input data patterns will be cleared and command fail.
 
 */
-(RETURN_CODE) getEncryptedData:(BOOL)lastPackage minLength:(int)minLength maxLength:(int)maxLength encryptedDisplayMessage:(NSData*)encData;

/**
 * Display Message and Get Numeric Key(s)
 *
 Decrypt and display message on LCD. Requires secure message data. Returns value in inputValue of BTPay200Delegate::pinpadData:keySN:event:() with event EVENT_PINPAD_NUMERIC
 
 @param maskInput If true, all entered data will be masked with asterik (*)
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param encData Encrypted secure message.  Default secure messages can be retrieved from BTPay::getSecureMessage:(). Alternately, secure messages not in the default list may be provided by a trusted source.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered numeric key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA  format .
 - When press Cancel key, the entered numeric key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getNumeric:(bool)maskInput minLength:(int)minLength maxLength:(int)maxLength encryptedDisplayMessage:(NSData*)encData;


/**
 * Display Message and Get Amount
 *
 Decrypt and display message on LCD. Requires secure message data. Returns value in inputValue of BTPay200Delegate::pinpadData:keySN:event:() with event EVENT_PINPAD_AMOUNT
 
 @param minLength Minimum account number length - not less than 1
 @param maxLength Maximum account number length - not more than 16
 @param encData Encrypted secure message.  Default secure messages can be retrieved from BTPay::getSecureMessage:(). Alternately, secure messages not in the default list may be provided by a trusted source.
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 \par Notes
 - If there is no any enter in 3 minutes, this command will time out.
 - If there is no any enter in 20 seconds, the entered namount key will be cleared.
 - When press Enter key , it will end this Command and response package with NGA  format .
 - When press Cancel key, the entered amount key will be cleared and if press Cancel key again, this command terminated.
 - Cancel Command can terminate this command.
 
 */
-(RETURN_CODE) getAmount:(int)minLength maxLength:(int)maxLength encryptedDisplayMessage:(NSData*)encData;


/**
 * Upload JPEG to device
 *
 Stores a picture on the device. The picture's dimensions must not exceed the display resolution of 128 x 64. The picture must be RGB JPEG.
 @param picture RGB JPEG image data
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 */
-(RETURN_CODE) uploadJPEG:(NSData*)picture;

/**
 * Show stored picture on the LCD
 *
 Show stored picture on the LCD defined by top left point [X0][Y0] and bottom right point [X1][Y1].  The values of X must be in the range 0-127, and the values of Y must be in the range of 0-63
 @param X0 Upper left X coordinate
 @param Y0 Upper left Y coordinate
 @param X1 Lower left X coordinate
 @param Y1 Lower left Y coordinate
 
 * @retval RETURN_CODE:
 - 0x0000: Success: no error - RETURN_CODE_DO_SUCCESS
 - 0x0001: Disconnect: not connect with reader - RETURN_CODE_ERR_DISCONNECT
 - 0x0002: Invalid Response: invalid response data - RETURN_CODE_ERR_CMD_RESPONSE
 - 0x0003: Timeout: time out for task or CMD - RETURN_CODE_ERR_TIMEDOUT
 - 0x0004: Invalid Parameter: wrong parameter - RETURN_CODE_ERR_INVALID_PARAMETER
 - 0x0005: MSR Busy: SDK is doing MSR Task - RETURN_CODE_SDK_BUSY_MSR
 - 0x0006: PINPad Busy:  SDK is doing PINPad task - RETURN_CODE_SDK_BUSY_PINPAD
 - 0x0007: Unknown:  Unknown error - RETURN_CODE_ERR_OTHER
 - 0x0100 through 0xFFFF refer to BTPay::getResponseCodeString:()
 
 
 */

-(RETURN_CODE) showJPEG:(int)X0 Y0:(int)Y0 X1:(int)X1 Y1:(int)Y1;



@end
