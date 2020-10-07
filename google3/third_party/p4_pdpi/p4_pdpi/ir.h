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

#ifndef P4_PDPI_IR_H
#define P4_PDPI_IR_H
// P4 intermediate representation definitions for use in conversion to and from
// Program-Independent to either Program-Dependent or App-DB formats

#include <string>

#include "third_party/absl/status/statusor.h"
#include "net/grpc/public/include/grpcpp/grpcpp.h"
#include "third_party/p4_pdpi/gutil/status.h"
#include "third_party/p4_pdpi/p4_pdpi/ir.pb.h"
#include "third_party/p4_pdpi/p4_pdpi/ir.proto.h"
#include "third_party/p4lang_p4runtime/proto/p4/config/v1/p4info.proto.h"
#include "third_party/p4lang_p4runtime/proto/p4/v1/p4runtime.pb.h"
#include "third_party/p4lang_p4runtime/proto/p4/v1/p4runtime.proto.h"

namespace pdpi {

// Creates IrP4Info and validates that the p4_info has no errors.
absl::StatusOr<IrP4Info> CreateIrP4Info(const p4::config::v1::P4Info& p4_info);

// Converts a PI table entry to the IR table entry.
absl::StatusOr<IrTableEntry> PiTableEntryToIr(const IrP4Info& info,
                                              const p4::v1::TableEntry& pi);

// Converts an IR table entry to the PI representation.
absl::StatusOr<p4::v1::TableEntry> IrTableEntryToPi(const IrP4Info& info,
                                                    const IrTableEntry& ir);

// Returns the IR of a packet-io packet.
absl::StatusOr<IrPacketIn> PiPacketInToIr(const IrP4Info& info,
                                          const p4::v1::PacketIn& packet);
absl::StatusOr<IrPacketOut> PiPacketOutToIr(const IrP4Info& info,
                                            const p4::v1::PacketOut& packet);

// Returns the PI of a packet-io packet.
absl::StatusOr<p4::v1::PacketIn> IrPacketInToPi(const IrP4Info& info,
                                                const IrPacketIn& packet);
absl::StatusOr<p4::v1::PacketOut> IrPacketOutToPi(const IrP4Info& info,
                                                  const IrPacketOut& packet);

// RPC-level conversion functions for read request
absl::StatusOr<IrReadRequest> PiReadRequestToIr(
    const IrP4Info& info, const p4::v1::ReadRequest& read_request);
absl::StatusOr<p4::v1::ReadRequest> IrReadRequestToPi(
    const IrP4Info& info, const IrReadRequest& read_request);

// RPC-level conversion functions for read response
absl::StatusOr<IrReadResponse> PiReadResponseToIr(
    const IrP4Info& info, const p4::v1::ReadResponse& read_response);
absl::StatusOr<p4::v1::ReadResponse> IrReadResponseToPi(
    const IrP4Info& info, const IrReadResponse& read_response);

// RPC-level conversion functions for update
absl::StatusOr<IrUpdate> PiUpdateToIr(const IrP4Info& info,
                                      const p4::v1::Update& update);
absl::StatusOr<p4::v1::Update> IrUpdateToPi(const IrP4Info& info,
                                            const IrUpdate& update);
// RPC-level conversion functions for update
absl::StatusOr<IrUpdate> PiUpdateToIr(const IrP4Info& info,
                                      const p4::v1::Update& update);
absl::StatusOr<p4::v1::Update> IrUpdateToPi(const IrP4Info& info,
                                            const IrUpdate& update);

// RPC-level conversion functions for write request
absl::StatusOr<IrWriteRequest> PiWriteRequestToIr(
    const IrP4Info& info, const p4::v1::WriteRequest& write_request);
absl::StatusOr<p4::v1::WriteRequest> IrWriteRequestToPi(
    const IrP4Info& info, const IrWriteRequest& write_request);

// Formats a grpc status about write request into a readible string.
std::string WriteRequestGrpcStatusToString(const grpc::Status& grpc_status);

// RPC-level conversion functions for write response
absl::StatusOr<IrWriteRpcStatus> GrpcStatusToIrWriteRpcStatus(
    const grpc::Status& status, int number_of_updates_in_write_request);
absl::StatusOr<grpc::Status> IrWriteRpcStatusToGrpcStatus(
    const IrWriteRpcStatus& ir_write_status);

}  // namespace pdpi
#endif  // P4_PDPI_IR_H
