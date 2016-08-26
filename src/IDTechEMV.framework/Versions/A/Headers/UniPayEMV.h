//
//  IDTechEMV.h
//  IDTech
//
//  Created by Randy Palermo on 7/31/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "APDUData.h"
#import <IDTech/IDTEMVData.h>
#import <IDTech/IDT_DEVICE.h>

/**
 device AID File - 47 bytes
 
 Used as parameter in IDTechEMV::setApplicationData:()
 
 Used as return value of aidResponse in IDTechEMV::retrieveApplicationData:()
 */
typedef struct {
    unsigned char acquirerIdentifier[6];        //!< Indicates  which  acquirer/processor  processes  the corresponding AID. Tag 9F01
    unsigned char aid[16];        //!< AID value as per payment networks.  Tag 9F06
    unsigned char aidLen;        //!< AID’s length.
    unsigned char applicationVersionNumber[2];        //!< EMV application version number.   Tag 9F09
    unsigned char tacDefault[5];        //!< Terminal Action Code Denial. Tag DF13
    unsigned char tacDenial[5];        //!< Terminal Action Code Denial.  Tag DF14
    unsigned char tacOnline[5];        //!< Terminal Action Code Denial.  Tag DF15
    unsigned char transactionCurrencyCode[2];        //!< AID’s currency.   Example: For US, {0x08,0x40}.  Tag 5F2A
    unsigned char transactionCurrencyExponent;        //!< Transaction Currency Exponent.   Example: Amount $4.53 is managed as 453. Tag 5F36
    unsigned char useTACDefault;  //!< Indicates if tacDefault value should be used
    unsigned char useTACDenial;  //!< Indicates if tacDefault value should be used
    unsigned char useTACOnline;  //!< Indicates if tacDefault value should be used
    unsigned char applicationSelectionIndicator;        //!< Indicates if partial AID matching is allowed.  0x01 = allowed DF62.


} UniPay_ApplicationID;

typedef enum{
    EMV_RC_APPROVED = 0X00,
    EMV_RC_DECLINED = 0X01,
    EMV_RC_GO_ONLINE = 0X02,
    EMV_RC_FAILED = 0X03,
    EMV_RC_SYSTEM_ERROR = 0X05,
    EMV_RC_NOT_ACCEPT = 0X07,
    EMV_RC_FALLBACK = 0X0A,
    EMV_RC_CANCEL = 0X0C,
    EMV_RC_OTHER_ERROR = 0X0F,
    EMV_RC_TIME_OUT = 0X0D,
    EMV_RC_OFFLINE_APPROVED = 0X10,
    EMV_RC_OFFLINE_DECLINED = 0X11,
    EMV_RC_REFERRAL_PROCESSING = 0X12,
    EMV_RC_ERROR_APP_PROCESSING = 0X13,
    EMV_RC_ERROR_APP_READING = 0X14,
    EMV_RC_ERROR_DATA_AUTH = 0X15,
    EMV_RC_ERROR_PROCESSING_RESTRICTIONS = 0X16,
    EMV_RC_ERROR_CVM_PROCESSING = 0X17,
    EMV_RC_ERROR_RISK_MGMT = 0X18,
    EMV_RC_ERROR_TERM_ACTION_ANALYSIS = 0X19,
    EMV_RC_ERROR_CARD_ACTION_ANALYSIS = 0X1A,
    EMV_RC_ERROR_APP_SELECTION_TIMEOUT = 0X1B,
    EMV_RC_ERROR_NO_CARD_INSERTED = 0X1C,
    EMV_RC_ERROR_APP_SELECTING = 0X1D,
    EMV_RC_ERROR_READING_CARD_APP = 0X1E,
    EMV_RC_ERROR_POWER_CARD_ERROR = 0X1F,
    EMV_RC_ERROR_NO_RESULT_CODE_PROVIDED_FOR_COMPLETION = 0X20,
    EMV_RC_APPROVED_WITH_ADVISE_NO_REASON = 0X21,
    EMV_RC_APPROVED_WITH_ADVISE_IA_FAILED = 0X22,
    EMV_RC_ERROR_AMOUNT_NOT_SPECIFIED = 0X23
} EMV_RC_Types;

typedef enum{
    MESSAGE_INSERT_CARD = 0,
    MESSAGE_REMOVE_CARD,
    MESSAGE_BAD_ICC,
    MESSAGE_TRANSACTION_CANCELLED,
    MESSAGE_FALLBACK_FAILED,
    MESSAGE_USE_CHIP_READER,
    MESSAGE_PROCESSING,
    MESSAGE_READY,
    MESSAGE_USE_MSR,
    MESSAGE_NOT_ACCEPTED
}MESSAGE_Types;

typedef enum{
    EMV_COMPLETION_RESULT_ACCEPTED = 0X00,
    EMV_COMPLETION_RESULT_UNABLE_TO_GO_ONLINE = 0X01,
    EMV_COMPLETION_RESULT_TECHNICAL_ISSUE = 0X02,
    EMV_COMPLETION_RESULT_DECLINED = 0X03,
    EMV_COMPLETION_RESULT_ISSUER_REFERAL = 0X04
} EMV_COMPLETION_RESULT;

/**
 device Terminal Configuration File - 45 bytes
 
 Used as parameter in   IDT_UniPay::setTerminalData:()
 
 Used as return value in    IDT_UniPay::setTerminalData:()
 */
typedef struct {
    unsigned char terminalCountryCode[2];     //!< Terminal’s location. Tag 9F1A {0x08,0x40}
    unsigned char merchantCategoryCode[2];     //!< Classifies the type of business being done by the mechant, represented according to ISO 8583:1993
    unsigned char merchantID[15];     //!< When concatenated with the Acquirer Identifer, uniquely identifies a given merchant
    unsigned char terminalID[8];     //!< Designates the unique location of a terminal at a merchant
    unsigned char defaultTACDefault[5];        //!< Default Terminal Action Code Denial. Tag DF13
    unsigned char defaultTACDenial[5];        //!< Default Terminal Action Code Denial.  Tag DF14
    unsigned char defaultTACOnline[5];        //!< Default Terminal Action Code Denial.  Tag DF15
    unsigned char useDefaultTACDefault;  //!< Indicates if tacDefault value should be used as terminal default value
    unsigned char useDefaultTACDenial;  //!< Indicates if tacDenial value should be used as terminal default value
    unsigned char useDefaultTACOnline;  //!< Indicates if tacOnline value should be used as terminal default value
    unsigned char terminalLocation[36]; //!< Terminal Location tag 9F4E

} UniPay_TerminalData;




/** Protocol methods established for IDTechEMV class  **/
@protocol IDTechEMV_Delegate <NSObject>
@optional
- (void) confirmApplicationSelection:(NSArray*)labelArray retry:(BOOL)tryAgain; //!<Receives available applications to select from during and EMV transactions when terminal is set for cardholder confirmation or a decision must be made between multiple applications available for selection. Array index of selected app must be passed back to IDT_UniPay::confirmApplication:() to continue the EMV transaction flow.
//!< @param labelArray Array of available application labels.  Use this array item index to pass back to IDT_UniPay::confirmApplication:()
//!< @param tryAgain Flag to indicate requirement to display "Try Again" along with application selection.

/**
 Language Preference
 
 After the kernal loads the Application, it will send back the Language Preference, tag 5F2D.  This can be used to determine what language to use when displaying prompts and messages for the remainder of the transaction flow.
 
 
 @param lang 1-4 languages stored in order of preference, each represented by 2 alphabetical characters according to ISO 639.
 */
- (void) languagePreference:(NSData*)lang;
/**
 EMV Transaction Data
 
 This protocol will receive results from IDT_Device::startEMVTransaction:otherAmount:timeout:cashback:additionalTags:()
 
 
 @param emvData EMV Results Data.  Result code, card type, encryption type, masked tags, encrypted tags, unencrypted tags and KSN
 
 @param error The error code as defined in the errors.h file
 
 @param reversal  If this flag is TRUE, this means the transaction was APPROVED online, but a TC was not returned by the kernel and the transaction was declined.  In most cases, the online host needs to be contacted to reverse the approval.
 
 
 */
- (void) emvTransactionData:(IDTEMVData*)emvData errorCode:(int)error performReversal:(BOOL)reversal;
/**
 EMV Messages
 
 During an EMV Transaction flow , this delegate with pass messages that to request further interaction, or post information
 
 @param message EMV Message Type-
 @code
 typedef enum{
    MESSAGE_INSERT_CARD = 0,
    MESSAGE_REMOVE_CARD

 }EMV_MESSAGE_Types;
 @endcode
 */
- (void) emvTransactionMessage:(MESSAGE_Types)message;

- (void) swipeMSRDataEMV:(IDTMSRData*)cardData  emv:(NSDictionary*)emvData;//!<Receives card data from MSR swipe.

@end

/**
 Class to drive the IDTechEMV device
 */
@interface IDTechEMV : NSObject <IDT_Device_Delegate> {
    id<IDTechEMV_Delegate> delegate;
}


@property(strong) id<IDTechEMV_Delegate> delegate;  //!<- Reference to IDTechEMV_Delegate.


/**
 * Singleton Instance
 *
 Establishes an singleton instance of IDTechEMV class.
 
 @retval  Instance of IDTechEMV
 */
+(IDTechEMV*) sharedController;


/**
 * Complete EMV Transaction Online Request
 *
 Completes an online EMV transaction request by the card
 
 The tags will be returned in the emvTransactionData delegate protocol.
 
 @param result EMV_COMPLETION_RESULT: Used to specify if contacting online host was successful or other problem occurred
 @param rc Result Code from host.  Manditory.  2 characters ASCII value.  Example "00"
 @param iad Issuer Authentication Data.  Optional.  10 bytes, 20 Hex Characters representing data.  Example  "11223344556677883030"
 @param scripts  Issuer Scripts.  Optional.  Data represented by Hex Characters.  TLV Format.  Must start with 71 or 72, followed by length, followed by data.  Example "711000112233445566778899AABBCCDDEEFF".  Can string multiple scripts, both 71 and 72.

 
 
Results are returned on delegate protocol emvTransactionData.
 

 
 */
-(void) completeOnlineEMVTransaction:(EMV_COMPLETION_RESULT)result resultCode:(NSString*)rc issuerAuthenticationData:(NSString*)iad issuerScripts:(NSString*)scripts;

/**
 Set Application
 
 During and EMV transaction flow, if there are multiple applications to chose from, or the terminal settings indicate cardholder confirmation for applicaiton selection, the delegate IDT_UniPay_Delegate::confirmApplicationSelection:() will receive an array with all the available applications to choose from.  The selected index of the application must be passed back to this method to continue the EMV transaction flow
 @param index The index of the selected app from the application array passed back from confirmApplicationSelection:()
 */
-(void) confirmApplication:(int)index;





/**
 * Get All Tags
 
 *
 Retrieves all EMV tags from the inserted card.  Only available after the card has been processed after executing IDTechEMV::startEMVTransaction:timeout:additionalTags:()
 
 @retval data NSdictionary with all tag values.  Key is NSString, Object is NSData.

 
 */
-(NSDictionary*) getAllTags;

/**
 * Returns SDK EMV Kernel Version
 *
 * @retval response Response returned of Kernel Version
 *
 
 *
 */
-(NSString*) getEMVKernelVersion;

/**
 * Get Tag
 
 *
 Retrieves an EMV tag from the inserted card.  Only available after the card has been processed after executing IDTechEMV::startEMVTransaction:timeout:additionalTags:()
 
 @param tagName Name fo tag to retrieve
 @retval data Tag data
 
 
 */
-(NSData*) getTag:(NSString*)tagName;


/**
 * Remove Application Data by AID
 *
 Removes the Application Data as specified by the AID name passed as a parameter
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 

 
 */
-(void) removeApplicationData:(NSString*)AID;








/**
 * Retrieve AID list
 *
 Returns all the AID name/length on the inserted ICC.  Populates response parameter with an dictionary with Keys of AID Names (NSData*), and values of AID Lengths (NSData*)
 
 @retval response Returns a NSArray of NSString of AID Names
 
 
 */
-(NSArray*) retrieveAIDList;


/**
 * Retrieve Application Data by AID
 *
 Retrieves the Application Data as specified by the AID name passed as a parameter.  The AID will be in the response parameter responseAID.  If aidLen = 0, then requested AID was not found.
 
 @param AID Name of ApplicationID in ASCII, example "A0000000031020".  Must be between 5 and 16 characters
 @retval responseAID  The AID returned from the method
 

 
 */
-(UniPay_ApplicationID) retrieveApplicationData:(NSString*)AID;




/**
 * Retrieve Terminal Data
 *
 Retrieves the Terminal Data.  The data will be in the response parameter
 
 @retval response Response returned as a TerminalData
 
 */
-(UniPay_TerminalData) retrieveTerminalData;



/**
 * Set Application Data by AID
 *
 Sets the Application Data as specified by the ApplicationID structure passed as a parameter
 
 @param data ApplicationID configuration file
 

 
 */
-(void) setApplicationData:(UniPay_ApplicationID)data;




/**
 * Set Tag
 
 *
 Sets an EMV tag from the tags read into the terminal.  Only available after the card has been processed after executing IDTechEMV::startEMVTransaction:timeout:additionalTags:()
 
 @param tagName Name of tag to set
 @param data Tag data
 

 
 */
-(void) setTag:(NSString*)tagName tagData:(NSData*)data;


/**
 * Preload EMV Card Data
 
 *
 This function will attempt to cache preliminary EMV card data.  It is to be used in an envvironment where there will be a few seconds between inserting the card and executing the startEMV command.  Example would be the idle time in collecting an amount before startEVM is executed.
 

 
 
 
 */
-(void) preloadEMVCardData;


/**
 * Set Terminal Data
 *
 Sets the Terminal Data as specified by the UniPay_TerminalData structure passed as a parameter
 
 @param data TerminalData configuration file
 
 */
-(void) setTerminalData:(UniPay_TerminalData)data;

/**
 * Receive Multi APDU
 *
 Receives a compound rAPDU response.  For internal use only
 
 @param rAPDU The multi rAPDU response
 
 */
-(void) receiveMultiAPDU:(NSArray*)rAPDU;

/**
 * Cancel Transaction
 *
 Cancels the current transaction
 
 */
- (void) cancelTransaction;

/**
 * Start EMV Transaction Request
 *
 Authorizes the EMV transaction amounts for an ICC card
 
 The tags will be returned in the emvTransactionData delegate protocol.
 
 @param amount Transaction amount value  (tag value 9F02)
 @param timeout Timeout value in seconds.
 @param type Transaction Type.
 @param tags Any other optional tags to be included in the request.  Passed as a mutable dictionary.
 
 
 
 
 
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


-(void) startEMVTransaction:(float)amount  timeout:(int)timeout transactionType:(int)type additionalTags:(NSMutableDictionary *)tags;




@end
