//
//  NSObject+AutomagicCoding.h
//  AutomagicCoding
//
//  Created by Stepan Generalov on 31.08.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "objc/runtime.h"

#pragma mark Collection Protocols


/** Protocol that describes selectors, which object must respond to in order to
 * be detected as Ordered Collection. */
@protocol AMCArrayProtocol

- (NSUInteger)count;
- (id) objectAtIndex:(NSUInteger) index; 
- (id) initWithArray:(NSArray *) array;

@end

/** Protocol that describes selectors, which object must respond to in order to
 * be detected as Mutable Ordered Collection.
 * It simply adds new methods to AMCArrayProtocol. 
 */
@protocol AMCArrayMutableProtocol <AMCArrayProtocol>

- (void) addObject: (id) anObject;

@end

/** Protocol that describes selectors, which object must respond to in order to
 * be detected as Hash(NSDictionary-Like Key-Value) Collection. */
@protocol AMCHashProtocol

- (NSUInteger)count;
- (NSArray *) allKeys;
- (id) initWithDictionary: (NSDictionary *) aDict;

@end

/** Protocol that describes selectors, which object must respond to in order to
 * be detected as Mutable Hash(NSMutableDictionary-Like Key-Value) Collection. 
 * It simply adds new methods to AMCArrayProtocol. 
 */
@protocol AMCHashMutableProtocol <AMCHashProtocol>

- (void) setObject: (id) anObject forKey:(NSString *) aKey;

@end



typedef enum 
{
    kAMCFieldTypeScalar, //< Scalar value.
    
    kAMCFieldTypeCustomObject,                   // Your own object, that will be saved as it's dictionaryRepresentation
    kAMCFieldTypeCollectionHash,           //< NSDictionary-like objects.
    kAMCFieldTypeCollectionHashMutable,    //< NSMutableDictionary-like objects.
    kAMCFieldTypeCollectionArray,          //< NSArray-like objects.
    kAMCFieldTypeCollectionArrayMutable,   //< NSMutableArray-like objects.
    
    kAMCFieldTypeStructure, //< Struct
} AMCFieldType;

@interface NSObject (AutomagicCoding)

/** Reimplement this method in your classes and return YES if you want to enable AutomagicCoding.
 * Returns NO by default. */
+ (BOOL) AMCEnabled;

#pragma Decode/Create/Init

/** Creates autoreleased object with given dictionary representation.
 * Returns nil, if object there's no such class name or aDict is nil.
 * Doesn't catch any exceptions, that can be thrown by KVC methods.
 *
 * @param aDict Dictionary that contains @"class" NSString with name of class & all other
 * values for keys in the saved object.
 */
+ (id) objectWithDictionaryRepresentation: (NSDictionary *) aDict;

/** Inits object with key values from given dictionary.
 * Doesn't test className to be equal with self className.
 * Reimplement this method to add your custom init behavior while initing from saved state.
 */
- (id) initWithDictionaryRepresentation: (NSDictionary *) aDict;

/** Used in initWithDictionaryRepresentation: to decode value with given key 
 * from given dicctionary representation.
 * Uses AMCDecodeObject() internally to decode objects from dict.
 * Reimplement this method to customize your own object decoding.
 */
- (id) AMCDecodeFieldWithKey: (NSString *) aKey fromDictionary: (NSDictionary *) aDict;


#pragma mark Encode/Save

/** Encodes object and returns it's dictionary representation, that can be writed to PLIST. */
- (NSDictionary *) dictionaryRepresentation;

/** Used in dictionaryRepresentation to encode value with given key.
 * Uses AMCEncodeObject() internally to encode objects.
 * Reimplement this method to customize your own object encoding.
 */
- (id) AMCEncodeFieldWithKey: (NSString *) aKey;


#pragma Info for Serialization

/** Returns array of keys, that will used get dictionaryWithValuesForKeys: for
 * automagic coding.
 * By default - uses list of all available properties in the object via runtime methods.
 * You can expand it with your custom non-property ivars, by appending your keys to returned in super. */
- (NSArray *) AMCKeysForDictionaryRepresentation;

/** Returns field type for given key to save/load it in dictionaryRepresentation
 * as Scalar, CustomObject, Collection, etc...
 * Reimplement this method to add your custom ivar without properties.
 */
- (AMCFieldType) AMCFieldTypeForValueWithKey: (NSString *) aKey;

#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED

- (NSString *) className;
+ (NSString *) className;

#endif

@end

#pragma mark Encode/Decode Helper Functions

/** Returns value, prepared for setValue:forKey: based on it's fieldType 
 * Recursively uses itself for objects in collections. */
id AMCDecodeObject (id value, AMCFieldType fieldType, id collectionClass);

/** Returns object that can be added to dictionary for dictionaryRepresentation. */
id AMCEncodeObject (id value, AMCFieldType fieldType);

#pragma mark Property Info Helper Functions
/** Returns Class of given property if it is a Objective-C object.
* Otherwise returns nil.
*/
id AMCPropertyClass (objc_property_t property);

/** Returns name of struct, if given property type is struct.
 * Otherwise returns nil.
 */
NSString *AMCPropertyStructName(objc_property_t property);

#pragma mark Field Type Info Helper Functions

/** Tries to guess fieldType for given encoded object. Used in collections decoding to create objects in collections. */
AMCFieldType AMCFieldTypeForEncodedObject(id object);

/** Returns fieldType for given not yet encoded object. */
AMCFieldType AMCFieldTypeForObjectToEncode(id object);

/** Returns YES, if instances of given class respond to all required instance methods listed
 * in protocol p.
 * Otherwise returns NO;
 */
BOOL classInstancesRespondsToAllSelectorsInProtocol(id class, Protocol *p );


