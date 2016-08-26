
//  UniMagHelper.m
//  OrderPort
//
//  Created by mak on 27.05.14.
//  Copyright (c) 2014 almighty technologies. All rights reserved.
//

#import "UniMagHelper.h"
#import <IDTech/IDTech.h>
#import <IDTechEMV/IDTechEMV.h>
#import <MediaPlayer/MediaPlayer.h>

@interface UniMagHelper() <IDT_UniPay_Delegate, IDTechEMV_Delegate>

#if !(TARGET_IPHONE_SIMULATOR)
// Device
@property (nonatomic, strong) uniMag *uniReader;
#endif

@end

@implementation UniMagHelper

+ (instancetype)shared
{
    static dispatch_once_t onceToken;
    static UniMagHelper *instance;
    dispatch_once(&onceToken, ^{
        instance = [[UniMagHelper alloc] init];
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self setType:UnimagTypeMag];
    }
    return self;
}

- (void)setDelegate:(id<UniMagHelperDelegate>)delegate
{
    _delegate = delegate;
}

- (void)setType:(UnimagType)type
{
    #if !(TARGET_IPHONE_SIMULATOR)
    _type = type;
    if (type == UnimagTypeMag){
//        [[Mixpanel sharedInstance] track:@"SELECT_SWIPER" properties:@{@"Type" : @"UniMag"}];
//    [Ouralabs log:OULogLevelWarn tag:@"SELECT_SWIPER" message:@"SELECT_SWIPER" kvp:@{@"Type" : @"UniMag"}];
        _serialNumber = @"";
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc addObserver:self selector:@selector(connectedNotification:) name:uniMagDidConnectNotification object:nil];
        [nc addObserver:self selector:@selector(disconnectNotification:) name:uniMagDidDisconnectNotification object:nil];
        [nc addObserver:self selector:@selector(dataProcessingNotification:) name:uniMagDataProcessingNotification object:nil];
        [nc addObserver:self selector:@selector(invalidSwipeNotification:) name:uniMagInvalidSwipeNotification object:nil];
        [nc addObserver:self selector:@selector(dataReceivedNotification:) name:uniMagDidReceiveDataNotification object:nil];
        [nc addObserver:self selector:@selector(attachmentNotification:) name:uniMagAttachmentNotification object:nil];
        [nc addObserver:self selector:@selector(detachmentNotification:) name:uniMagDetachmentNotification object:nil];
        [uniMag enableLogging:YES];
        [nc addObserver:self selector:@selector(commandReceivedNotification:) name:uniMagDidReceiveCmdNotification object:nil];
        
        self.uniReader = [[uniMag alloc] init];
        [self.uniReader setSwipeTimeoutDuration:0];
        [self.uniReader setAutoAdjustVolume:TRUE];
    }
    if (type == UnimagTypePay){
//        [Ouralabs log:OULogLevelWarn tag:@"SELECT_SWIPER" message:@"SELECT_SWIPER" kvp:@{@"Type" : @"UniPay"}];
        
        [IDT_UniPay sharedController].delegate = self;
        [[IDT_UniPay sharedController] config_setCmdTimeOutDuration: 4];
        [IDTechEMV sharedController].delegate = self;
    }
    #endif
}

- (NSString *)deviceName
{
    if (_type == UnimagTypeMag){
        return @"UniMag 2 /Shuttle";
    }
    if (_type == UnimagTypePay){
        return @"UniPay";
    }
    return @"";
}

- (void)updateDeviceStatus
{
     #if !(TARGET_IPHONE_SIMULATOR)
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidStatusChange:)]){
        if (_type == UnimagTypeMag){
            [self.delegate uniMagDidStatusChange:[self.uniReader getConnectionStatus]];
        }
        if (_type == UnimagTypePay){
            BOOL b = [[IDT_UniPay sharedController] isConnected];
//            [Ouralabs log:OULogLevelWarn tag:@"IDT_UNIPAY_CONNECTION" message:@"IDT_UNIPAY_CONNECTION" kvp:@{@"Connected" : @(b)}];
            [self.delegate uniMagDidStatusChange:b];
            /*
            BOOL status = [[IDT_UniPay sharedController] isConnected];
            [Ouralabs log:OULogLevelWarn tag:@"IDT_UNIPAY_CONNECTION" message:@"IDT_UNIPAY_CONNECTION" kvp:@{@"Connected" : @(status)}];
            [self.delegate uniMagDidStatusChange:status]; */
        }
    }
    #endif
}

- (void)requestSwipe
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        if (![self isDeviceConnected]){
            [self.uniReader startUniMag:YES];
        }
        [self.uniReader requestSwipe];
    }
    if (_type == UnimagTypePay){
        MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
        if (musicPlayer.volume != 1){
            [musicPlayer setVolume:1];
        }
        
        [[IDT_UniPay sharedController] msr_startMSRSwipe:0];
//        [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_REQUEST_SWIPE" message:@"UNIPAY_REQUEST_SWIPE" kvp:nil];
        
    }
    #endif
}

- (void)cancelSwipe
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        [self.uniReader cancelTask];
        [self.uniReader startUniMag:NO];
    }
    if (_type == UnimagTypePay){
//        [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_CANCEL_SWIPE" message:@"UNIPAY_CANCEL_SWIPE" kvp:nil];
        [[IDT_UniPay sharedController] msr_cancelMSRSwipe];
    }
    #endif
}

- (BOOL)isDeviceConnected
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        return [self.uniReader getConnectionStatus];
    }
    if (_type == UnimagTypePay){
        return [[IDT_UniPay sharedController] isConnected];
    }
    #endif
    return NO;
}

- (BOOL)isDeviceAttached
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        return self.uniReader.isReaderAttached;
    }
    if (_type == UnimagTypePay){
        return [[IDT_UniPay sharedController] isConnected];
    }
    #endif
    return NO;
}

-(void)reset
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        [self.uniReader startUniMag:YES];
    }
    if (_type == UnimagTypePay){
        [[IDT_UniPay sharedController] device_rebootDevice];
    }
    #endif
}

- (void)getSerial
{
    #if !(TARGET_IPHONE_SIMULATOR)
    [self.uniReader sendCommandGetSerialNumber];
    #endif
}

- (void)serialNum
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (_type == UnimagTypeMag){
        [self.uniReader startUniMag:YES];
    }
    if (_type == UnimagTypePay){
        NSString *serial = @"";
        [[IDT_UniPay sharedController] config_getSerialNumber:&serial];
        // TODO - provide serial here
    }
    #endif
}

#pragma mark - IDTechEMV_Delegate

- (void)emvTransactionMessage:(MESSAGE_Types)message
{
    NSString *messageReceived = [self messageToString:message];
//    [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_EMV_TRANSACTION_MESSAGE"  message:@"UNIPAY_EMV_TRANSACTION_MESSAGE" kvp:@{@"Code" : @(message), @"Message" : messageReceived}];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_MESSAGE: %@", messageReceived]];
    }
    if (message == MESSAGE_REMOVE_CARD){
        /*
        NSMutableDictionary* dict = [NSMutableDictionary new];
        
        [dict setObject:[self hexToData:@"3030"] forKey:@"8a"];
        
        NSDictionary* myDict;
        myDict = nil;
        [[IDTechEMV sharedController] completeOnlineEMVTransaction:EMV_COMPLETION_RESULT_ACCEPTED resultCode:@"00" issuerAuthenticationData:nil issuerScripts:nil];
        
        [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_EMV_COMPLETE_TRANS"  message:@"UNIPAY_EMV_COMPLETE_TRANS" kvp:nil]; */
    }
}

- (NSString *)messageToString:(MESSAGE_Types)message
{
    NSString *messageReceived = @"";
    #if !(TARGET_IPHONE_SIMULATOR)

    switch (message) {
        case MESSAGE_INSERT_CARD:
            messageReceived = @"MESSAGE_INSERT_CARD";
            break;
        case MESSAGE_REMOVE_CARD:
            messageReceived = @"MESSAGE_REMOVE_CARD";
            break;
        case MESSAGE_BAD_ICC:
            messageReceived = @"MESSAGE_BAD_ICC";
            break;
        case MESSAGE_TRANSACTION_CANCELLED:
            messageReceived = @"MESSAGE_TRANSACTION_CANCELLED";
            break;
        case MESSAGE_FALLBACK_FAILED:
            messageReceived = @"MESSAGE_FALLBACK_FAILED";
            break;
        case MESSAGE_USE_CHIP_READER:
            messageReceived = @"MESSAGE_USE_CHIP_READER";
            break;
        case MESSAGE_PROCESSING:
            messageReceived = @"MESSAGE_PROCESSING";
            break;
        case MESSAGE_READY:
            messageReceived = @"MESSAGE_READY";
            break;
        case MESSAGE_USE_MSR:
            messageReceived = @"MESSAGE_USE_MSR";
            break;
        case MESSAGE_NOT_ACCEPTED:
            messageReceived = @"MESSAGE_NOT_ACCEPTED";
            break;
        default:
            break;
    }
    #endif
    return messageReceived;
}

- (void)emvTransactionData:(IDTEMVData *)emvData errorCode:(int)error performReversal:(BOOL)reversal
{
    NSMutableDictionary *dictToLog = [NSMutableDictionary new];
    dictToLog[@"unencryptedTags"] = [self dictionaryFromDataDictionary:emvData.unencryptedTags];
    dictToLog[@"encryptedTags"] = [self dictionaryFromDataDictionary:emvData.encryptedTags];
    NSString *ksnDescr = [emvData.KSN description];
    dictToLog[@"ksn"] = ksnDescr ? ksnDescr : @"";
//    [[Mixpanel sharedInstance] track:@"UNIPAY_EMV_TRANSACTION_DATA" properties:dictToLog];
//    [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_EMV_TRANSACTION_DATA" message:@"UNIPAY_EMV_TRANSACTION_DATA" kvp:dictToLog];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_DATA: unencryptedTags = %@",emvData.unencryptedTags.description]];
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_DATA: encryptedTags = %@",emvData.encryptedTags.description]];
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_DATA: ksn = %@", ksnDescr ? ksnDescr : @""]];
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_DATA: maskedTags = %@",emvData.maskedTags.description]];
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_EMV_TRANSACTION_DATA: Result code = %2X, Error Code %4X",emvData.resultCode,error]];
        
        NSDictionary *paramsToLog = @{@"Dictionary": emvData.unencryptedTags.description ? emvData.unencryptedTags.description : @"",
                                      @"Result code" : [NSString stringWithFormat:@"%2X", emvData.resultCode],
                                      @"Error Code" : [NSString stringWithFormat:@"%4X", error]};
//        [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_EMV_TRANSACTION_DATA" message:@"UNIPAY_EMV_TRANSACTION_DATA" kvp:paramsToLog];
    }
    
    NSString *errorStr = [NSString stringWithFormat:@"%4X", error];
    if ([errorStr integerValue] == 100){
        if ([self.delegate respondsToSelector:@selector(unipaySwipeDataReceived:)]){
            NSData *dataCardName = emvData.unencryptedTags[@"5F20"];
            NSString *cardName = [[NSString alloc] initWithData:dataCardName encoding:NSUTF8StringEncoding];
            NSDictionary *dataDict = [self parseDataFromIDTEMVDataToDictionary:emvData];
            NSDictionary *response = @{@"CardName" : cardName, @"Data" : dataDict};
            [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"DATA DICT: %@", dataDict]];
            [self.delegate unipaySwipeDataReceived:response];
            
//            [Ouralabs log:OULogLevelWarn tag:@"DATA DICT" message:@"DATA DICT" kvp:@{@"Data" : dataDict}];
        }
    }
    
    /*
    Byte *b = malloc(1);
    NSData*CID = [emvData.unencryptedTags objectForKey:@"9F27"];
    [CID getBytes:b];
    if (b[0] == 0x01) {
        NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"ERROR: SERVICE NOT ALLOWED"}];
        [self.delegate uniMagSwipeError:error];
        free(b);
        return;
    }
     if ((emvData.resultCode == 2) || (b[0] == 0x88) || (b[0] == 0x8B) || (b[0] == 0x8A)) {
         NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"ERROR: TRANSACTION TERMINATED"}];
         [self.delegate uniMagSwipeError:error];
         free(b);
         return;
     } else{

     }
     free(b); */
}

- (NSDictionary *)dictionaryFromDataDictionary:(NSDictionary *)dataDictionary
{
    NSMutableDictionary *dictResult = [NSMutableDictionary new];
    for (NSString *key in [dataDictionary allKeys]){
        id objectKey = dataDictionary[key];
        if ([objectKey isKindOfClass:[NSData class]]){
            dictResult[key] = [objectKey description];
        } else {
            dictResult[key] = objectKey;
        }
    }
    return dictResult;
}

#pragma mark - UniPay delegate

- (void)plugStatusChange:(BOOL)deviceInserted
{
//    [Ouralabs log:OULogLevelWarn tag:@"plugStatusChange" message:@"plugStatusChange" kvp:@{@"deviceInserted" : @(deviceInserted)}];
    if (deviceInserted){
        if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
            [self.delegate unipayDidReceiveMessage:@"device attached"];
        }
//        [Ouralabs log:OULogLevelWarn tag:@"Start connection task.." message:@"Start connection task.."];
        if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
            [self.delegate unipayDidReceiveMessage:@"Start connection task.."];
        }
        [[IDT_UniPay sharedController] device_connectToAudioReader];
    } else {
        if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
            [self.delegate unipayDidReceiveMessage:@"device removed"];
        }
    }
}

- (void)deviceConnected
{
//    [Ouralabs log:OULogLevelWarn tag:@"deviceConnected" message:@"deviceConnected"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"deviceConnected"];
    }
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidStatusChange:)]){
        [self.delegate uniMagDidStatusChange:YES];
    }
}

- (void)deviceDisconnected
{
//    [Ouralabs log:OULogLevelWarn tag:@"deviceDisconnected" message:@"deviceDisconnected"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"deviceDisconnected"];
    }
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidStatusChange:)]){
        [self.delegate uniMagDidStatusChange:NO];
    }
}

- (void)dataInOutMonitor:(NSData *)data incoming:(BOOL)isIncoming
{
    /*
    if (!isIncoming){
        if (self.delegate && [self.delegate respondsToSelector:@selector(unimagSwipeStart)]){
            [self.delegate unimagSwipeStart];
        }
    } */
}

-(void) eventFunctionICC: (Byte) nICC_Attached{
    #if !(TARGET_IPHONE_SIMULATOR)
//    [Ouralabs log:OULogLevelWarn tag:@"eventFunctionICC" message:@"eventFunctionICC" kvp:@{@"nICC_Attached" : @(nICC_Attached)}];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UniPay_EventFunctionICC Return Status Code %2X ", nICC_Attached]];
    }
    NSLog(@"UniMagHelper: UniPay_EventFunctionICC Return Status Code %2X ",  nICC_Attached);
    #endif   
}

- (void)swipeMSRData:(IDTMSRData *)cardData
{
    NSString *text = [[NSString alloc] initWithData:cardData.cardData encoding:NSASCIIStringEncoding];
    if (!([text isEqualToString:@"Detecting"] || [text isEqualToString:@"Finished Recording"] || [text isEqualToString:@"Reader Not Detected."])){
        
        /*
        NSDictionary *response = [self parseDataFromSwiper:cardData.cardData];
        NSMutableDictionary *responseMutable = [NSMutableDictionary dictionaryWithDictionary:response];
        responseMutable[@"result"] = [self stringResponseFromIDTMSRData:cardData]; */
        
//        [[Mixpanel sharedInstance] track:@"UNIPAY_MSR_TRANSACTION_DATA" properties:responseMutable];
        /*
        [Ouralabs log:OULogLevelWarn tag:@"UNIPAY_MSR_TRANSACTION_DATA" message:@"UNIPAY_MSR_TRANSACTION_DATA" kvp:@{@"cardData" : cardData.cardData ? cardData.cardData : @"",
                                                                                                                     @"KSN" : cardData.KSN ? cardData.KSN : @"",
                                                                                                                     @"Track1" : cardData.track1 ? cardData.track1 : @"",
                                                                                                                     @"Track2" : cardData.track2 ? cardData.track2 : @""}]; */
        
        if (cardData.cardData.length > 20){
            NSDictionary *result = [self parseDataFromSwiper:cardData.cardData];
            if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
                [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"UNIPAY_MSR_TRANSACTION_DATA: cardData - %@", result]];
            }
            
            [self.delegate uniMagSwipeDataReceived:result];
        } else {
            NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"Please swipe card properly\nInvalid swipe"}];
            [self.delegate uniMagSwipeError:error];
        }
    }
}

#pragma mark - NSNotifications UniMag

- (void)attachmentNotification:(NSNotification *)notification
{
    #if !(TARGET_IPHONE_SIMULATOR)
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidAttach:)]){
        [self.delegate uniMagDidAttach:YES];
    }
    if ([self.serialNumber isEqualToString:@""]){
        [self.uniReader sendCommandGetSerialNumber];
    }
    #endif
}

- (void)detachmentNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidAttach:)]){
        [self.delegate uniMagDidAttach:NO];
    }
}

- (void)connectedNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidStatusChange:)]){
        [self.delegate uniMagDidStatusChange:YES];
    }
}

- (void)disconnectNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagDidStatusChange:)]){
        [self.delegate uniMagDidStatusChange:NO];
    }
}

- (void)dataProcessingNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(unimagSwipeStart)]){
        [self.delegate unimagSwipeStart];
    }
}

- (void)invalidSwipeNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagSwipeError:)]){
        NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"Please swipe card properly\nInvalid swipe"}];
//        [Ouralabs log:OULogLevelWarn tag:@"UNIMAG_TRANSACTION_DATA" message:@"UNIMAG_TRANSACTION_DATA" kvp:@{@"Error" : @"Please swipe card properly\nInvalid swipe"}];
//        [[Mixpanel sharedInstance] track:@"UNIMAG_TRANSACTION_DATA" properties:@{@"Error" : @"Please swipe card properly\nInvalid swipe"}];
//        [Ouralabs log:OULogLevelWarn tag:@"UNIMAG_TRANSACTION_DATA" message:@"UNIMAG_TRANSACTION_DATA" kvp:@{@"Error" : @"Please swipe card properly\nInvalid swipe"}];
        [self.delegate uniMagSwipeError:error];
    }
}

- (void)dataReceivedNotification:(NSNotification *)notification
{
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagSwipeDataReceived:)] && [self.delegate respondsToSelector:@selector(uniMagSwipeError:)]){
        NSData *data = [notification object];
        NSString *text = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
        if (!([text isEqualToString:@"Detecting"] || [text isEqualToString:@"Finished Recording"] || [text isEqualToString:@"Reader Not Detected."])){

            NSString *fullString = [self replaceDataDescriptionArrows:[data description]];
//            [Ouralabs log:OULogLevelWarn tag:@"UNIMAG_TRANSACTION_DATA_FULL" message:@"UNIMAG_TRANSACTION_DATA_FULL" kvp:@{@"Data" : fullString}];
            NSDictionary *response = [self parseDataFromSwiper:data];
            
//            [[Mixpanel sharedInstance] track:@"UNIMAG_TRANSACTION_DATA" properties:response];
//            [Ouralabs log:OULogLevelWarn tag:@"UNIMAG_TRANSACTION_DATA" message:@"UNIMAG_TRANSACTION_DATA" kvp:response];
//            [Ouralabs log:OULogLevelWarn tag:@"UNIMAG_TRANSACTION_DATA" message:@"UNIMAG_TRANSACTION_DATA" kvp:response];
            
            if (data.length > 20){
                if (response[@"result"]){
//                    TFLog(@"Data received");
                    [self.delegate uniMagSwipeDataReceived:response];
                } else {
//                    TFLog(@"Please swipe card propertly\nMasked tracks not received");
                    NSString *track1 = response[@"Track1"];
                    if (track1 && track1.length > 0){
                        NSError *error = [NSError errorWithDomain:@"OrderPort" code:1 userInfo:@{NSLocalizedDescriptionKey : @"Please swipe card properly\nMasked tracks not received"}];
                        [self.delegate uniMagSwipeError:error];
                        if ([self.delegate respondsToSelector:@selector(uniMagGiftSwiperReceived:)]){
                            [self.delegate uniMagGiftSwiperReceived:response];
                        }
                    } else {
                        NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"Please swipe card properly\nMasked tracks not received"}];
                        [self.delegate uniMagSwipeError:error];
                    }
                }
            } else {
//                TFLog(@"Please swipe card propertly\nInvalid swipe");
                NSError *error = [NSError errorWithDomain:@"OrderPort" code:0 userInfo:@{NSLocalizedDescriptionKey : @"Please swipe card properly\nInvalid swipe"}];
                [self.delegate uniMagSwipeError:error];
            }
        }
        /*
        if (!([text isEqualToString:@"Detecting"] || [text isEqualToString:@"Finished Recording"])){
            if (text.length > 0){
                [self.delegate uniMagSwipeDataReceived:@{@"response" : text}];
            } else {
                [self.delegate uniMagSwipeError];
            }
        }*/

    }
}

- (void)commandReceivedNotification:(NSNotification *)notif
{
    NSData *data = [notif object];
    NSString *text = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    _serialNumber = text;
    if (self.delegate && [self.delegate respondsToSelector:@selector(uniMagCmdReceived:)]){
        [self.delegate uniMagCmdReceived:text];
    }
    
}

#pragma mark - Helper Methods

- (NSString *)stringResponseFromIDTMSRData:(IDTMSRData *)data
{
    NSString *strTrack1 = [self replaceDataDescriptionArrows:[data.encTrack1 description]];
    NSString *strTrack2 = [self replaceDataDescriptionArrows:[data.encTrack2 description]];
    NSString *strKSN = [self replaceDataDescriptionArrows:[data.KSN description]];
    NSString *strMaskedTrack1 = [self replaceDataDescriptionArrows:[data.track1 description]];
    NSString *strMaskedTrack2 = [self replaceDataDescriptionArrows:[data.track2 description]];
    NSString *strMaskedTracks = [NSString stringWithFormat:@"%@;%@", strMaskedTrack1, strMaskedTrack2];
    NSString *response = [NSString stringWithFormat:@"%@||%@|%@||||||%@|||", strMaskedTracks, strTrack1, strTrack2, strKSN];
    return response;
}

- (NSString *)replaceDataDescriptionArrows:(NSString *)fullDescription
{
    NSString *result = [fullDescription stringByReplacingOccurrencesOfString:@"<" withString:@""];
    result = [result stringByReplacingOccurrencesOfString:@">" withString:@""];
    result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
    return result;
}

- (NSString *)parseDataFromIDTEMVDataToTLV:(IDTEMVData *)data
{
    NSDictionary *dictToParse = data.unencryptedTags;
    NSMutableString *result = [NSMutableString string];
    for (NSString *key in dictToParse.allKeys){
        [result appendString:key];
        NSData *data = dictToParse[key];
        NSString *strLenght = [NSString stringWithFormat:@"%02X", data.length];
        NSString *dataStr = [self replaceDataDescriptionArrows:data.description];
        [result appendString:strLenght];
        [result appendString:dataStr];
    }
    return result;
}

- (NSDictionary *)parseDataFromIDTEMVDataToDictionary:(IDTEMVData *)data
{
    NSDictionary *dictToParse = data.unencryptedTags;
    NSMutableDictionary *result = [[NSMutableDictionary alloc] init];
    for (NSString *key in dictToParse.allKeys){
        NSData *data = dictToParse[key];
        NSString *dataStr = [self replaceDataDescriptionArrows:data.description];
        result[key] = dataStr;
    }
    return result;
}

- (NSDictionary *)parseDataFromSwiper:(NSData *)data
{
    NSString *stringFromSwiper = [data description];
    stringFromSwiper = [self replaceDataDescriptionArrows:stringFromSwiper];
    
    
    NSString *dataLengthLowByte = [stringFromSwiper substringWithRange:NSMakeRange(2, 2)];
    NSString *dataLengthHighByte = [stringFromSwiper substringWithRange:NSMakeRange(4, 2)];
    
    NSLog(@"UniMagHelper: dataLengthLowByte: %@\n dataLengthHighByte: %@\n", dataLengthLowByte, dataLengthHighByte);
    
    NSString *track1UnencryptedLength = [stringFromSwiper substringWithRange:NSMakeRange(10, 2)];
    NSString *track2UnencryptedLength = [stringFromSwiper substringWithRange:NSMakeRange(12, 2)];
    
    NSLog(@"UniMagHelper: track1UnencryptedLength: %@\n track2UnencryptedLength: %@\n", track1UnencryptedLength, track2UnencryptedLength);
    
    NSString *fieldByte1 = [stringFromSwiper substringWithRange:NSMakeRange(16, 2)];
    NSString *fieldByte2 = [stringFromSwiper substringWithRange:NSMakeRange(18, 2)];
    
    NSLog(@"UniMagHelper: fieldByte1: %@\n fieldByte2: %@\n", fieldByte1, fieldByte2);
    
    NSInteger lenghtTrack1 = [self hexToInt:track1UnencryptedLength];
    lenghtTrack1 = lenghtTrack1 % 8 != 0 ? lenghtTrack1 + 8 - (lenghtTrack1 % 8) : lenghtTrack1;
    
    NSInteger lenghtTrack2 = [self hexToInt:track2UnencryptedLength];
    lenghtTrack2 = lenghtTrack2 % 8 != 0 ? lenghtTrack2 + 8 - (lenghtTrack2 % 8) : lenghtTrack2;
    
    NSString *openText = [self hexComponentsToString:stringFromSwiper];
    
    NSRange rangeStartMaskedTracks = [openText rangeOfString:@"%*"];
    NSRange rangeEndMaskedTracks = [openText rangeOfString:@"?*" options:NSBackwardsSearch];
    
    if (rangeStartMaskedTracks.location == NSNotFound || rangeEndMaskedTracks.location == NSNotFound){
        NSString *track1UnencryptedLength = [stringFromSwiper substringWithRange:NSMakeRange(10, 2)];
        NSInteger lenghtTrack1 = [self hexToInt:track1UnencryptedLength];
        NSString *track1Hex = [stringFromSwiper substringWithRange:NSMakeRange(20, lenghtTrack1 * 2)];
        NSString *track1 = [self hexComponentsToString:track1Hex];
        return @{@"Track1" : track1.length > 0 ? track1 : @""};
    }
    
    NSString *stringFromSwiperToCheck = [self stringToHex:openText];
    NSLog(@"UniMagHelper: stringFromSwiperToCheck: %@\n", stringFromSwiperToCheck);
    
    NSString *maskedTracksOpen = [openText substringWithRange:NSMakeRange(rangeStartMaskedTracks.location, rangeEndMaskedTracks.location - rangeStartMaskedTracks.location + rangeEndMaskedTracks.length)];
    NSLog(@"UniMagHelper: maskedTracksOpen: %@\n", maskedTracksOpen);
    
    NSString *maskedTracksHex = [self stringToHex:maskedTracksOpen];
    NSLog(@"UniMagHelper: maskedTracksHex: %@\n", maskedTracksHex);
    
    NSRange rangeMaskedTracks = [stringFromSwiper rangeOfString:maskedTracksHex];
    
    NSString *track1 = [stringFromSwiper substringWithRange:NSMakeRange(rangeMaskedTracks.location + rangeMaskedTracks.length, lenghtTrack1 * 2)];
    
    NSString *track2 = [stringFromSwiper substringWithRange:NSMakeRange(rangeMaskedTracks.location + rangeMaskedTracks.length + lenghtTrack1 * 2, lenghtTrack2 * 2)];
    
    NSInteger ksnStart = rangeMaskedTracks.location + rangeMaskedTracks.length + lenghtTrack1 * 2 + lenghtTrack2 * 2 + 20;
    NSString *ksn = [stringFromSwiper substringWithRange:NSMakeRange(ksnStart, 20)];
    
    // getting result
    NSRange rangeBeforeMT = NSMakeRange(0, rangeMaskedTracks.location);
    NSRange rangeAfterMT = NSMakeRange(rangeMaskedTracks.location + rangeMaskedTracks.length, stringFromSwiper.length - rangeMaskedTracks.location - rangeMaskedTracks.length);
    NSString *strBeforeMT = [stringFromSwiper substringWithRange:rangeBeforeMT];
    NSString *strAfterMT = [stringFromSwiper substringWithRange:rangeAfterMT];
    
    NSLog(@"UniMagHelper: strBeforeMT: %@", strBeforeMT);
    NSLog(@"UniMagHelper: strAfterMT: %@", strAfterMT);
    
    NSString *responseRes = [NSString stringWithFormat:@"%@%@%@", strBeforeMT, maskedTracksOpen, strAfterMT];
    
    NSDictionary *result = @{@"MaskedTracks" : maskedTracksOpen,
                             @"Track1" : [track1 uppercaseString],
                             @"Track2" : [track2 uppercaseString],
                             @"KSN" : [ksn uppercaseString],
                             @"result": responseRes};
    return result;
}

- (NSString *)hexComponentsToString:(NSString *)hexComponents
{
    NSMutableString * newString = [[NSMutableString alloc] init];
    int i = 0;
    while (i < [hexComponents length])
    {
        NSString * hexChar = [hexComponents substringWithRange: NSMakeRange(i, 2)];
        int value = 0;
        sscanf([hexChar cStringUsingEncoding:NSASCIIStringEncoding], "%x", &value);
        [newString appendFormat:@"%c", (char)value];
        i+=2;
    }
    return newString;
}

- (NSString *)stringToHex:(NSString *)string
{
    NSString *result = [NSString stringWithFormat:@"%@",
                              [NSData dataWithBytes:[string cStringUsingEncoding:NSUTF8StringEncoding]
                                             length:strlen([string cStringUsingEncoding:NSUTF8StringEncoding])]];
    
    for(NSString *toRemove in [NSArray arrayWithObjects:@"<", @">", @" ", nil])
        result = [result stringByReplacingOccurrencesOfString:toRemove withString:@""];
    return result;
}

- (NSInteger)hexToInt:(NSString *)hexString
{
    unsigned result = 0;
    NSScanner *scanner = [NSScanner scannerWithString:hexString];
    [scanner scanHexInt:&result];
    return result;
}

- (NSData *)hexToData:(NSString*)str { //Example - Pass string that contains characters "30313233", and it will return a data object containing ascii characters "0123"
    if ([str length] == 0) {
        return nil;
    }
    
    unsigned stringIndex=0, resultIndex=0, max=(int)[str length];
    NSMutableData* result = [NSMutableData dataWithLength:(max + 1)/2];
    unsigned char* bytes = [result mutableBytes];
    
    unsigned num_nibbles = 0;
    unsigned char byte_value = 0;
    
    for (stringIndex = 0; stringIndex < max; stringIndex++) {
        unsigned int val = [self char2hex:[str characterAtIndex:stringIndex]];
        
        num_nibbles++;
        byte_value = byte_value * 16 + (unsigned char)val;
        if (! (num_nibbles % 2)) {
            bytes[resultIndex++] = byte_value;
            byte_value = 0;
        }
    }
    
    
    //final nibble
    if (num_nibbles % 2) {
        bytes[resultIndex++] = byte_value;
    }
    
    [result setLength:resultIndex];
    
    return result;
}


-(unsigned int) char2hex:(char)c{
    
    switch (c) {
        case '0' ... '9': return c - '0';
        case 'a' ... 'f': return c - 'a' + 10;
        case 'A' ... 'F': return c - 'A' + 10;
        default: return -1;
    }
}

- (void)emvPowerOnICC
{
//    [Ouralabs log:OULogLevelWarn tag:@"f_IccPowerON" message:@"f_IccPowerON"];
    NSData* response;
    RETURN_CODE rt = [[IDT_UniPay sharedController] icc_powerOnICC:&response];
    
    if(RETURN_CODE_DO_SUCCESS == rt)
    {
        NSString *str = [NSString stringWithFormat:@"ICC Powerd On, data: %@\n%@", response.description, [[NSString alloc] initWithData:response encoding:NSASCIIStringEncoding] ];
        if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
            [self.delegate unipayDidReceiveMessage:str];
        }
        
        int lengthp = response.length;
        Byte * Response = (Byte*)[response bytes];
        if (0x90 == Response[lengthp-2])
        {
            if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
                [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"ICC Powerd On - Success."]];
            }
        }
        else
        {
            if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
                [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"ICC Powerd On - Failed."]];
            }
        }
    }else
    {
        [self displayUpRet2: @"ICC Powerd On" returnValue:rt];
    }
}

- (void)emvPowerOffICC
{
//    [Ouralabs log:OULogLevelWarn tag:@"f_IccPowerOFF" message:@"f_IccPowerOFF"];
    
    NSString* error;
    RETURN_CODE rt = [[IDT_UniPay sharedController] icc_powerOffICC:&error];
    if(RETURN_CODE_DO_SUCCESS == rt)
    {
        if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
            [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"ICC PowerOff: OK."]];
        }
    }else
    {
        [self displayUpRet2: @"ICC PowerOff" returnValue:rt];
    }
}

-(void) displayUpRet2:(NSString*) operation returnValue: (RETURN_CODE)rt
{
    
    NSString * str = [NSString stringWithFormat:
                      @"%@ ERROR: ID-\"%i\", message: %@.",
                      operation, rt, [[IDT_UniPay sharedController] device_getResponseCodeString:rt]];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:str];
    }
    
}

- (void)emvStart
{
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"startEMV"];
    }
//    [Ouralabs log:OULogLevelWarn tag:@"startEMV" message:@"startEMV"];
    NSMutableDictionary* tags;
    [[IDTechEMV sharedController] startEMVTransaction:2.95 timeout:30 transactionType:0 additionalTags:tags];
}

- (void)emvComplete
{
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"completeEMV"];
    }
    
//    [Ouralabs log:OULogLevelWarn tag:@"completeEMV" message:@"completeEMV"];
    
    NSMutableDictionary* dict = [NSMutableDictionary new];
    
    [dict setObject:[self hexToData:@"3030"] forKey:@"8a"];
    
    NSDictionary* myDict;
    myDict = nil;
    [[IDTechEMV sharedController]  completeOnlineEMVTransaction:EMV_COMPLETION_RESULT_ACCEPTED resultCode:@"00" issuerAuthenticationData:nil issuerScripts:nil];
}

- (void)emvCancel
{
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"cancelEMV"];
    }
//    [Ouralabs log:OULogLevelWarn tag:@"emvCancel" message:@"emvCancel"];
    [[IDTechEMV sharedController] cancelTransaction];
}

- (void)removeAID
{
//    [Ouralabs log:OULogLevelWarn tag:@"removeAIDFile" message:@"removeAIDFile"];
    
    [[IDTechEMV sharedController] removeApplicationData:@"a0000000031010"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"Removed AID a0000000031010"];
    }
}


-(void)createTerminalFile:(id)sender{
    
    
//    [Ouralabs log:OULogLevelWarn tag:@"createTerminalFile" message:@"createTerminalFile"];
    
    //UniPay
    IDTech_TerminalData t;
    t.terminalCountryCode[0] = 0x08;
    t.terminalCountryCode[1] = 0x40;
    t.merchantCategoryCode[0] = 0x12;
    t.merchantCategoryCode[1] = 0x34;
    t.merchantID[0] = '0';
    t.merchantID[1] = '0';
    t.merchantID[2] = '9';
    t.merchantID[3] = '8';
    t.merchantID[4] = '4';
    t.merchantID[5] = '2';
    t.merchantID[6] = '0';
    t.merchantID[7] = '1';
    t.merchantID[8] = '8';
    t.merchantID[9] = '5';
    t.merchantID[10] = '2';
    t.merchantID[11] = '5';
    t.merchantID[12] = '8';
    t.merchantID[13] = '8';
    t.merchantID[14] = '7';
    
    t.terminalID[0] = 0x30;
    t.terminalID[1] = 0x35;
    t.terminalID[2] = 0x35;
    t.terminalID[3] = 0x36;
    t.terminalID[4] = 0x36;
    t.terminalID[5] = 0x33;
    t.terminalID[6] = 0x30;
    t.terminalID[7] = 0x34;
    
    
    t.defaultTACDefault[0] = 0x00;
    t.defaultTACDefault[1] = 0x00;
    t.defaultTACDefault[2] = 0x80;
    t.defaultTACDefault[3] = 0x00;
    t.defaultTACDefault[4] = 0x00;
    t.defaultTACDenial[0] = 0x00;
    t.defaultTACDenial[1] = 0x00;
    t.defaultTACDenial[2] = 0x00;
    t.defaultTACDenial[3] = 0x00;
    t.defaultTACDenial[4] = 0x00;
    t.defaultTACOnline[0] = 0x00;
    t.defaultTACOnline[1] = 0x00;
    t.defaultTACOnline[2] = 0x00;
    t.defaultTACOnline[3] = 0x00;
    t.defaultTACOnline[4] = 0x00;
    
    t.useDefaultTACDefault = 0x00;
    t.useDefaultTACDenial = 0x00;
    t.useDefaultTACOnline = 0x00;
    
    t.terminalLocation[0] = '5';
    t.terminalLocation[1] = '8';
    t.terminalLocation[2] = '0';
    t.terminalLocation[3] = '6';
    t.terminalLocation[4] = 'A';
    t.terminalLocation[5] = ' ';
    t.terminalLocation[6] = '1';
    t.terminalLocation[7] = '1';
    t.terminalLocation[8] = '9';
    t.terminalLocation[9] = 't';
    t.terminalLocation[10] = 'h';
    t.terminalLocation[11] = ' ';
    t.terminalLocation[12] = 'A';
    t.terminalLocation[13] = 'v';
    t.terminalLocation[14] = 'e';
    t.terminalLocation[15] = ' ';
    t.terminalLocation[16] = 'S';
    t.terminalLocation[17] = 'E';
    t.terminalLocation[18] = ' ';
    t.terminalLocation[19] = '#';
    t.terminalLocation[20] = '1';
    t.terminalLocation[21] = '0';
    t.terminalLocation[22] = '2';
    t.terminalLocation[23] = ',';
    t.terminalLocation[24] = ' ';
    t.terminalLocation[25] = 'B';
    t.terminalLocation[26] = 'e';
    t.terminalLocation[27] = 'l';
    t.terminalLocation[28] = 'l';
    t.terminalLocation[29] = 'e';
    t.terminalLocation[30] = 'v';
    t.terminalLocation[31] = 'u';
    t.terminalLocation[32] = 'e';
    t.terminalLocation[33] = ',';
    t.terminalLocation[34] = ' ';
    t.terminalLocation[35] = 'W';
    
    
    [[IDTechEMV sharedController] setTerminalData:t];
    
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:@"Terminal Settings Created"];
    }
}

-(void) getTerminalFile:(id)sender{
    
//    [Ouralabs log:OULogLevelWarn tag:@"getTerminalFile" message:@"getTerminalFile"];
    
    IDTech_TerminalData t = [[IDTechEMV sharedController] retrieveTerminalData];
    NSString* terminalCC = [NSString stringWithFormat:@"%4x",t.terminalCountryCode[0]*0x100+t.terminalCountryCode[1]];
    terminalCC = [terminalCC stringByReplacingOccurrencesOfString:@" " withString:@"0"];
    
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"Terminal Country Code = %@", terminalCC]];
    }
    
    
    NSString* merchantCC = [NSString stringWithFormat:@"%4x",t.merchantCategoryCode[0]*0x100+t.merchantCategoryCode[1]];
    merchantCC = [merchantCC stringByReplacingOccurrencesOfString:@" " withString:@"0"];
    
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"Merchant Country Code = %@", merchantCC]];
    }
    
    NSString* merchantID = [[NSString alloc] initWithBytes:&t.merchantID length:15 encoding:NSASCIIStringEncoding];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"Merchant ID = %@", merchantID]];
    }
    
    NSString* terminalID = [[NSString alloc] initWithBytes:&t.terminalID length:8 encoding:NSASCIIStringEncoding];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"Terminal ID = %@", terminalID]];
    }
    
    
    NSString* TACDefault = @"";
    
    TACDefault = [NSString stringWithFormat:@"%2x%2x%2x%2x%2x",t.defaultTACDefault[0],t.defaultTACDefault[1],t.defaultTACDefault[2],t.defaultTACDefault[3],t.defaultTACDefault[4]];
    TACDefault = [TACDefault stringByReplacingOccurrencesOfString:@" " withString:@"0"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"TAC Default = %@", TACDefault]];
    }
    
    NSString* TACDenial = @"";
    TACDenial = [NSString stringWithFormat:@"%2x%2x%2x%2x%2x",t.defaultTACDenial[0],t.defaultTACDenial[1],t.defaultTACDenial[2],t.defaultTACDenial[3],t.defaultTACDenial[4]];
    TACDenial = [TACDenial stringByReplacingOccurrencesOfString:@" " withString:@"0"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"TAC Denial = %@", TACDenial]];
    }
    
    NSString* TACOnline = @"";
    TACOnline = [NSString stringWithFormat:@"%2x%2x%2x%2x%2x",t.defaultTACOnline[0],t.defaultTACOnline[1],t.defaultTACOnline[2],t.defaultTACOnline[3],t.defaultTACOnline[4]];
    TACOnline = [TACOnline stringByReplacingOccurrencesOfString:@" " withString:@"0"];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"TAC TACOnline = %@", TACOnline]];
    }
    
    NSString* terminalLocation = [[NSString alloc] initWithBytes:&t.terminalLocation length:36 encoding:NSASCIIStringEncoding];
    if ([self.delegate respondsToSelector:@selector(unipayDidReceiveMessage:)]){
        [self.delegate unipayDidReceiveMessage:[NSString stringWithFormat:@"Terminal Location = %@", terminalLocation]];
    }
}

@end
