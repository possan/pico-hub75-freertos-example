# PicoW Hub75 FreeRTOS example

## Set up environment

Clone repo with submodules to also get FreeRTOS kernel, and clone the [pico-sdk](https://github.com/raspberrypi/pico-sdk/) somewhere else.

```
mkdir build
cd build
cmake -DPICO_BOARD=pico_w -DPICO_SDK_PATH=your-path-to-pico-sdk -DWIFI_SSID=your-ssid -DWIFI_PASSWORD=your-password ..
```

## Build and upload to pico

```
make -j9
picotool load sign.uf2 -f
```

Then connect your favorite serial port monitor to the usb serial port

Sometimes you might have to reboot the pico

```
picotool reboot
```
