// Copyright 2019 Google
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#import "FIRCLSApplicationIdentifierModel.h"

#import <XCTest/XCTest.h>

NS_ASSUME_NONNULL_BEGIN

@interface FABMockApplicationIdentifierModel : FIRCLSApplicationIdentifierModel

// Allows us to set this value in FIRCLSSettingsTests
@property(nonatomic, copy) NSString* buildInstanceID;

// Allows us to set this value in FIRCLSReportManagerTests
@property(nonatomic, copy) NSString* bundleID;

@end

NS_ASSUME_NONNULL_END
