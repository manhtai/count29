# Count to 9 (count29)

A program allows to set HIGH/LOW for D pins in [ESP8266][2] [NodeMCU][3] board, from D0
to D9 (hence **count to 9**).

Using this we can make a switch through WiFi to turn on/off 9 diffirent
devices (actually 7 because D0 and D4 are for 2 built-in LEDs).

## Driver

You need [CP210x][0] driver to connect the board to your computer.

## Build & Upload

We use [PlatformIO][1] for library management & building.

```
# Install Platformio
pip install platformio

# Set enviroments
export PLATFORMIO_BUILD_FLAGS='-DMQTTSERVER=\"test.mosquitto.org\"'

# Build project
pio run

# Upload firmware
pio run --target upload
```

## What's missing?

You need a [MQTT server][5], and an [app][4] to pub/sub messages to it.


[0]: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
[1]: https://platformio.org/
[2]: https://en.wikipedia.org/wiki/ESP8266
[3]: https://github.com/nodemcu/nodemcu-devkit-v1.0
[4]: https://github.com/manhtai/count29-app
[5]: https://github.com/manhtai/mqtt-server
