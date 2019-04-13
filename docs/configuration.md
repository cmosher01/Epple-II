[home](index.md)
[download](https://github.com/cmosher01/Epple-II/releases/latest)
[screenshots](screenshots.md)
configuration
[reference](usermanual.md)

---

The Epple \]\[ Emulator is most useful when properly configured.
After downloading and installing the Epple \]\[ Program, some System ROM (demo or real), and
optionally the peripheral card ROMs you want to use, you will need to configure the program.
Configuring is concerned primarily with telling the program what peripheral cards to use,
and where the ROM files are located (and what memory addresses to load them at).

## epple2.conf

The default configuration file for
the Epple \]\[ emulator is:

    /usr/local/etc/epple2/epple2.conf

For Windows, the file will be:

    C:\<Path to>\epple2\epple2.conf

You can specify a different configuration file for the emulator to use by
specifying its name as the argument when running the epple2 program.

The format of the configuration file is just lines of [commands](usermanual.md).
Comments begin with a hash `#` character, and continue to the end of the line.

## Firmware

Firmware, commonly called ROM images or ROMs, will need to be loaded
into the ROM areas of the emulated Apple in order for it to boot.
Once the firmware image files are installed on your computer, you can use the `import`
command in the configuration file to load the image files into the emulator.

The original system software on the
Apple \]\[ machines is copyright by Apple, and is proprietary. If you own an actual
Apple \]\[ or Apple \]\[ plus machine, you can copy the firmware
from it (`$D000`-`$FFFF`) (into a file on your PC) and use it. Alternatively, you can
download the firmware images from the [Apple II Library](https://mosher.mine.nu/apple2/).

If you do not want to download the proprietary firmware, you can still use the emulator (albeit
in a rudimentary manner) with the free (GPLv3) *System ROM (Demo)* package (included with EPPLE \]\[).
The source code is also available; you can
assemble it using the [xa Assembler](http://www.floodgap.com/retrotech/xa/).
The Demo System ROM only provides commands to dump or set memory bytes,
or run a program in memory. It is not compatible with any Apple software, nor can it read from disks.
It's just a free simple demo system to make the emulator do something reasonable.

## Peripheral Cards

Another primary concern for configuring the Epple \]\[ is the insertion of
peripheral cards into the Apple's slots. The most useful card is probably the
Disk \]\[ Controller card; you will need to insert one of these cards if you want
to be able to load floppy disk images into the emulated Apple. The various
types of cards available are described in the cards section.
You can use the `slot` command in the configuration file to insert
them into the Apple's slots.

If you are going to use the Disk \]\[ Controller peripheral card (which you almost certainly will, if
you want to read any floppy disk images), you will need to have to original ROMs from Apple.
No demo of the Disk ROMs are provided here; you will need to
use the original ROMs, which can be obtained from an original machine (by copying
it from `$C600`-`$C6FF` into a file on the PC), or by downloading it
from the [Apple II Library](http://mosher.mine.nu/apple2/).
(And, of course, you will need the real Apple System ROM, as noted in the previous section,
not the Demo System ROM.)

If you are using any of the clock, the standard output, or the
standard input cards, you will need to load their firmware as well.

## Sample Configuration Files

There are a number of configuration files provided with the distribution for
your convenience. They provide various common configurations of machines.
You can use one of these files directly, by specifying its path as the argument
to the program, or as a starting point for you own configuration file. These sample
files are in the same location as the default `epple2.conf` file, or
on [github](https://github.com/cmosher01/Epple-II/tree/master/conf).

The Epple \]\[ Emulator can actually be run without a configuration file at all.
In this case, you will be able to power on the Apple and see the low-resolution
graphics display showing random RAM data. The system will not run, because
there is no firmware loaded, but the emulator will otherwise be functional.
