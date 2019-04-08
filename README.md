# epple2, The Emulated Apple ][

Copyright © 2008–2019, Christopher Alan Mosher, Shelton, Connecticut, USA, <cmosher01@gmail.com>.

[![License](https://img.shields.io/github/license/cmosher01/Epple-II.svg)](https://www.gnu.org/licenses/gpl.html)
[![Latest Release](https://img.shields.io/github/release-pre/cmosher01/Epple-II.svg)](https://github.com/cmosher01/Epple-II/releases/latest)
[![Build Status](https://travis-ci.com/cmosher01/Epple-II.svg?branch=master)](https://travis-ci.com/cmosher01/Epple-II)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-v1.4%20adopted-ff69b4.svg)](./code-of-conduct.md)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CVSSQ2BWDCKQ2)

Home page: <https://mosher.mine.nu/epple2>

`epple2` is an emulator of the **Apple ][** and **Apple ][ plus**
computers from Apple, Inc.

---

The fundamental principle of `epple2` is to accurately and faithfully
emulate the behavior of the original Apple ][ machines, including the
internal workings, as much as practical.

Features of the emulation include:
* individual cycles of the 6502 CPU
* NTSC video signal generation
* displaying the *strange orange line*
* other display color anomalies
* floating data bus
* Disk ][ Logic State Sequencer
* read copy-protected disks
* read/write half- and quarter-tracks
* configurable RAM chips
* cassette tape interface

The CPU and video timings are synchronized, and the emulator
is designed to run at the same speed as the original
machine (about 1MHz). It also emulates a television screen,
and color and monochrome monitors, for the display.

It includes emulation of the **Disk ][** controller card,
the ROM firmware card, and the RAM Language Card, as well
as a simple clock card.

The primary source for information about the internals of the
Apple ][ is *Understanding the Apple ][*, by Jim Sather.

`epple2` is written in C++, and is designed to be buildable
on Linux, Mac, or Windows (mingw).
[SDL](http://www.libsdl.org/) is the only dependency.

See files `COPYING.*` for other copyright notices.

Please note that this project is released with a Contributor
Code of Conduct. By participating in this project you agree
to abide by its terms.
