This software works in tandem with the Super Game Boy Hardware Corrupter, an arduino based device that induces user controlled glitches in Game Boy Games via a Super Game Boy SNES Cartridge.

The arduino is hooked up to 2 different shift registers, and these shift registers are then attached to the first 7 lines of the VRAM address and VRAM data lines of the Super Game Boy via NPN transistors. When a pin on the shift register reads high, it pulls the corresponding VRAM line low, therefore changing the values being read or written to in VRAM. This creates visual glitches on screen.

The arduino is also hooked up to the first lines of the WRAM address, WRAM data, GB Cartridge address and GB Cartridge data lines. These lines proved to be very sensitive, and corrupting them for too many CPU cycles caused very consistent crashes. This required that these lines be hooked directly up to the arduino and special code to be written to control when these lines are pulled low. With these improvements, it's possible to pull these lines to low for one CPU cycle for very granular corruption control. 

Outside of the control of the arduino is a variable CPU clock that is implemented with a LTC1799 variable oscillator chip. This allows the user to speed up and slow down the clock speed of the Super Game Boy.

Inspiration for this device was taken from Ircluzar's NES Corrupter. Videos of Ircluzar's device and my device in action are linked below.

NES Hardware Corrupter
https://www.youtube.com/watch?v=CzwsCyM2bUo

Super Game Boy Hardware Corrupter
https://vid.me/e5Mp
