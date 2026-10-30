# /// script
# requires-python = ">=3.10"
# dependencies = ["bleak"]
# ///
"""Test bench: talk to the rave cape over Bluetooth from a computer.

Usage:
  uv run tools/cape.py scan
  uv run tools/cape.py send rainbow_flow
  uv run tools/cape.py send pulse:833ab4,fd1d1d,fcb045 brightness:80
"""
import asyncio
import sys

from bleak import BleakClient, BleakScanner

DEVICE_NAME = "RaveCapeController"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"


async def find_cape():
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10)
    if device is None:
        sys.exit(f"Could not find {DEVICE_NAME}. Is the cape powered on and in range?")
    return device


async def scan():
    device = await find_cape()
    print(f"Found {device.name} at {device.address}")


async def send(commands):
    device = await find_cape()
    async with BleakClient(device) as client:
        greeting = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f"Connected. Cape says: {greeting.decode(errors='replace')}")
        for command in commands:
            await client.write_gatt_char(CHARACTERISTIC_UUID, command.encode(), response=True)
            print(f"Sent: {command}")


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("scan", "send"):
        sys.exit(__doc__)
    if sys.argv[1] == "scan":
        asyncio.run(scan())
    else:
        if len(sys.argv) < 3:
            sys.exit("send needs at least one command")
        asyncio.run(send(sys.argv[2:]))


if __name__ == "__main__":
    main()
