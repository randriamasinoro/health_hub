#!/usr/bin/env python3
# Example frame:
# AccelX:-4,0;AccelY:-11,0;AccelZ:8,4;GyroX:0,16;GyroY:0,8;GyroZ:0,3;RoomTempC:22,3;PulseIR:717;PulseBPM:0,22;FeverTempC:21,75

import argparse
from time import sleep
import serial
from tb_device_mqtt import TBDeviceMqttClient

DEFAULT_DEVICE = "/dev/ttyUSB0"
DEFAULT_BAUD = 115200
DEFAULT_TIMEOUT = 2.0   # seconds overall read timeout

DEFAULT_URL = "campusco.univ-ubs.fr"
DEFAULT_PORT = 64083

# Replace these with your real telemetry sender, e.g. tb.sendTelemetryData(...)
def send_float(name: str, value: float):
    print(f"FLOAT  -> {name}: {value}")
    client.send_telemetry({name: value})

def send_int(name: str, value: int):
    print(f"INT    -> {name}: {value}")
    client.send_telemetry({name: value})

def send_unknown(name: str, value: int):
    print(f"UNKNOWN KEY: {name} -> {value}")
    client.send_telemetry({name: value})

KEY_MAP_FLOAT = {
    "AccelX": "Acceleration X",
    "AccelY": "Acceleration Y",
    "AccelZ": "Acceleration Z",
    "GyroX":  "Gyro X",
    "GyroY":  "Gyro Y",
    "GyroZ":  "Gyro Z",
    "RoomTempC": "Room Temperature",
    "FeverTempC": "Fever Temperature",
    "PulseBPM": "Pulse BPM",
}
KEY_MAP_INT = {
    "PulseIR": "Pulse IR",
}

def parse_frame(frame: str):
    # Normalize comma decimal separator to dot and strip whitespace/newline
    frame = frame.replace(",", ".").strip()
    if not frame:
        return

    # Split tokens by ';' then each token by the first ':' into key/value
    for token in frame.split(';'):
        token = token.strip()
        if not token:
            continue
        if ':' not in token:
            continue
        key, val = token.split(':', 1)
        key = key.strip()
        val = val.strip()
        if not val:
            continue

        # Dispatch conversions similarly to your C example
        if key in KEY_MAP_FLOAT:
            try:
                send_float(KEY_MAP_FLOAT[key], float(val))
            except ValueError:
                print(f"PARSE ERROR (float): {key} -> '{val}'")
        elif key in KEY_MAP_INT:
            try:
                # allow integer values that might be represented with a decimal (e.g. "717.0")
                send_int(KEY_MAP_INT[key], int(float(val)))
            except ValueError:
                print(f"PARSE ERROR (int): {key} -> '{val}'")
        elif key:
            send_unknown(key, val)

def read_one_line_from_serial(port, baud, timeout):
    try:
        with serial.Serial(port, baud, timeout=timeout) as ser:
            raw = ser.readline()
            if not raw:
                print("No data (timeout).")
                return None
            return raw.decode(errors='replace').strip()
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None

if __name__ == "__main__":
    p = argparse.ArgumentParser(description=
                                """
                                Read and parse serial frame. Then send them to Thinsgboard.
                                Trace Format
                                """
                                )
    
    # Thingsboard connection parameters
    p.add_argument("--url", "-u", default=DEFAULT_URL,
                   help=f"Remote URL for telemetry (default: {DEFAULT_URL})")
    p.add_argument("--port", "-p", type=int, default=DEFAULT_PORT,
                   help=f"Remote port for telemetry (default: {DEFAULT_PORT})")
    p.add_argument("--token", "-t", required=True,
                   help="Authentication token (required)")
    # Serial connection parameters
    p.add_argument("--device", "-d", default=DEFAULT_DEVICE,
                   help=f"Serial port device (default: {DEFAULT_DEVICE})")
    p.add_argument("--baud", "-b", type=int, default=DEFAULT_BAUD,
                   help=f"Baud rate (default: {DEFAULT_BAUD})")
    p.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT,
                   help=f"Overall read timeout in seconds (default: {DEFAULT_TIMEOUT})")
    
    args = p.parse_args()

    client = TBDeviceMqttClient(args.url, port=args.port, username=args.token)
    client.connect()
    while client.is_connected() is False:
        print("Wait client connection")
        sleep(0.5)

    while(1):
        line = read_one_line_from_serial(args.device, args.baud, args.timeout)
        if line:
            print("Received:", line)
            parse_frame(line)
    
    # Will be never reached
    client.disconnect()