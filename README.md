# Fake omapi service written in cpp

Aim to run omapi service without java environment.  
To provide a method to decrypt in recovery like TWRP

Only implemented part of original secure element app.

Tested devices:
- Xiaomi SM8750 devices
- Xiaomi SM8650 devices
- Redmi SM8650 devices
- Realme GT7 Pro

How to use:
-----------
- Clone this repo to aosp source tree, eg. $TWRP_ROOT/vendor/twrp
- lunch target device
- run `make android.se.omapi.fake` 
 
Then, the binary will be found in out/target/product/$DEVICE/vendor 
 
- Copy binary file, manifest file and rc file to your TWRP device tree
- Build twrp
- Enjoy
