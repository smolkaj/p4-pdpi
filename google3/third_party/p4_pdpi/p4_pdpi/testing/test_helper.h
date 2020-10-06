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

#ifndef P4_PDPI_TESTING_TEST_HELPER_H_
#define P4_PDPI_TESTING_TEST_HELPER_H_

#include <functional>
#include <iostream>
#include <string>

#include "net/proto2/public/message.h"
#include "net/proto2/util/public/message_differencer.h"
#include "third_party/absl/status/status.h"
#include "third_party/absl/status/statusor.h"
#include "third_party/absl/strings/str_cat.h"
#include "third_party/p4_pdpi/gutil/proto.h"
#include "third_party/p4_pdpi/gutil/status.h"
#include "third_party/p4_pdpi/gutil/testing.h"
#include "third_party/p4_pdpi/p4_pdpi/ir.h"
#include "third_party/p4_pdpi/p4_pdpi/ir.proto.h"

constexpr char kBanner[] =
    "=========================================================================";

enum InputValidity {
  INPUT_IS_VALID = 1,
  INPUT_IS_INVALID = 2,
};

std::string TestHeader(const std::string& test_name) {
  return absl::StrCat(kBanner, "\n", test_name, "\n", kBanner);
}

void Fail(const std::string& message) {
  // Splitting "DO NOT SUB***" into two pieces to avoid triggering our tools.
  std::cerr << "TEST FAILED (DO NOT " << "SUBMIT)" << std::endl;
  std::cerr << "FAILURE REASON: " << message << std::endl;
}

// Runs a generic test starting from an invalid PI and checks that it cannot be
// translated to IR. If you want to test valid PI, instead write a generic PD
// test.
template <typename IR, typename PI>
void RunGenericPiTest(
    const pdpi::IrP4Info& info, const std::string& test_name, const PI& pi,
    const std::function<absl::StatusOr<IR>(const pdpi::IrP4Info&, const PI&)>&
        pi_to_ir) {
  // Input and header.
  std::cout << TestHeader(test_name) << std::endl << std::endl;
  std::cout << "--- PI (Input):" << std::endl;
  std::cout << pi.DebugString() << std::endl;

  // Convert PI to IR.
  const auto& status_or_ir = pi_to_ir(info, pi);
  if (!status_or_ir.ok()) {
    std::cout << "--- PI is invalid/unsupported:" << std::endl;
    std::cout << status_or_ir.status() << std::endl;
  } else {
    Fail(
        "Expected PI to be invalid (valid PI should instead be tested using "
        "RunGenericPdTest.");
  }
  std::cout << std::endl;
}

// Runs a generic test starting from an invalid IR and checks that it cannot be
// translated to PI. If you want to test valid IR, instead write a
// generic PD test. IR to PD is not tested, since only PI to PD is supported for
// converting to PD.
template <typename IR, typename PI>
void RunGenericIrTest(
    const pdpi::IrP4Info& info, const std::string& test_name, const IR& ir,
    const std::function<absl::StatusOr<PI>(const pdpi::IrP4Info&, const IR&)>&
        ir_to_pi) {
  // Input and header.
  std::cout << TestHeader(test_name) << std::endl << std::endl;
  std::cout << "--- IR (Input):" << std::endl;
  std::cout << ir.DebugString() << std::endl;

  // Convert IR to PI.
  const auto& status_or_pi = ir_to_pi(info, ir);
  if (!status_or_pi.ok()) {
    std::cout << "--- IR (converting to PI) is invalid/unsupported:"
              << std::endl;
    std::cout << status_or_pi.status() << std::endl;
  } else {
    Fail(
        "Expected IR to be invalid (valid IR should instead be tested using "
        "RunGenericPdTest.");
  }
  std::cout << std::endl;
}

// Runs a generic test starting from a valid PD entity. If pd is valid, then it
// is translated: PD -> IR -> PI -> IR2 -> PD2 and IR == IR2 and PD == PD2 are
// checked.
template <typename PD, typename IR, typename PI>
void RunGenericPdTest(
    const pdpi::IrP4Info& info, const std::string& test_name, const PD& pd,
    const std::function<absl::StatusOr<IR>(const pdpi::IrP4Info&, const PD&)>&
        pd_to_ir,
    const std::function<absl::Status(const pdpi::IrP4Info&, const IR&,
                                     proto2::Message*)>& ir_to_pd,
    const std::function<absl::StatusOr<PI>(const pdpi::IrP4Info&, const IR&)>&
        ir_to_pi,
    const std::function<absl::StatusOr<IR>(const pdpi::IrP4Info&, const PI&)>&
        pi_to_ir,
    const InputValidity& validity) {
  // Input and header.
  std::cout << TestHeader(test_name) << std::endl << std::endl;
  std::cout << "--- PD (Input):" << std::endl;
  std::cout << pd.DebugString() << std::endl;

  // Set-up message differencer.
  proto2::util::MessageDifferencer diff;
  diff.set_report_moves(false);
  diff.set_repeated_field_comparison(
      proto2::util::MessageDifferencer::RepeatedFieldComparison::AS_SET);
  std::string explanation;
  diff.ReportDifferencesToString(&explanation);

  // Convert PD to IR.
  const auto& status_or_ir = pd_to_ir(info, pd);
  if (!status_or_ir.ok()) {
    if (validity == INPUT_IS_VALID) {
      Fail(
          "Translation from PD to IR failed even though input was marked as "
          "valid.");
      std::cout << status_or_ir.status().message() << std::endl;
      return;
    }
    std::cout << "--- PD is invalid/unsupported:" << std::endl;
    std::cout << status_or_ir.status() << std::endl;
    return;
  }
  const auto& ir = status_or_ir.value();
  // Convert IR to PI.
  const auto& status_or_pi = ir_to_pi(info, ir);

  if (validity == INPUT_IS_INVALID) {
    if (status_or_pi.status().ok()) {
      Fail(
          "Input was marked as invalid, but translation from PD to IR and IR "
          "to PI both succeeded.");
      return;
    } else {
      std::cout << "--- PD is invalid/unsupported (detected when translating "
                   "IR to PI):"
                << std::endl;
      std::cout << status_or_pi.status().message() << std::endl;
      return;
    }
  }

  // At this point, validity == INPUT_IS_VALID
  std::cout << "--- IR:" << std::endl;
  std::cout << ir.DebugString() << std::endl;
  if (!status_or_pi.status().ok()) {
    Fail(
        "Translation from IR to PI failed even though input was marked as "
        "valid.");
    std::cout << status_or_pi.status().message() << std::endl;
    return;
  }
  const auto& pi = status_or_pi.value();
  std::cout << "--- PI:" << std::endl;
  std::cout << pi.DebugString() << std::endl;

  // Convert PI back to IR.
  const auto& status_or_ir2 = pi_to_ir(info, pi);
  if (!status_or_ir2.status().ok()) {
    Fail("Reverse translation from PI to IR failed.");
    std::cout << status_or_ir2.status().message() << std::endl;
    return;
  }
  if (!diff.Compare(ir, status_or_ir2.value())) {
    Fail("Reverse translation from PI to IR resulted in a different IR.");
    std::cout << "Differences: " << explanation << std::endl;
    std::cout << "IR (after reverse translation):" << std::endl
              << status_or_ir2.value().DebugString() << std::endl;
    return;
  }

  // Convert IR back to PD.
  PD pd2;
  const auto& status_pd2 = ir_to_pd(info, ir, &pd2);
  if (!status_pd2.ok()) {
    Fail("Reverse translation from IR to PD failed.");
    std::cout << status_pd2.message() << std::endl;
    return;
  }
  if (!diff.Compare(pd, pd2)) {
    Fail("Reverse translation from IR to PD resulted in a different PD.");
    std::cout << "Differences: " << explanation << std::endl;
    std::cout << "PD (after reverse translation):" << std::endl
              << pd2.DebugString() << std::endl;
    return;
  }
  std::cout << std::endl;
}

#endif  // P4_PDPI_TESTING_TEST_HELPER_H_
