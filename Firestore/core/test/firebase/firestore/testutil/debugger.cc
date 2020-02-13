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

#include "Firestore/core/test/firebase/firestore/testutil/debugger.h"

#include <cassert>
#include <csignal>

#if __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#elif __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>
#include <cstring>

#elif _WIN32
#include <debugapi.h>
#endif  // __APPLE__

// Non-Clang compilers don't support __has_builtin
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

namespace firebase {
namespace firestore {
namespace testutil {

/**
 * Returns true if the current process is running under a debugger.
 */
bool IsRunningUnderDebugger() {
#if __APPLE__
  // Initialize the flags so that, if sysctl fails for some bizarre reason, we
  // get a predictable result.
  struct kinfo_proc info {};
  info.kp_proc.p_flag = 0;

  // Initialize mib, which tells sysctl the info we want, in this case we're
  // looking for information about a specific process ID.
  constexpr unsigned int kMibSize = 4;
  int mib[kMibSize];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;
  mib[3] = getpid();

  // Call sysctl.
  size_t size = sizeof(info);
  int rc = sysctl(mib, kMibSize, &info, &size, NULL, 0);
  assert(rc == 0);

  // We're being debugged if the P_TRACED flag is set.
  return (info.kp_proc.p_flag & P_TRACED) != 0;

#elif __linux__
  char buf[4096];

  int status_fd = ::open("/proc/self/status", O_RDONLY);
  if (status_fd < 0) return false;

  ssize_t num_read = ::read(status_fd, buf, sizeof(buf) - 1);
  if (num_read <= 0) return false;

  buf[num_read] = '\0';
  const char* buf_end = buf + num_read;

  constexpr char tracer_pid_start[] = "TracerPid:";
  const char* pid_str = ::strstr(buf, tracer_pid_start);
  if (!pid_str) return false;

  pid_str += strlen(tracer_pid_start);
  while (pid_str < buf_end && ::isspace(*pid_str)) {
    ++pid_str;
  }

  if (pid_str >= buf_end) return false;
  return ::isdigit(*pid_str) != 0 && *pid_str != '0';

#elif _WIN32
  return ::IsDebuggerPresent() != 0;

#else

  return false;
#endif  // __APPLE__
}

/**
 * Forces the program to stop under the debugger.
 */
void DebugBreak() {
#if _MSC_VER
  // MSVC compiler intrinsic
  __debugbreak();

#elif __has_builtin(__builtin_debugtrap)
  // Clang compiler intrinsic. Prefer this to __builtin_trap because it is not
  // marked _Noreturn.
  __builtin_debugtrap();

#elif __has_builtin(__builtin_trap)
  // Clang compiler intrinsic. Prefer this to __builtin_trap because it is not
  // marked _Noreturn.
  __builtin_trap();

#elif defined(__i386__) || defined(__x86_64__)
  // x86 platforms (including Linux on x86)
  __asm__ volatile("int $03");

#elif defined(SIGTRAP)
  // Otherwise just raise SIGTRAP.
  raise(SIGTRAP);

#else
  abort();
#endif
}

/**
 * A ThrowHandler that forces assertion failures to stop under the debugger.
 */
void DebugThrowHandler(util::ExceptionType type,
                       const char* file,
                       const char* func,
                       int line,
                       const std::string& message) {
  if (IsRunningUnderDebugger()) {
    DebugBreak();
  } else {
    DefaultThrowHandler(type, file, func, line, message);
  }
}

}  // namespace testutil
}  // namespace firestore
}  // namespace firebase
