/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FIRESTORE_CORE_TEST_FIREBASE_FIRESTORE_TESTUTIL_DEBUGGER_H_
#define FIRESTORE_CORE_TEST_FIREBASE_FIRESTORE_TESTUTIL_DEBUGGER_H_

#include <string>

#include "Firestore/core/src/firebase/firestore/util/exception.h"
#include "absl/base/attributes.h"

namespace firebase {
namespace firestore {
namespace testutil {

/**
 * Returns true if the current process is running under a debugger.
 */
bool IsRunningUnderDebugger();

/**
 * Forces the program to stop under the debugger.
 */
void DebugBreak();

/**
 * A ThrowHandler that forces assertion failures to stop under the debugger.
 */
void DebugThrowHandler(util::ExceptionType type,
                       const char* file,
                       const char* func,
                       int line,
                       const std::string& message);

class RestoreDefaultThrowHandler {
 public:
  explicit RestoreDefaultThrowHandler() {
    old_handler_ = util::SetThrowHandler(util::DefaultThrowHandler);
  }

  explicit RestoreDefaultThrowHandler(util::ThrowHandler new_handler) {
    old_handler_ = util::SetThrowHandler(new_handler);
  }

  ~RestoreDefaultThrowHandler() {
    util::SetThrowHandler(old_handler_);
  }

 private:
  util::ThrowHandler old_handler_;
};

}  // namespace testutil
}  // namespace firestore
}  // namespace firebase

#endif  // FIRESTORE_CORE_TEST_FIREBASE_FIRESTORE_TESTUTIL_DEBUGGER_H_
