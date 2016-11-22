#!/bin/bash

if [ -z "$ARDUINO" ]; then
    echo "Need to set ARDUINO envvar to your arduino binary"
    echo "You can use something like this:"
    echo "  ARDUINO=/path/to/arduino $0"
    exit 1
fi

PLATFORMS="arduino:avr:uno arduino:avr:mega:cpu=atmega2560"

for pde in `find | grep '\.pde$'`; do
    echo "--------------------------------------------------------------------------------"
    for platform in $PLATFORMS; do
        echo "$pde - $platform"
        echo ""
        $ARDUINO --verify --board $platform $pde || exit $?
    done
done

echo "--------------------------------------------------------------------------------"
echo "All examples compiled for all platforms, great work."

