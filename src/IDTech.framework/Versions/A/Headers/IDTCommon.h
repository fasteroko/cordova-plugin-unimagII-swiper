//
//  IDTCommon.h
//  IDT
//
//  Created by Randy Palermo on 5/1/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//




/** Structure used to return response from IDT_BTPay::icc_getICCReaderStatus() and IDT_UniPay::icc_getICCReaderStatus()
 */
typedef struct {
    bool iccPower; //!< Determines if ICC has been powered up
    bool cardSeated;  //!< Determines if card is inserted
    bool latchClosed;  //!< Determines if Card Latch is engaged.  If device does not have a latch, value is always FALSE
    bool cardPresent;  //!< If device has a latch, determines if the card is present in device.  If the device does not have a latch, value is always FALSE
    bool magneticDataPresent;  //!< True = Magnetic data present, False = No Magnetic Data
}ICCReaderStatus;


/**
 Structure to set ICC power on options.  Used by IDT_BTPay::icc_powerOnICC:response:() IDT_UniPay::icc_powerOnICC:response:()
 */
typedef struct {
    BOOL sendIFS; //!< Send S(IFS) request if T=1 protocolError: Reference source not found
    BOOL explicitPPS;  //!< Explicit PPSError: Reference source not found
    BOOL disableAutoPPS; //!< No auto pps for negotiate mode
    BOOL disableResponseCheck;  //!< No check on response of S(IFS) request
    unsigned char* pps; //!< pps is used to set the Protocol and Parameters Selection between card and reader, only Di <= 4 are supported. pps must follow the structure specified in ISO 7816-3 as PPS0, [PPS1], [PPS2], and [PPS3]. For more information see ISO 7816-3 section 7.2.
    unsigned char ppsLength; //!< lenght of pps data
}PowerOnStructure;
#ifndef UNIPAY_SHOULD_SKIP_THIS

/**
 Certificate Authority Public Key
 
 Used as parameter in IDT_BTPay::emv_retrieveCAPK:response:(), IDT_BTPay::emv_removeCAPK:(), IDT_BTPay::emv_setCAPK:(), IDT_UniPay::emv_retrieveCAPK:response:(), IDT_UniPay::emv_removeCAPK:(), IDT_UniPay::emv_setCAPK:()
 
 Used as return value in IDT_BTPay::emv_retrieveCAPK:response:() IDT_UniPay::emv_retrieveCAPK:response:()
 */
typedef struct {
    unsigned char hashAlgorithm;       //!< Hash Algorithm  0x01 = SHA-1
    unsigned char encryptionAlgorithm;       //!< Encryption Algorithm 0x01 = RSA
    unsigned char rid[5];  //!< As per payment networks definition
    unsigned char index;  //!< As per payment networks definition
    unsigned char exponentLength;  //!< Length of exponent. 0x01 or 0x03 as per EMV specs
    unsigned char keyLength;  //!< Length of key. max 248 bytes as per EMV specs
    unsigned char exponent[3];  //!< CA Public Key Exponent
    unsigned char key[248];       //!< CA Public Key
    
} CAKey;


/**
 AID Entry - Used to populate array in IDT_BTPay::emv_retrieveAIDList:()  IDT_UniPay::emv_retrieveAIDList:().
 */
typedef struct {
    unsigned char aid[16];        //!< AID value as per payment networks.
    unsigned char aidLen;        //!< AID’s length.
} AIDEntry;

/**
 Mask and Encryption - Used to Set/Retrieve mask and encryption values IDT_BTPay::emv_retrieveAIDList:()  IDT_UniPay::emv_retrieveAIDList:().
 */
typedef struct {
    unsigned char prePANClear;        //!< Leading PAN digits to display. Values '0' - '6'.  Default '4'
    unsigned char postPANClear;        //!< Last PAN digits to display. Values '0' - '4'.  Default '4'
    unsigned char maskChar;        //!< Last PAN digits to display. Values 0x20-0x7E.  Default 0x2A '*'
    unsigned char displayExpDate;        //!< Mask or display expiration date. Values '0' = mask, '1' = don't mask.  Default '1'
    unsigned char baseKeyType;        //!< BTPay Only. Key Type. Values '0' = Data Key,  '1' = Pin Key.  Default '0'
    unsigned char encryptionType;     //!< BTPay Only. Key Type. Values '1' = TDES,  '2' = AES.  Default '1'
    unsigned char encryptionOption;     //!< UniPay II Only.
    //!< Bit 0: T1 force encrypt
    //!< Bit 1 : T2 force encrypt
    //!< Bit 2 : T3 force encrypt
    //!< Bit3 : T3 force encrypt when card type is 0
    unsigned char maskOption;     //!< UniPay II Only.
    //!< Bit0: T1 mask allowed
    //!< Bit1: T2 mask allowed
    //!< Bit2: T3 mask allowed
} MaskAndEncryption;


/**
 device AID File - 571 bytes
 
 Used as parameter in IDT_BTPay::emv_setApplicationData:()
 
 Used as return value of aidResponse in IDT_BTPay::emv_retrieveApplicationData:response:()
 */
typedef struct {
    unsigned char acquirerIdentifier[6];        //!< Indicates  which  acquirer/processor  processes  the corresponding AID. Tag 9F01
    unsigned char aid[16];        //!< AID value as per payment networks.  Tag 9F06
    unsigned char aidLen;        //!< AID’s length.
    unsigned char applicationSelectionIndicator;        //!< Standard parameter.
    unsigned char applicationVersionNumber[2];        //!< EMV application version number.   Tag 9F09
    unsigned char XAmount[3];        //!< Not used by Agnos Framework.
    unsigned char YAmount[3];        //!< Not used by Agnos Framework.
    unsigned char skipTACIACDefault;        //!< Indicates whether or not terminal uses default values for risk management.
    unsigned char tac;        //!< Indicates whether or not terminal uses Terminal Action Code.   0x00 or 0x01
    unsigned char floorlLimitChecking;        //!< Indicates whether  or  not  terminal  uses  Floor  Limit Checking.   0x00 or 0x01
    unsigned char randomTransactionSelection;        //!< Indicates whether  or  not  terminal  uses  Random Transaction Selection.   0x00 or 0x01
    unsigned char velocitiyChecking;        //!< Indicates whether  or  not  terminal  uses  Velocity Checking.   0x00 or 0x01
    unsigned char tACDenial[5];        //!< Terminal Action Code Denial.
    unsigned char tACOnline[5];        //!< Terminal Action Code Online.
    unsigned char tACDefault[5];        //!< Terminal Action Code Default.
    unsigned char terminalFloorLimit[3];        //!< Standard parameter.  Tag 9F1B
    unsigned char targetPercentage;        //!< EMV offline risk management parameter.
    unsigned char thresholdValue[3];        //!< EMV offline risk management parameter.
    unsigned char maxTargetPercentage;        //!< EMV offline risk management parameter.
    unsigned char defaultTDOL;        //!< Standard parameter.
    unsigned char tdolValue[252];        //!< Transaction Data Object List value.
    unsigned char tdolLen;        //!< Transaction Data Object List length.
    unsigned char defaultDDOL;        //!< Standard  parameter..  Tag
    unsigned char ddolValue[252];        //!< Dynamic Data Object List value.
    unsigned char ddolLen;        //!< Dynamic Data Object List length.
    unsigned char transactionCurrencyCode[2];        //!< AID’s currency.   Example: For Canada, {0x01,0x24}.  Tag 5F2A
    unsigned char transactionCurrencyExponent;        //!< Transaction Currency Exponent.   Example: Amount 4.53$ is managed as 453. Tag 5F36
    
} ApplicationID;


/**
 device Terminal Configuration File - 44 bytes
 
 Used as parameter in IDT_BTPay::setTerminalData:()
 
 Used as return value in IDT_BTPay::emv_retrieveTerminalData:()
 */
typedef struct {
    unsigned char terminalCountryCode[2];     //!< Terminal’s location. Tag 9F1A {0x08,0x40}
    unsigned char provideCardholderConfirmation;     //!< Indicates whether or not cardholder may confirm application selection at EMV Selection time. Tag 58 0x00 or 0x01
    unsigned char terminalType;     //!< Standard parameter. Tag 9F35  See EMVCo book IV
    unsigned char emvContact;     //!< Indicates whether terminal supports EMV contact. Tag 9F33, byte 1, bit 6 0x00 or 0x01
    unsigned char terminalCapabilities[3];     //!< Standard parameter.  Tag 9F33 See EMVCo book IV
    unsigned char additionalTerminalCapabilities[5];     //!< Standard parameter.  Tag 9F40 See EMVCo book IV
    unsigned char emvContactless;     //!< Indicates whether  or  not  terminal support scontactless  in EMV mode.  0x00 or 0x01
    unsigned char magstripe;     //!< Indicates whether terminal supports magstripe.  0x00 or 0x01
    unsigned char pinTimeOut;     //!< In seconds. Time allocated to cardholder to enter PIN.  Binary value Example  : 0x0F for 15s
    unsigned char batchManaged;     //!< Indicates whether or not Batch messages are supported by Terminal.  0x00 or 0x01
    unsigned char adviceManaged;     //!< Indicates whether or not Advice messages are supported by Terminal (only if needed by Level3 implementation).  0x00 or 0x01
    unsigned char pse;     //!< Indicates whether or not PSE Selection method is supported by Terminal.  0x00 or 0x01
    unsigned char autoRun;     //!< Indicates whether or not Terminal is configured in AutoRun.  0x00 or 0x01
    unsigned char predefinedAmount[3];     //!< Fixed amount.  Binary value
    unsigned char pinByPass;     //!< Indicates whether or not PIN bypass is supported by Terminal.  0x00 or 0x01
    unsigned char referalManaged;     //!< Indicates whether or not Referal managed are supported by Terminal (only if needed by Level3 implementation)..  0x00 or 0x01
    unsigned char defaultTAC;     //!< Indicates whether or not Default TAC are supported by Terminal.  0x00 or 0x01
    unsigned char defaultTACDenial[5];     //!< Default TAC Denial value.  See EMVCo book IV
    unsigned char defaultTACOnline[5];     //!< Default TAC Online value.  See EMVCo book IV
    unsigned char defaultTACDefault[5];     //!< Default TAC Default value.  See EMVCo book IV
    unsigned char notRTS;     //!< Indicates RTS are not supported by Terminal or not.  0x00 or 0x01
    unsigned char notVelocity;     //!< Indicates Velocity are not supported by Terminal or not.  0x00 or 0x01
    unsigned char cdaType;  //!< Supported CDA type. Value should be 0x02
} TerminalData;

/**
 Certificate Revocation List Entry - 9 bytes
 
 Used as parameter in IDT_BTPay::emv_retrieveCRLForRID:response:(), IDT_BTPay::emv_removeCRL:(), IDT_BTPay::emv_removeCRLUnit:(), IDT_BTPay::emv_setCRL:() IDT_UniPay::emv_retrieveCRLForRID:response:(), IDT_UniPay::emv_removeCRL:(), IDT_UniPay::emv_removeCRLUnit:(), IDT_UniPay::emv_setCRL:()
 
 */
typedef struct {
    unsigned char rid[5];  //!< As per payment networks definition
    unsigned char index;  //!< As per payment networks definition
    unsigned char serialNumber[3];  //!< As per payment networks definition
} CRLEntry;

#endif
//Versioning


typedef enum{
    POWER_ON_OPTION_IFS_FLAG=1,
    POWER_ON_OPTION_EXPLICIT_PPS_FLAG=2,
    POWER_ON_OPTION_AUTO_PPS_FLAG=64,
    POWER_ON_OPTION_IFS_RESPONSE_CHECK_FLAG=128
}POWER_ON_OPTION;

typedef enum{
    LANGUAGE_TYPE_ENGLISH=1,
    LANGUAGE_TYPE_PORTUGUESE,
    LANGUAGE_TYPE_SPANISH,
    LANGUAGE_TYPE_FRENCH
}LANGUAGE_TYPE;

typedef enum{
    PIN_KEY_TDES_MKSK_extp=0x00, //external plain text
    PIN_KEY_TDES_DUKPT_extp=0x01, //external plain text
    PIN_KEY_TDES_MKSK_intl=0x10,  //internal BTPay
    PIN_KEY_TDES_DUKPT_intl=0x11, //internal BTPay
    PIN_KEY_TDES_MKSK2_intl=0x20,  //internal UniPayII
    PIN_KEY_TDES_DUKPT2_intl=0x21, //internal  UniPayII
}PIN_KEY_Types;

#define EVENT_IDLE  0
#define EVENT_ASYNC_DATA 0x0100

typedef enum{
    EVENT_PINPAD_UNKNOWN = 11,
    EVENT_PINPAD_ENCRYPTED_PIN,
    EVENT_PINPAD_NUMERIC,
    EVENT_PINPAD_AMOUNT,
    EVENT_PINPAD_ACCOUNT,
    EVENT_PINPAD_ENCRYPTED_DATA,
    EVENT_PINPAD_CANCEL,
    EVENT_PINPAD_TIMEOUT,
    EVENT_PINPAD_FUNCTION_KEY,
    EVENT_PINPAD_DATA_ERROR
}EVENT_PINPAD_Types;

typedef enum{
    IDT_DEVICE_BTPAY_IOS = 0,
    IDT_DEVICE_BTPAY_OSX_BT,
    IDT_DEVICE_BTPAY_OSX_USB,
    IDT_DEVICE_UNIPAY_IOS,
    IDT_DEVICE_UNIPAY_OSX_USB,
    IDT_DEVICE_UNIPAYII_IOS,
    IDT_DEVICE_UNIPAYII_OSX_USB,
    IDT_DEVICE_IMAG_IOS,
    IDT_DEVICE_VENDI_MOBILE,
    IDT_DEVICE_VENDI_MOBILEII,
    IDT_DEVICE_UNIMAG
}IDT_DEVICE_Types;



typedef enum{
    EVENT_MSR_UNKNOWN = 31,
    EVENT_MSR_CARD_DATA,
    EVENT_MSR_CANCEL_KEY,
    EVENT_MSR_BACKSPACE_KEY,
    EVENT_MSR_ENTER_KEY,
    EVENT_MSR_DATA_ERROR,
    EVENT_MSR_ICC_START,
    EVENT_BTPAY_CARD_DATA,
    EVENT_UNIPAYII_EMV_NO_ICC_MSR_DATA,
    EVENT_UNIPAYII_EMV_FALLBACK_DATA
}EVENT_MSR_Types;

typedef enum{
    EVENT_ACTIVE_TRANSACTION = 51
}EVENT_CTLS_Types;

typedef enum{
    UNIMAG_COMMAND_DEFAULT_GENERAL_SETTINGS,
    UNIMAG_COMMAND_ENABLE_ERR_NOTIFICATION,
    UNIMAG_COMMAND_DISABLE_ERR_NOTIFICATION,
    UNIMAG_COMMAND_ENABLE_EXP_DATE,
    UNIMAG_COMMAND_DISABLE_EXP_DATE,
    UNIMAG_COMMAND_CLEAR_BUFFER,
    UNIMAG_COMMAND_RESET_BAUD_RATE
}UNIMAG_COMMAND_Types;

typedef enum {
    RETURN_CODE_DO_SUCCESS = 0,             //!< no error, beginning task
    RETURN_CODE_ERR_DISCONNECT,         //!< no response from reader
    RETURN_CODE_ERR_CMD_RESPONSE,       //!< invalid response data
    RETURN_CODE_ERR_TIMEDOUT,           //!< time out for task or CMD
    RETURN_CODE_ERR_INVALID_PARAMETER,  //!< wrong parameter
    RETURN_CODE_SDK_BUSY_MSR,           //!< SDK is doing MSR or ICC task
    RETURN_CODE_SDK_BUSY_PINPAD,        //!< SDK is doing PINPad task
    RETURN_CODE_SDK_BUSY_CTLS,        //!< SDK is doing CTLS task
    RETURN_CODE_ERR_OTHER,              //!< SDK is doing Other task
    RETURN_CODE_FAILED,                 //!< err response or data
    RETURN_CODE_NOT_ATTACHED,           //!< no reader attached
    RETURN_CODE_MONO_AUDIO,           //!< mono audio is enabled
    RETURN_CODE_CONNECTED,           //!< did connection
    RETURN_CODE_LOW_VOLUME,           //!< audio volume is too low
    RETURN_CODE_CANCELED,           //!< task or CMD be canceled
    RETURN_CODE_INVALID_STR,           //!< UF wrong string format
    RETURN_CODE_NO_FILE,           //!< UF file not found
    RETURN_CODE_INVALID_FILE,           //!< UF wrong file format
    
    
    RETURN_CODE_EMV_AUTHORIZATION_ACCEPTED = 0x0E00,          //!< Authorization Accepted
    RETURN_CODE_EMV_AUTHORIZATION_UNABLE_TO_GO_ONLINE = 0x0E01,   //!< Unable to go online
    RETURN_CODE_EMV_AUTHORIZATION_TECHNICAL_ISSUE = 0x0E02,   //!< Technical Issue
    RETURN_CODE_EMV_AUTHORIZATION_DECLINED = 0x0E03,           //!< Declined
    RETURN_CODE_EMV_AUTHORIZATION_ISSUER_REFERRAL = 0x0E04,           //!< Issuer Referral transaction
    
    RETURN_CODE_EMV_APPROVED = 0x0F00,   //!< Accept the online transaction
    RETURN_CODE_EMV_DECLINED = 0x0F01,   //!< Decline the online transaction
    RETURN_CODE_EMV_GO_ONLINE = 0x0F02,          //!< Request to go online
    RETURN_CODE_EMV_FAILED = 0x0F03,             //!< Transaction is terminated
    RETURN_CODE_EMV_SYSTEM_ERROR = 0x0F05,       //!< Application was not selected by kernel or ICC format error or ICC missing data error
    RETURN_CODE_EMV_NOT_ACCEPTED = 0x0F07,       //!< ICC didn't accept transaction
    RETURN_CODE_EMV_FALLBACK = 0x0F0A,           //!< Application may fallback to magstripe technology
    RETURN_CODE_EMV_CANCEL = 0x0F0C,             //!< Transaction was cancelled
    RETURN_CODE_EMV_TIMEOUT = 0x0F0D,             //!< Timeout
    RETURN_CODE_EMV_OTHER_ERROR = 0x0F0F,        //!< Other EMV Error
    RETURN_CODE_EMV_OFFLINE_APPROVED = 0x0F10,   //!< Accept the offline transaction
    RETURN_CODE_EMV_OFFLINE_DECLINED = 0x0F11,   //!< Decline the offline transaction
    
    
    
    RETURN_CODE_EMV_NEW_SELECTION = 0x0F21,      //!< ICC detected tah the conditions of use are not satisfied
    RETURN_CODE_EMV_NO_AVAILABLE_APPS = 0x0F22,   //!< No app were found on card matching terminal configuration
    RETURN_CODE_EMV_NO_TERMINAL_FILE = 0x0F23,   //!< Terminal file does not exist
    RETURN_CODE_EMV_NO_CAPK_FILE = 0x0F24,       //!< CAPK file does not exist
    RETURN_CODE_EMV_NO_CRL_ENTRY = 0x0F25,       //!< CRL Entry does not exist
    RETURN_CODE_BLOCKING_DISABLED = 0x0FFE,        //!< Return code when blocking is disabled
    RETURN_CODE_COMMAND_UNAVAILABLE = 0x0FFF       //!< Return code when command is not applicable on the selected device
    
    
} RETURN_CODE;

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
    EMV_RESULT_CODE_ERROR_DATA_LEN_INCORRECT = 0X1C,
    EMV_RESULT_CODE_CALL_YOUR_BANK = 0X1D,
    EMV_RESULT_CODE_NO_ICC_ON_CARD = 0X1E,
    EMV_RESULT_CODE_NEW_SELECTION = 0X1F,
    EMV_RESULT_CODE_START_TRANSACTION_SUCCESS = 0X20
} EMV_RESULT_CODE_Types;

typedef enum{
    EMV_AUTHORIZATION_RESULT_ACCEPTED = 0X00,
    EMV_AUTHORIZATION_RESULT_UNABLE_TO_GO_ONLINE = 0X01,
    EMV_AUTHORIZATION_RESULT_TECHNICAL_ISSUE = 0X02,
    EMV_AUTHORIZATION_RESULT_DECLINED = 0X03,
    EMV_AUTHORIZATION_RESULT_ISSUER_REFERAL = 0X04
} EMV_AUTHORIZATION_RESULT;




/** Common structures and enumerations shared by IDTech Framework classes.  Evaluate header source for more info **/

@interface IDTCommon : NSObject





@end
