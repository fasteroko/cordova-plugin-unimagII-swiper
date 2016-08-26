//
//  IDTResult.h
//  IDTech
//
//  Created by Randy Palermo on 5/13/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IDTCommon.h"
#import "APDUResponse.h"

@class IDTResult;


//async task methods return value
//Description                   |Applicable task
//                                                          |Connect|Swipe|Cmd
typedef enum {            //--------------------------------+-------+-----+--
    RET_DO_SUCCESS=0,           //no error, beginning task  | *     | *   | * |
    RET_ERR_DISCONNECT,         //not connect with reader   | *     | *   | * |
    RET_ERR_CMD_FAILED,
    //send cmd fail, or response is err code   | *     | *   | * |
    RET_ERR_CMD_RESPONSE,       //invalid response data     | *     | *   | * |
    RET_ERR_TIMEDOUT,           //time out for task or CMD  | *     | *   | * |
    RET_ERR_INVALID_PARAMETER,  //wrong parameter           |       |     | * |
    RET_SDK_BUSY_MSR,           //SDK is doing MSR task     | *     |     | * |
    RET_SDK_BUSY_PINPad,        //SDK is doing PINPad task  | *     | *   | * |
    RET_ERR_OTHER,              //SDK is doing PINPad task  | *     | *   | * |
} IDTStatus;

static inline NSString* IDTStatus_lookup(IDTStatus c) {
#define URLOOK(a) case a: return @#a;
    switch (c) {
            URLOOK(RET_DO_SUCCESS)
            URLOOK(RET_ERR_DISCONNECT)
            URLOOK(RET_ERR_CMD_FAILED)
            URLOOK(RET_ERR_CMD_RESPONSE)
            URLOOK(RET_ERR_TIMEDOUT)
            URLOOK(RET_ERR_INVALID_PARAMETER)
            URLOOK(RET_SDK_BUSY_MSR)
            URLOOK(RET_SDK_BUSY_PINPad)
            URLOOK(RET_ERR_OTHER)
        default: return @"<unknown code>";
    }
#undef URLOOK
}
/**  Class to encapsulate data 
 */

@interface IDTResult : NSObject{
    /**
     Result Status
     
     @code
     typedef enum {
        RET_DO_SUCCESS=0,           no error, beginning task
        RET_ERR_DISCONNECT,         not connect with reader
        RET_ERR_CMD_FAILED,         send cmd fail, or response is err code
        RET_ERR_CMD_RESPONSE,       invalid response data
        RET_ERR_TIMEDOUT,           time out for task or CMD
        RET_ERR_INVALID_PARAMETER,  wrong parameter
        RET_SDK_BUSY_MSR,           SDK is doing MSR task
        RET_SDK_BUSY_PINPad,        SDK is doing PINPad task
        RET_ERR_OTHER,              SDK is doing PINPad task
     } RDStatus;
     
     typedef enum{
        EVENT_AUTHORIZATION_GO_ONLINE = 51,     Request to go online
        EVENT_AUTHORIZATION_OFFLINE_APPROVED,   Accept the offline transaction
        EVENT_AUTHORIZATION_OFFLINE_DECLINED,   Decline the offline transaction
        EVENT_AUTHORIZATION_APPROVED,           Accept the online transaction
        EVENT_AUTHORIZATION_DECLINED,           Decline the online transaction
        EVENT_AUTHORIZATION_FALLBACK,           Application may fallback to magstripe technology
        EVENT_AUTHORIZATION_NEW_SELECTION,      ICC detected tah the conditions of use are not satisfied
        EVENT_AUTHORIZATION_NOT_ACCEPTED,       ICC didn't accept transaction
        EVENT_AUTHORIZATION_CANCEL,             Transaction was cancelled
        EVENT_AUTHORIZATION_SYSTEM_ERROR,       Application was not selected by kernel or ICC format error or ICC missing data error
        EVENT_AUTHORIZATION_FAILED,             Transaction is terminated
        EVENT_AUTHORIZATION_OTHER_ERROR
     }EVENT_AUTHORIZATION_Types;
     
     @endcode
     */
    IDTStatus status;
    NSMutableData *data;  //!< Byte data returned from method
    iccReaderStatus readerStatus;  //!< Structure used to return response from BTPay::getICCReaderStatus()
    APDUResponse* apduResponse;  //!< Class used to return response from BTPay::exchangeAPDU:()
    /**
     Used to return list results from methods.
     -  Contains KEY=(NSData*)AID_NAME and VALUE=(NSData*)AID_LENGTH when returning values for BTPay::retrieveAIDList()
     -  Contains KEY=(NSData*)CAPK_RID and VALUE=(NSData*)CAPK_INDEX when returning values for BTPay::RetrieveCAPK:()
     -  Contains KEY=(NSString*)Tag_Name and VALUE=(NSData*)TagValue when returning values for BTPay::startEMVTransaction:otherAmount:timeout:processingCode:additionalTags:()
     -  Contains KEY=(NSString*)Tag_Name and VALUE=(NSData*)TagValue when returning values for BTPay::completeOnlineEMVTransaction:hostResponseTags:()
     */
    NSMutableDictionary* listResult;
    applicationID* aidResponse;  //!< Class used to return response from BTPay::RetrieveApplicationData:()
    terminalFile* terminalData;  //!< Class used to return response from BTPay::RetrieveTerminalData()
    caKey* caKeyData;  //!< Class used to return response from BTPay::RetrieveCAPK:()
    /**
     Used to return list results from methods.
     -  Contains NSData items, 6 bytes, bytes 1-5 RID, byte 6 index, when returning values for BTPay::RetrieveCAPK:()
     -  Contains NSData items for each RID, when returning values for BTPay::retrieveCRL()
     -  Contains NSData items for each Serial Number, when returning values for BTPay::retrieveCRLforRIDIndex:()
     */
    NSMutableArray* arrayResult;


    
}

/**
 * clears all IDTResult properties
 */
-(void)clear;

/**
 * Singleton instance of IDTResult
 */
+ (IDTResult *)sharedController;

@property (nonatomic, strong) NSMutableArray* arrayResult;
@property (nonatomic, strong) NSMutableDictionary* listResult;
@property (nonatomic, strong) NSMutableData *data;
@property (nonatomic, strong) APDUResponse* apduResponse;
@property IDTStatus status;
@property iccReaderStatus readerStatus;
@property applicationID* aidResponse;
@property caKey* caKeyData;
@property terminalFile* terminalData;

@end
