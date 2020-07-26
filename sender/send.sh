#!/bin/bash
cd ${BASH_SOURCE[0]%/*}
objcopy --input-target=binary --output-target=ihex main.bin main.hex
php send.php