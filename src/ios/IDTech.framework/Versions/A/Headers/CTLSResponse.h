//
//  CTLSResponse.h
//  IDTech
//
//  Created by Randy Palermo on 10/30/14.
//  Copyright (c) 2014 IDTech Products. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IDTEMVData.h"

@class CTLSResponse;

/**
 Class to encapsulate the CTLSResponse
 */
@interface CTLSResponse : NSObject {
    NSString* track1MSR; //!<Track 1 data from MSR
    NSString* track2MSR; //!<Track 2 data from MSR
    NSDictionary* clearingRecord; //!<DE 055 data (if available) as a TLV data objects. The DE 055 data is the same data as is included in the Clearing Record
    NSDictionary* unencryptedTags; //!<EMV card data
    NSDictionary* encryptedTags; //!<EMV card data
    NSDictionary* maskedTags; //!<EMV card data
}

/**
 * clears all CTLSResponse properties
 */
-(void)clear;
/**
 * Singleton instance of CTLSResponse
 */
+ (CTLSResponse *)sharedController;


@property (nonatomic, strong) NSString* track1MSR;
@property (nonatomic, strong) NSString* track2MSR;
@property (nonatomic, strong) NSDictionary* clearingRecord;
@property (nonatomic, strong) NSDictionary* unencryptedTags;
@property (nonatomic, strong) NSDictionary* encryptedTags;
@property (nonatomic, strong) NSDictionary* maskedTags;

@end
