#!/bin/sh
set -e
exec grub-mkconfig -o "$1"
