#!/bin/sh
set -e
exec grub2-mkconfig -o "$1"
