#!/bin/sh

# Update the firmware from the last build into Google Storage.  This makes
# it accessable to download from https://starman.array.org/${track}
#
# For gsutil to work, be sure to run `gcloud init` and have access to the
# Array project's starman Storage Bucket

track=$1

if [ "x$track" == "x" ]; then
    echo "Usage:"
    echo
    echo "\t$0 <track>"
    echo
    echo "\t<track> must be stable, testing, or unstable"
    echo
    exit
fi

echo Upload build/starman.bin to gs://starman-firmware/${track}.bin ...
gsutil cp build/starman.bin gs://starman-firmware/${track}.bin
