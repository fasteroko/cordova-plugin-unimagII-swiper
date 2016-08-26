//
//  IDTEMVData.h
//  IDTech
//
//  Created by Randy Palermo on 9/5/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IDTCommon.h"


/**
 * Encapsulating data class for EMV data capture
 */
@interface IDTEMVData : NSObject{
 @public
    /**
     Result Code. Uses enumeration EMV_RESULT_CODE_Types
     
     @code
     typedef enum{
     EMV_RESULT_CODE_APPROVED = 0X00,
     EMV_RESULT_CODE_DECLINED = 0X01,
     EMV_RESULT_CODE_GO_ONLINE = 0X02,
     EMV_RESULT_CODE_FAILED = 0X03,
     EMV_RESULT_CODE_SYSTEM_ERROR = 0X05,
     EMV_RESULT_CODE_NOT_ACCEPT = 0X07,
     EMV_RESULT_CODE_FALLBACK = 0X0A,
     EMV_RESULT_CODE_CANCEL = 0X0C,
     EMV_RESULT_CODE_OTHER_ERROR = 0X0F,
     EMV_RESULT_CODE_TIME_OUT = 0X0D,
     EMV_RESULT_CODE_OFFLINE_APPROVED = 0X10,
     EMV_RESULT_CODE_OFFLINE_DECLINED = 0X11,
     EMV_RESULT_CODE_REFERRAL_PROCESSING = 0X12,
     EMV_RESULT_CODE_ERROR_APP_PROCESSING = 0X13,
     EMV_RESULT_CODE_ERROR_APP_READING = 0X14,
     EMV_RESULT_CODE_ERROR_DATA_AUTH = 0X15,
     EMV_RESULT_CODE_ERROR_PROCESSING_RESTRICTIONS = 0X16,
     EMV_RESULT_CODE_ERROR_CVM_PROCESSING = 0X17,
     EMV_RESULT_CODE_ERROR_RISK_MGMT = 0X18,
     EMV_RESULT_CODE_ERROR_TERM_ACTION_ANALYSIS = 0X19,
     EMV_RESULT_CODE_ERROR_CARD_ACTION_ANALYSIS = 0X1A,
     EMV_RESULT_CODE_ERROR_APP_SELECTION_TIMEOUT = 0X1B,
     EMV_RESULT_CODE_ERROR_DATA_LEN_INCORRECT = 0X1C
     } EMV_RESULT_CODE_Types;
     @endcode
     */
    EMV_RESULT_CODE_Types  resultCode;
    int encryptionMode;  //!< 0 = TDES, 1 = AES
    int cardType;  //!< 0 = Contact, 1 = Contactless
    NSDictionary* unencryptedTags; //!< Unencrypted EMV Tags.  Key = tag name (NSString), Object = tag value (NSData)
    NSDictionary* encryptedTags; //!< Encrypted EMV Tags.  Key = tag name (NSString), Object = tag value (NSData)
    NSDictionary* maskedTags; //!< Encrypted EMV Tags.  Key = tag name (NSString), Object = tag value (NSData)
    NSData* KSN; //!< Key Serial Number for encrypted EMV tags
}

/**
 * clears all IDTEMVData properties
 */
-(void)clear;
/**
 * Singleton instance of IDTEMVData
 */
+ (IDTEMVData *)sharedController;

@property EMV_RESULT_CODE_Types resultCode;
@property int encryptionMode;
@property int cardType;
@property (nonatomic, strong) NSDictionary* unencryptedTags;
@property (nonatomic, strong) NSDictionary* encryptedTags;
@property (nonatomic, strong) NSDictionary* maskedTags;
@property (nonatomic, strong) NSData* KSN;

@end
