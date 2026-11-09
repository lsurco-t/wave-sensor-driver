# wave-sensor-driver

A small Linux character device driver, written as an out-of-tree kernel module, that exposes simulated sensor data to user space. Includes a C++ program that reads from the device.

I built it to understand how data moves safely between kernel space and user space.

## What it does

- Registers a character device and creates `/dev/wave_sensor` automatically (via `class_create` / `device_create`).
- Implements `read` in `file_operations`, returning a simulated sensor reading: `Vibration data: 42 Hz`.
- Copies data to user space with `copy_to_user`, copying at most the requested length and tracking the file offset, so partial reads work and small user buffers can't overflow. Returns 0 (EOF) when all data has been read.
- Cleans up the device, class and major number when the module is unloaded.

## Requirements

- Linux with kernel headers for the running kernel (`sudo apt install linux-headers-$(uname -r) build-essential` on Debian/Ubuntu)
- Kernel 6.4 or newer (uses the single-argument `class_create`)
- `g++` for the user-space reader

Tested on Debian 13. Run it in a virtual machine: a bug in kernel code can crash the whole system.

## Build and load

```bash
make
sudo insmod wave_sensor.ko
ls -l /dev/wave_sensor
```

## Test

```bash
sudo cat /dev/wave_sensor                       # full reading, then EOF
sudo head -c 5 /dev/wave_sensor                 # "Vibra": only the requested bytes
sudo dd if=/dev/wave_sensor bs=5 2>/dev/null    # full reading, 5 bytes per read call
sudo dmesg | tail                               # driver log messages
```

## User-space reader

```bash
g++ -std=c++17 -Wall -Wextra reader.cpp -o reader
sudo ./reader
```

The device node is owned by root, so reading requires `sudo`.

## Unload

```bash
sudo rmmod wave_sensor
make clean
```

## Possible improvements

- Use `alloc_chrdev_region` + `cdev_add` instead of the older `register_chrdev`
- Add a udev rule so non-root users can read the device
- Replace the static string with data from real hardware (e.g. over I2C or SPI)

## License

GPL-2.0 (matches `MODULE_LICENSE("GPL")`).