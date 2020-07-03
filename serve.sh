#!/usr/bin/env bash
set -e

make -f Makefile.nginx

start http://localhost:4242/release

echo "Starting Nginx..."
cd .nginx
./nginx.exe
