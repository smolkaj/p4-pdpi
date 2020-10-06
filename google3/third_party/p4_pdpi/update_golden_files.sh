#!/bin/bash
# Updates all golden files.

blaze run //third_party/p4_pdpi/p4_pdpi/testing:info_test -- --update
blaze run //third_party/p4_pdpi/p4_pdpi/testing:table_entry_test -- --update
blaze run //third_party/p4_pdpi/p4_pdpi/testing:packet_io_test -- --update
blaze run //third_party/p4_pdpi/p4_pdpi/testing:rpc_test -- --update
blaze run //third_party/p4_pdpi/p4_pdpi/testing:main_pd_test -- --update
