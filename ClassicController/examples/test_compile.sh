#!/bin/bash

if [ -z "$ARDUINO" ]; then
    echo "Need to set ARDUINO envvar to your arduino binary"
    echo "You can use something like this:"
    echo "  ARDUINO=/path/to/arduino $0"
    exit 1
fi

PLATFORMS="arduino:avr:uno arduino:avr:mega:cpu=atmega2560"

for ino in `find | grep '\.ino$'`; do
    echo "--------------------------------------------------------------------------------"
    for platform in $PLATFORMS; do
        echo "$ino - $platform"
        echo ""
        $ARDUINO --verify --board $platform $ino || exit $?
    done
done

echo "--------------------------------------------------------------------------------"
echo "All examples compiled for all platforms, great work."

