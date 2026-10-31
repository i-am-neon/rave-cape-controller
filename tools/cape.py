# /// script
# requires-python = ">=3.10"
# dependencies = ["bleak"]
# ///
"""Test bench: talk to the rave cape over Bluetooth from a computer.

Usage:
  uv run tools/cape.py scan
  uv run tools/cape.py send rainbow_flow
  uv run tools/cape.py send pulse:833ab4,fd1d1d,fcb045 brightness:80
  uv run tools/cape.py tour [seconds]   play every spooky animation in turn
  uv run tools/cape.py draw-demo        draw a zig-zag with the live-draw protocol
"""
import asyncio
import sys

from bleak import BleakClient, BleakScanner

DEVICE_NAME = "RaveCapeController"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
SPOOKY = ["bonfire", "ghostfire", "witchfire", "inferno", "heartbeat", "ghost", "drip", "ecto",
          "cauldron", "banshee", "glitch", "reaper", "rain", "pumpkin", "candycorn", "aurora"]


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


async def tour(seconds):
    device = await find_cape()
    async with BleakClient(device) as client:
        for i, name in enumerate(SPOOKY, 1):
            await client.write_gatt_char(CHARACTERISTIC_UUID, name.encode(), response=True)
            print(f"[{i}/{len(SPOOKY)}] {name}", flush=True)
            await asyncio.sleep(seconds)


async def draw_demo():
    device = await find_cape()
    async with BleakClient(device) as client:
        await client.write_gatt_char(CHARACTERISTIC_UUID, b"draw", response=True)
        colors = ["ff5000", "8000ff"]
        for i in range(90):  # ~3 seconds at 30 messages per second, like the app
            y = i / 89
            x = abs((i % 30) - 15) / 15
            point = f"p:0,{round(x * 1000)},{round(y * 1000)},{colors[(i // 30) % 2]}"
            await client.write_gatt_char(CHARACTERISTIC_UUID, point.encode(), response=False)
            await asyncio.sleep(1 / 30)
        await client.write_gatt_char(CHARACTERISTIC_UUID, b"u:0", response=True)
        print("Drew a zig-zag from shoulders to hem")


def main():
    if len(sys.argv) < 2 or sys.argv[1] not in ("scan", "send", "tour", "draw-demo"):
        sys.exit(__doc__)
    if sys.argv[1] == "scan":
        asyncio.run(scan())
    elif sys.argv[1] == "draw-demo":
        asyncio.run(draw_demo())
    elif sys.argv[1] == "tour":
        asyncio.run(tour(float(sys.argv[2]) if len(sys.argv) > 2 else 7))
    else:
        if len(sys.argv) < 3:
            sys.exit("send needs at least one command")
        asyncio.run(send(sys.argv[2:]))


if __name__ == "__main__":
    main()
