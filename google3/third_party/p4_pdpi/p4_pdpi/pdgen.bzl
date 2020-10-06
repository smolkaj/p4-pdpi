# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Rule for invoking the PD generator."""

def p4_pd_proto(name, src, out, package, visibility = None):
    """Generates PD proto from p4info file."""
    native.genrule(
        name = name,
        outs = [out],
        cmd = """
            $(location //third_party/p4_pdpi/p4_pdpi:pdgen)\\
                --p4info $(location {p4info})\\
                --package "{package}"\\
                > $(OUTS)
        """.format(
            p4info = ":" + src,
            package = package,
        ),
        tools = ["//third_party/p4_pdpi/p4_pdpi:pdgen"],
        srcs = [
            ":" + src,
        ],
        visibility = visibility,
    )
