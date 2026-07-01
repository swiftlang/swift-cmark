#
# This source file is part of the Swift.org open source project
#
# Copyright (c) 2026 Apple Inc. and the Swift project authors
# Licensed under Apache License v2.0 with Runtime Library Exception
#
# See https://swift.org/LICENSE.txt for license information
# See https://swift.org/CONTRIBUTORS.txt for Swift project authors
#

# winget isn't easily made available in containers, so use chocolatey
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

choco install -y cmake.portable --no-progress
choco install -y make --no-progress

Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv

(Get-Command make).Path
(Get-Command cmake).Path

# Let swiftc find the path to link.exe in the CMake smoke test
$env:Path += ";$(Split-Path -Path "$(& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" "-latest" -products Microsoft.VisualStudio.Product.BuildTools -find VC\Tools\MSVC\*\bin\HostX64\x64\link.exe)" -Parent)"
