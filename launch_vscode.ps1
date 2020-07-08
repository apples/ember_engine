$PKG = "$PSScriptRoot\.pkg"
$EMSDK = "$PSScriptRoot\.pkg\emsdk"

& "$PKG\msys64\msys2_shell.cmd" -mingw64 -c "source '$EMSDK\emsdk_env.sh'; '/c/Program Files/Microsoft VS Code/Code.exe' '$PSScriptRoot' &"
