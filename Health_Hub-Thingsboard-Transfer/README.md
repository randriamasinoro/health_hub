# Health Hub Python Thingsboard interface

Here you shall have a USB/TTL (Serial) cable and connect it to your computer.
Then check with device is used (COM in Window, /dev in Linux), create your own device on [Thingsboard](http://campusco.univ-ubs.fr:63080), get the token and use the script as below:

```sh
python3 captureFrameAndSendToThingsboard.py --help
usage: captureFrameAndSendToThingsboard.py [-h] [--url URL] [--port PORT] --token TOKEN [--device DEVICE] [--baud BAUD] [--timeout TIMEOUT]

Read and parse serial frame. Then send them to Thinsgboard. Trace Format

options:
  -h, --help            show this help message and exit
  --url URL, -u URL     Remote URL for telemetry (default: campusco.univ-ubs.fr)
  --port PORT, -p PORT  Remote port for telemetry (default: 64083)
  --token TOKEN, -t TOKEN
                        Authentication token (required)
  --device DEVICE, -d DEVICE
                        Serial port device (default: /dev/ttyUSB0)
  --baud BAUD, -b BAUD  Baud rate (default: 115200)
  --timeout TIMEOUT     Overall read timeout in seconds (default: 2.0)
```

To log in to Thingsboard, check the available resources in corresponding Moodle webpages.  

In case of uninstalled module, activate the environment, launch the command, then, **once finished** deactivate the environment:
```
source ./venv/bin/activate
python3 captureFrameAndSendToThingsboard.py -t "MY_PERSONAL_TOKEN"
deactivate
```

Or, in Windows:
```
# In PowerShell
venv\bin\Activate.ps1
python3 captureFrameAndSendToThingsboard.py -t "MY_PERSONAL_TOKEN"
deactivate
```

If the environment doesn't work properly, you can delete it and recreate it manually:
```
# In Linux
rm -rf venv
python3 -m venv venv
. venv/bin/activate
pip install pyserial tb-mqtt-client setuptools==80.9.0
```

## Known Data format

The frame data format shall be like following:
```
"AccelX:-4,0;AccelY:-11,0;AccelZ:8,4;GyroX:0,16;GyroY:0,8;GyroZ:0,3;RoomTempC:22,3;PulseIR:717;PulseBPM:0,22;FeverTempC:21,75\n"
```
And will send to Thingsboard telemetry data with following key & value regarding this example:

| Name in Frame  | Key  | Value | type |
| --- | --- | --- | --- |
| AccelX | Acceleration X | -4.0 | float |
| AccelY | Acceleration Y | -11.0 | float |
| AccelZ | Acceleration Z | 8.4 | float |
| GyroX | Gyro X | 0.16 | float |
| GyroY | Gyro Y | 0.8 | float |
| GyroZ | Gyro Z | 0.3 | float |
| RoomTempC | Room Temperature | 22.3 | float |
| PulseIR | Pulse IR | 717 | uint |
| PulseBPM | Pulse BPM | 0.22 | float |
| FeverTempC | Fever Temperature | 21.75 | float |

## Send Unknown Data

If you want to send some unknown data, like Step number, etc...

Use following structure `"<Key1>:<Data1>;<Key2>:<Data2>;<Key3>:<Data3>\n"`

## Create our own dashboard

To create your one dashboard you can have an idea with [live demo](https://thingsboard.io/use-cases/health-care/) and follow [official documentation](https://thingsboard.io/docs/).
