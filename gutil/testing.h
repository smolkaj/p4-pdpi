// Copyright 2020 Google LLC
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

#ifndef GUTIL_TESTING_H
#define GUTIL_TESTING_H

#include <string_view>

#include "absl/status/status.h"
#include "glog/logging.h"
#include "gutil/proto.h"
#include "gutil/status.h"

// Crash if `status` is not okay. Only use in tests.
#define CHECK_OK(val) CHECK_EQ(::absl::OkStatus(), (val))

namespace gutil {

// Parses a protobuf from a string, and crashes if parsing failed. Only use in
// tests.
template <typename T>
T ParseProtoOrDie(std::string_view proto_string) {
  T message;
  CHECK_OK(ReadProtoFromString(proto_string, &message));
  return message;
}

// Parses a protobuf from a file, and crashes if parsing failed. Only use in
// tests.
template <typename T>
T ParseProtoFileOrDie(std::string_view proto_file) {
  T message;
  CHECK_OK(ReadProtoFromFile(proto_file, &message));
  return message;
}

}  // namespace gutil

#endif  // GUTIL_TESTING_H
