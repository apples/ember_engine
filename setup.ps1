$MSYS2_URL = 'https://github.com/msys2/msys2-installer/releases/download/2020-06-29/msys2-base-x86_64-20200629.sfx.exe'
$PKG = "$PSScriptRoot\.pkg"
$TMP = "$PSScriptRoot\.tmp"
$EMSDK = "$PSScriptRoot\.pkg\emsdk"
$DOTVSCODE = "$PSScriptRoot\.vscode"
$C_CPP_PROPERTIES = "$DOTVSCODE\c_cpp_properties.json"
$CMAKE_KITS = "$DOTVSCODE\cmake-kits.json"

if (!(Test-Path "$PKG")) {
    New-Item -Path "$PSScriptRoot" -Name ".tmp" -ItemType "directory"
    New-Item -Path "$PSScriptRoot" -Name ".pkg" -ItemType "directory"
    wget "$MSYS2_URL" -OutFile "$TMP\msys2.exe"
    & "$TMP\msys2.exe" -y "-o$PKG\"

    & "$PKG\msys64\usr\bin\bash" -lc ' '
    & "$PKG\msys64\usr\bin\bash" -lc 'pacman -Syuu'
    & "$PKG\msys64\usr\bin\bash" -lc 'pacman -Syuu'
    & "$PKG\msys64\usr\bin\bash" -lc 'pacman -S --noconfirm git unzip mingw-w64-x86_64-python mingw-w64-x86_64-curl mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-gcc'
}

if (!(Test-Path "$EMSDK")) {
    $OLD_PATH = "$Env:PATH"
    $Env:PATH = "$PKG\msys64\mingw64\bin;$PKG\msys64\usr\bin;$Env:PATH"
    git clone https://github.com/emscripten-core/emsdk "$EMSDK"
    & "$EMSDK\emsdk.bat" install latest
    & "$EMSDK\emsdk.bat" activate latest
    $Env:PATH = "$OLD_PATH"
}

if (!(Test-Path "$DOTVSCODE")) {
    New-Item -Path "$PSScriptRoot" -Name ".vscode" -ItemType "directory"
}

if (!(Test-Path "$C_CPP_PROPERTIES")) {
    cp "$PSScriptRoot\setup_c_cpp_properties.json" $C_CPP_PROPERTIES
}

if (!(Test-Path "$CMAKE_KITS")) {
    cp "$PSScriptRoot\setup_cmake-kits.json" $CMAKE_KITS
}

echo Done