[home](index.md)
[download](https://github.com/cmosher01/Epple-II/releases/latest)
[screenshots](screenshots.md)
[configuration](configuration.md)
reference

---



### Overview

Epple \]\[ (the Emulated Apple \]\[ is a free (as in GPLv3), cross-platform
(Windows, Linux, Mac) emulator of the Apple \]\[ and Apple \]\[ plus computers
from Apple, Inc. It strives for accurate emulation of the original machines,
with a few extra features added.

---



### Commands

#### slot

The `slot` command inserts a card into a peripheral slot of the emulated Apple.

``` conf
slot <slot> <card>
```

`slot` Slot number, 0 through 7, to insert the card into.

`card` The type of card to insert into the slot (see `cards` below):

* `language`
* `firmware`
* `disk`
* `clock`
* `stdout`
* `stdin`
* `empty`

The `slot` command inserts a card into a peripheral slot. For example:

``` conf
slot 0 language
```

inserts a language card into slot zero. Use `empty` to remove a card:

``` conf
slot 0 empty
```

Note that the emulated Apple should be *powered off* before inserting or removing cards.





#### motherboard

The `motherboard` command configures the emulated Apple's motherboard RAM chips and strapping block.

``` conf
motherboard ram   {C|D|E} { 4K | 4096 | 16K | 4116 | - | (other-models) } [...up to 8]
motherboard strap {C|D|E} { 4K | 16K } <base>
```

The RAM configuration lines represent the rows of chips on the motherboard. The motherboard labels the
rows as C, D, and E. Each row has 8 chips, one per bit in a byte. The Apple \]\[ accepts 4K or 16K chips.
You use the `ram` command to insert (or remove) chips from the sockets.
You configure each row's address range by using a "strapping block" on the original Apple. In the
emulator, use the `strap` command to perform this function. You should strap 4K rows to a 4K range
of RAM. You should always assign some RAM to the zero address.

For more information about RAM configuration, see
Christopher Espinosa, [*Apple II Reference Manual*](https://archive.org/details/Apple_II_Reference_Manual_1979_Apple/page/n79)
(Cupertino, Calif.: Apple Computer, 1978), pp. 70-72.

In the `motherboard ram` configuration line `(other-models)` of chips are supported, and will produce different bit patterns at power-on time:

``` conf
MM5290
MK4116
MCM4116
```



Example of normal 48K RAM configuration:
``` conf
ram e 16K
strap e 16K 8000
ram d 16K
strap d 16K 4000
ram c 16K
strap c 16K 0000
```

Example of 4K, showing how you could specify each chip:
``` conf
ram e - - - - - - - -
ram d - - - - - - - -
ram c 4K 4K 4K 4K 4K 4K 4K 4K
strap c 4K 0000
```

Example of 4K at zero address, and 8K at HI-RES page one:
``` conf
ram e 4K
strap e 4K 3000
ram d 4K
strap d 4K 2000
ram c 4K
strap c 4K 0000
```





#### cpu

The `cpu` command chooses which CPU emulator to run with.

``` conf
cpu epple2
```

Valid values are:

`epple2` The standard, faster, albeit less accurate, high-level emulator. Works for 99.99% of known cases.
This is the default value, used when the `cpu` command is not present.

`visual6502` The emulator based on the algorithm and transistor circuitry from http://www.visual6502.org/.
WARNING: this emulator is *extremely slow*, but absolutely 100% accurate to the original MOS6502.

Note: the CPU cannot be changed in the user interface, only in the configuration file.



#### import

The `import` command imports a binary image file into the emulated Apple's ROMs.

``` conf
import slot <slot> { rom | rom7 | rombank } <base> <file-path>
import motherboard rom <base> <file-path>
```

`slot` Slot number, 0 through 7, of peripheral card to import the binary image into.

`base` Base address in hexadecimal within the given memory area at which to start loading the binary image.

`file-path` Path of the binary image to import.

The `import` command reads the binary image byte-for-byte from the given `file-path`
into an area of ROM in the emulated Apple. You can load into either the motherboard or
a card in one of the slots. For a card in a slot, you can choose either the normal ROM,
the bank-switched ROM, or the so-called *seventh ROM* area.

You also have to specify the base address within the specific memory
area at which the image file will be loaded. Note that the base address is specified as the offset
within the specific memory area, and not necessarily as the actual memory address as seen
by the Apple. So for motherboard ROM, for example, specifying a base as `2DED` will cause the
image to be loaded at offset `$2DED` in the ROM, which will be addressed by the Apple at
memory address `$FDED`, because motherboard ROM *starts* at address `$D000`, and `$D000` + `$2DED` = `$FDED`.

For peripheral cards, the ROM will be seen at locations `$Cs00`-`$CsFF`, where s is the slot
number (1 through 7). The *seventh ROM* can be seen as locations `$C800`-`$CFFF`; Jim Sather
describes this functionality in
[*Understanding the Apple II*](https://archive.org/details/understanding_the_apple_ii/page/n137),
on page 6-4, section *The Seventh ROM Chip*. The EPPLE \]\[ emulator handles this processing correctly. A card can
also have bank-switched ROM, which will show up at addresses `$D000`-`$FFFF` when switched in
(stealing that address range from motherboard ROM... see
[*Understanding the Apple II*](https://archive.org/details/understanding_the_apple_ii/page/n117),
p. 5-26 *The 16K RAM Card*).





#### load

The `load` command loads a [WOZ 2.0 format](https://applesaucefdc.com/woz/) floppy disk image into one of the emulated disk drives.

``` conf
load slot <slot> drive <drive> [ <file-path> ]
```

`slot` Slot number, 0 through 7, of Disk \]\[ controller card to load the disk image into.

`drive` Drive number, 1 or 2, of the disk drive on the controller card to load the disk image into.

`file-path` Optional path of the disk image to import. If omitted, a dialog box will be presented to select the file to load.

The `load` command will load a WOZ 2.0 image into a disk drive. Specify the slot that
contains a Disk \]\[ controller peripheral card, and specify which drive number (1 or 2).

> #### note
> The floppy disk image **MUST** be a [**WOZ 2.0 DISK IMAGE**](https://applesaucefdc.com/woz/).
> Other formats (for example, nibble, `.nib`, DOS order, `.do`, PRODOS order,
> `.po`, `.dsk`, or anything else) must first be converted to WOZ 2.0 format.

You can use [Applesauce](https://applesaucefdc.com) to generate such files from original floppy disks.
Or you can convert `.dsk` or `.d13` images using [DskToWoz2](https://github.com/cmosher01/DskToWoz2).





#### unload

The `unload` command removes a floppy disk image from one of the emulated disk drives.

``` conf
unload slot <slot> drive <drive>
```

`slot` Slot number, 0 through 7, of Disk \]\[ controller card to which the drive is attached.

`drive` Drive number, 1 or 2, of the disk drive on the controller card to remove the floppy disk image from.

The `unload` command removes the disk from the specified slot and drive.

> #### Warning
> If the disk has been modified but not saved, the modifications will be **DISCARDED**.





#### save

The `save` command saves changes made on an emulated floppy disk back to the original image file.

``` conf
save slot <slot> drive <drive>
```

`slot` Slot number, 0 through 7, of Disk \]\[ controller card to which the drive is attached.

`drive` Drive number, 1 or 2, of the disk drive on the controller card to save.

The `save` command saves any changes that the emulated Apple \]\[ has made to the floppy
disk image. It is important to note that the emulator operates on the image only in memory, and does
not immediately write changes back to the real file. You need to issue the `save` command
in order to write changes back to the file. Note that the emulator will display a asterisk `\*`
next to the file-name of a disk image if it has any unsaved changes.





#### cassette

The `cassette` command performs various operations of the emulated cassette tape.


``` conf
cassette load [ <file-path> ]
cassette rewind
cassette tone
cassette blank <file-path>
cassette save
cassette eject { in | out }
```

`file-path` File path of the cassette tape image file, a standard WAVE file.

See below for more information about operating the emulated cassette tape interface.





#### revision

The `revision` command specifies which revision of Apple \]\[ motherboard to use.

``` conf
revision <rev>
```

`rev` Revision number of the motherboard. Currently, only two values make any difference in behavior: `0` or `1`.

The `revision` command chooses which revision of the Apple \]\[ motherboard to
use. The only revisions that make any difference (for now, at least) are 0 or 1. Zero
is the original (rare) version of the motherboard, that only had two hi-res
colors (green and purple), and always displayed text with green and purple fringes.

---



### Display

The orignal Apple \]\[s didn't come with a display. The user needed to use either a standard
television or a monitor in order to see the computer's output. The EPPLE \]\[ emulates a variety of
displays; you can cycle between the different types using `F2`.
There are two major types of displays: televisions and monitors. Monitors generally have
higher quality (sharper) displays. The displays show the normal visible area of the NTSC
video signal generated by the emulated Apple \]\[ machine.

#### Monitors

The emulator provides a color monitor, and a green monochrome monitor.
These emulate standard, no-frills NTSC monitors. The most noticeable characteristic of a monitor
is the horizontal display of pixels. Monitors react faster than TVs, so two adjacent pixels will
not merge together; both will be distinctly visible, with blackness between them. For example,
type in the following Applesoft command, then cycle through the display types. The monitors
will show thin, vertical, green lines; TVs will show continuous horizontal lines.

``` visualbasic
GR : COLOR=4 : HLIN 10,20 AT 10
```

#### Televisions

Televisions react more slowly to
changes in the incoming video signal than monitors do, and as a result, horizontal pixels will
merge together, forming a more uniform appearance. EPPLE \]\['s television mode emulates the
signal decoding circuitry of a real television. This includes separating out the *chroma* portion
of the incoming NTSC video signal using a filter algorithm, and *calculating* the color to display.

#### Scan Lines

NTSC displays (TVs or monitors) usually receive signals that are interlaced. However, the
Apple \]\[ doesn't generate interlaced screens. This causes blank rows between each displayed
row of pixels. The EPPLE \]\[ emulates this behavior, but also allows you to *fill in* these
black rows with a copy of the row above it, for a more continuous display (vertically). Use
the `F4` key to toggle between these two modes.

#### Resolution

The Apple \]\[s are commonly documented as having a resolution of 280x192 pixels, and to an
extent this is true. Vertically there are 192 pixels, but since there is no interlacing,
it is more accurate to display them with one blank space between each. So the EPPLE \]\[ has
two times 192, or 384, vertical pixels in its display. Horizontally there are 280 pixels, but
each could also be shifted right one-half dot, allowing for two times 280, or 560, different
horizontal displayable positions. To emulate this, the EPPLE \]\[ shows each emulated pixel as
two pixels wide, and displays an emulated half-dot shift as an actual one pixel shift. So the
display area of the EPPLE \]\[ is 560x384. There is an informational area below and to the
right of the emulated display that shows various statistics of the emulator. So the total screen
area used by the EPPLE \]\[ is a standard 640x480 pixels.

#### Full Screen

The EPPLE \]\[ can run in either full-screen mode, or within a window. Use the `F3` key to toggle
between the two.

#### Informational Area

The area at the bottom and the right of the EPPLE \]\[ display show various information about
the emulator.

* POWER light
* current cards in SLOTS
* CASSETTE tape information
* emulated CPU speed (MHz)
* function-keys help

---



### Keyboard

The EPPLE \]\[ emulates the original Apple \]\[ keyboard. The original Apple \]\[
keyboard had symbols in different places than current common PC keyboards. For
example, Shift-2 on the Apple \]\[ produces a double quote `"`, but on a PC keyboard
it produces an at-sign `@`. For ease of typing, the EPPLE \]\[ emulator does not
mimic the positions of the original keys, but rather mimics the symbols on
the current PC keyboard. So, for example, if you type Shift-2 on the PC
keyboard into the EPPLE \]\[, it produces an at-sign, as you would normally expect.

The Apple \]\[ keyboard didn't produce lower-case letters; neither does the emulator.
Also, the Apple couldn't produce an opening square bracket `[`, braces `{}`, vertical
bar `|`, or backslash `\`. There were no up- or down-arrow keys. You cannot type these into
the emulator, either. There are other, unusual, cases that are emulated correctly,
as well, such as typing Control in conjunction with a number key simply produces
that number. So typing a Control-3 is the same as just typing a 3. Also, typing
Shift-Control-2 produces the NUL character (ASCII $80).

The Apple \]\[ keyboards didn't automatically repeat typing characters when a
key was held down. Instead, the user would hold down the REPT (*repeat*) key
while holding down the key that was to be repeated. On the EPPLE \]\[, this
behavior is emulated, and the `F10` key is used as the REPT key.

The Apple \]\[ had no keyboard buffer (actually, it had a buffer of one character).
So if you typed several characters on the keyboard before the currently running
program had a chance to read them, they would get lost (only the final character
typed would be remembered). This behavior can be
toggled on or off in the EPPLE \]\[. By default, the EPPLE \]\[ will buffer up any
characters you type and deliver them to the emulated machine when it asks for
them. However, the Apple program must be written properly to allow this to work.
Some Apple \]\[ programs (like maybe some games) may not work correctly in this
respect, so you may want to turn off buffering in these cases. With buffering
turned off, the EPPLE \]\[ accurately emulates the original Apple \]\[. Use the `F12`
key to toggle the keyboard buffering. Note that pasting from the clipboard (with
the Insert key) will most definitely cause keys to be lost if the keyboard
buffer is turned off. So if you have a big Applesoft program in the clipboard
and you want to paste it into the EPPLE \]\[ correctly, make sure the keyboard
buffer is on.

Special Keys:

* `F1` Emulates the POWER switch on the back of the Apple \]\[.
* `F2` Cycles among different display types (TV, monitor, etc.).
* `F3` Toggles between full-screen or window display.
* `F4` Toggles between showing scan lines on the display, or duplicating
each scan line to the following line, to fill-in the otherwise black line.
* `F5` Go to *command entry* mode.
* `F6` Emulates the RESET key.
* `F7` Pastes characters from the clipboard into the emulated Apple
(as if they had been typed on the keyboard).
* `F8` Save a bitmap file of the current EPPLE \]\[ screen.
The file will be in the default directory, named `ep2_YYYYMMDDHHMMSS.bmp`.
* `F9` Quit the EPPLE \]\[ program, *immediately*!
* `F10` Emulates the REPT key.
* `F11` Toggles between running the emulator at authentic speed
(1.02 MHz CPU), or as fast as possible.
* `F12` Toggles the keyboard buffer.

---



### Peripheral Cards

#### Disk \]\[ Controller

The Disk \]\[ Controller card emulates the floppy disk controller card and associated
disk drives in the original Apple \]\[ systems. In the emulator, each card has two
drives attached to it, referred to as drive 1 and drive 2. The floppy disks
themselves are represented by a *nibble* image of the contents. The
emulator emulates the hardware, but to be of any use, you will need to provide
the firmware ROM code.

To use a disk card and drives, add these lines to your epple2.conf file, for example:

``` conf
slot 6 disk
import slot 6 rom 0 /usr/lib/apple2/dos3x/16sector/controller/disk2.a65
```

The first line uses the `slot` command to
insert a disk contoller card into slot 6, which is the standard
slot used for disk cards.
The `disk` keyword loads the 16-sector P6 ROM (Logic State Sequencer).
Alternatively, use `disk13` to load the 13-sector P6 ROM (for DOS 3.2 or earlier).
The next line uses the `import`
command to load the card's ROM with the disk controller
firmware. This firmware is known as the *bootstrap* or *P5* ROM code.
It is seen by the Apple \]\[ at memory addresses `$Cs00-$CsFF`, where s is the
slot number (so in the common case of the card being in slot 6, the ROM is
at `$C600-$C6FF`). The firmware is copyright by Apple, and is available from
the [Apple II Library](https://mosher.mine.nu/apple2/).

You can also load a floppy disk image (nibble format) into the drive, either by putting
the `load` command into the `epple2.conf` file, or by using the command prompt
in the emulator (`F5` key). For example, you could load the DOS 3.3 system master into
slot 6, drive 1, with this command

``` conf
load slot 6 drive 1 /usr/lib/apple2/dos3x/16sector/disks/dos330/clean330sysmas.nib
```




#### Language

The language card emulates an Apple 16K RAM card, commonly called a Language Card.
To use a language card, add this line to your epple2.conf file:

``` conf
slot 0 language
```

Note that DOS and ProDOS will make use of a language card only if it is in slot *zero*.

The language card has RAM at addresses `$E000`-`$FFFF`, as well as two banks of RAM
at addresses `$D000`-`$DFFF`. A program switches between these RAMs and/or the
motherboard ROM by using the I/O switches at `$C080`-`$C08F`.

The information area of the Epple \]\[ will show the current state of the
language card as follows:

*  `R`   Read from card RAM (vs. motherboard ROM)
*  `W`   Write to card RAM (vs. write-disabled)
*  `B1`  Use `$D000` bank 1
*  `B2`  Use `$D000` bank 2

An overview of the I/O switches that control the language card
is provided by Jim Sather in
[*Understanding the Apple II*](https://archive.org/details/understanding_the_apple_ii/page/n121),
p. 5-30, Table 5.4, as follows:

	+==================================================================+
	| BANK2   BANK1                 ACTION                  READ?      |
	+==================================================================+
	|  C080    C088      WRTCOUNT = 0*, WRITE DISABLE      ENABLE      |
	|_ C084 _  C08C ___________________________________________________|
	| RC081   RC089      WRTCOUNT = WRTCOUNT + 1*          DISABLE     |
	|_RC085 _ RC08D ___________________________________________________|
	| WC081   WC089      WRTCOUNT = 0*                     DISABLE     |
	|_WC085 _ WC08D ___________________________________________________|
	|  C082    C08A      WRTCOUNT = 0*, WRITE DISABLE      DISABLE     |
	|_ C086 _  C08E ___________________________________________________|
	| RC083   RC08B      WRTCOUNT = WRTCOUNT + 1*          ENABLE      |
	|_RC087 _ RC08F ___________________________________________________|
	| WC083   WC08B      WRTCOUNT = 0*                     ENABLE      |
	|_WC087 _ WC08F ___________________________________________________|

       * Writing to expansion RAM is enabled when WRTCOUNT reaches 2.



#### Firmware

The firmware card emulates a (modified) Apple Firmware card.
The firmware card is simply an alternate ROM, at addresses
`$D000`-`$FFFF`, that is switched using the I/O switches
at addresses `$C080`-`$C08F`. In order to make use of the
firmware card, you will need to load the ROM with a binary image
from a file on disk. For example, to insert an Integer BASIC
firmware card into the emulator, add these lines to your
`epple2.conf` file:

``` conf
# Firmware card with Integer BASIC and old Monitor
slot 0 firmware
import slot 0 rombank 1000 /usr/lib/apple2/system/intbasic/intbasic.a65
import slot 0 rombank 2425 /usr/lib/apple2/system/other/other.a65
import slot 0 rombank 2800 /usr/lib/apple2/system/monitor/apple2/monitor.a65
```

For an Applesoft BASIC firmware card, use these:

``` conf
# Firmware card with Applesoft BASIC and Autostart Monitor
slot 0 firmware
import slot 0 rombank 0000 /usr/lib/apple2/system/applesoft/applesoft.a65
import slot 0 rombank 2800 /usr/lib/apple2/system/monitor/apple2plus/monitor.a65
```

Note that the addresses specified in the `epple2.conf` file for the
rombank are based on the beginning of the bank ROM itself. For example, specifying
`1000` (which is 1000 hex) represents the final memory address of `$E000`, because
the bank ROM is always based at address `$D000`.

The idea is that you would load your motherboard with, for
example, Applesoft BASIC and the Autostart Monitor ROM (to emulate
an Apple \]\[ plus), and then install a firmware card with Integer
BASIC and the old Monitor. Booting with DOS 3.3, then, would allow
you to type `FP` to use Applesoft BASIC, or `INT` to switch to Integer BASIC.

Note that DOS and ProDOS will make use of a firmware card only if it is in slot *zero*.

Jim Sather, in
[*Understanding the Apple II*](https://archive.org/details/understanding_the_apple_ii/page/n151),
on pages 6-18 through 6-21, explains
how to modify a firmware card to allow independent switching of the `$F800`-`$FFFF`
ROM memory. This area is occupied by the Monitor, so it is primarily
intended to allow the user to switch between the old Monitor and the Autostart
Monitor, independent of switching between Integer and Applesoft BASIC. The EPPLE \]\[
firmware card emulates this behavior.

The information area of the EPPLE \]\[ will show the current state of the
firmware card as follows:

* `D`   Read from firmware card `$D000`-`$F7FF` (vs. motherboard BASIC ROM)
* `F8`  Read from firmware card `$F800`-`$FFFF` (vs. motherboard Monitor ROM)



#### Clock

The clock card emulates a ProDOS-compatible real-time clock card for the Apple \]\[.
To use a clock card, you will need to configure the EPPLE \]\[ to insert one into
a slot, typically slot 4. You will also need to load the card with its ROM code,
which is provided with the emulator in the clock.a65 file.
For example, add this to your epple2.conf file:

``` conf
slot 4 clock
import slot 4 rom 0 /usr/lib/epple2/cards/clock.a65
```

Of course you may need to adjust the path for your particular system.

To verify that the clock card is working correctly, you can run the following Applesoft
program to retrieve the current time from the clock card and print it.
This program assumes the card is in slot 4.

``` visualbasic
NEW

10 CALL -15360 : REM $C400 SLOT 4 ENTRY POINT
20 A = 512 : REM $0200 INPUT BUFFER
30 C = PEEK(A)
40 IF C < 160 THEN 99
50 PRINT CHR$(C);
60 A = A+1
70 GOTO 30
99 END

RUN
```

The card returns data (into the `GETLN` input buffer at `$0200`) in
the following format:

`mm,ww,dd,hh,nn,ss,000,yyyy,Time Zone,v`

* `mm`    Month, 01-12
* `ww`    Weekday, 00=Sunday... 06=Saturday
* `dd`    Day, 01-31
* `hh`    Hour, 00-23
* `nn`    Minute, 00-59
* `ss`    Second, 00-61
* `000`   Milliseconds; always zero
* `yyyy`  Year, e.g., 2008
* `Time Zone`  time zone string (could contain lower-case characters, which won't display correctly)
* `v`     Daylight Saving Time in effect, 0=no, 1=yes

Note that only `mm,ww,dd,hh,nn` fields are used by ProDOS. The other
fields, `ss,000,yyyy,Time Zone,v`, are an EPPLE \]\[ extension. Also note
that ProDOS was not designed to work for years past 2007, so
ProDOS will show the incorrect year, but the other fields will be accurate.
I believe patches exist for ProDOS to fix this.



#### Standard Input

The *standard input* card doesn't emulate a real piece of hardware; rather, it
reads characters from standard input (stdin) (of the EPPLE \]\[ emulator).

To use a standard input card, add these lines to your `epple2.conf` file:

``` conf
# IN#2 reads from standard input
slot 2 stdin
import slot 2 rom 0 /usr/lib/epple2/cards/stdin.a65
```

That will insert a stdin card into slot 2, and then load its
ROM image into the card.
The stdin ROM is provided with the EPPLE \]\[ distribution.

For example, if you have a stdin card installed in slot 2, start
the EPPLE \]\[ emulator from the command line, and at the Applesoft
prompt, type `IN#2`. Then you can switch back to the
command shell, and whatever you type will be fed into the emulated
Apple. Use RESET or `IN#0` to go back to normal.



#### Standard Output

The *standard output* card doesn't emulate a real piece of hardware; rather, it acts
similar to a printer card, but instead of sending characters to a printer, it sends
them to standard output (stdout) (of the EPPLE \]\[ emulator).

To use a standard output card, add these lines to your `epple2.conf` file:

``` conf
# PR#1 prints to standard output
slot 1 stdout
import slot 1 rom 0 /usr/lib/epple2/cards/stdout.a65
```

This will insert a stdout card into slot 1 (which is the typical
slot for a printer card), and then load its ROM image into the card.
The stdout ROM is provided with the EPPLE \]\[ distribution.

For example, if you have a stdout card installed, at the Applesoft
prompt, type `PR#1`. Whatever you type next will be
echoed to standard output. Type `PR#0` to stop echoing.

---



### Cassette Tape Interface

The Apple \]\[ and Apple \]\[ plus machines have the ability to save and load binary
data to and from cassette tape. The user would attach a standard cassette tape
recorder to the jacks on the back of the Apple \]\[, and use the monitor commands
`R` and `W`, or the BASIC commands `LOAD` and `SAVE`, to read and write data
on the cassette tape. The user would have to press the play and/or record buttons
on the player at the right time.

The Apple \]\[ has two cassette ports, CASSETTE IN and CASSETTE OUT. To save a program to
tape, the user would attach a cassette recorder to the CASSETTE OUT port, load a blank
cassette into the recorder, press RECORD (and PLAY), then on the Apple type SAVE. When
finished, the user would press STOP, and eject the tape.

To load a previously saved program
from tape, the user would attach the player to the CASSETTE IN port, then load and REWIND
the tape. The user would PLAY the tape until the header tone could be heard, then STOP.
On the Apple \]\[ the user would type LOAD and immediately press PLAY on the cassette player.
After the file loaded, the user would STOP and eject the tape.

The Epple \]\[ emulates the cassette interface, using a standard WAVE (PCM) file to
hold the recorded portion of the tape. It provides two ports, one for CASSETTE IN and
a separate one for CASSETTE OUT. Generally you'll use only one at a time. Use CASSETTE IN
for LOADing a program, or CASSETTE OUT for SAVEing a program.

To load a program from a cassette image WAVE file, use `cassette load`
to put the tape into the cassette player. The tape will automatically rewind and
advance to the header tone. Then use the Apple LOAD command to load the program
from the tape. If the Apple gives you `ERR`, that means it could not interpret the
WAVE audio correctly.
If you want to rewind the tape, you can use the `casssette rewind` command.
Use `cassette eject in` to close the file.

To save an in-memory program to a cassette tape image WAVE file, use
`cassette blank <file-path>` to put a new blank tape image into
the cassette recorder. Then use the Apple SAVE command to record to the tape, and then
`cassette save` to save the WAVE file. Use `cassette eject out` to close the file.

The emulator will not overwrite existing data in a tape image file.

#### Commands

`cassette load [ <file-path> ]`

This loads an existing WAVE file (from the host computer) containing a cassette tape image
onto the CASSETTE IN port,
in preparation for loading the program from it. If `file-path`
is omitted, a dialog box will be presented to select the file to load.
The tape is automatically positioned at the first header tone.

`cassette rewind`

This command rewinds the tape currently on the CASSETTE IN port. After rewinding
the tape, you will typically need to fast-forward to the head tone using
the `cassette tone` command.

`cassette tone`

If more than one program is stored in one WAVE file, then after loading the first
program, you may need to fast-forward to the next header tone. This command will
do just that.

`cassette blank <file-path>`

This creates a new empty file (on the host computer) that represents a cassette tape image,
and loads it onto the CASSETTE OUT port,
in preparation for saving a program to it.
The file must not already exist. The file type should be `.wav` to indicate a WAVE format file.

`cassette save`

This command saves the changed tape to the file. Note that the display will show
an asterisk `*` next to the file name if there are unsaved changes that need to
be saved.

`cassette eject { in | out }`

This removes the file from the specified cassette port (CASSETTE IN port, or CASSETE OUT port).

#### Example of Saving to Tape

Start up the emulator with Applesoft ROMs for this tutorial.
Enter a simple Applesoft program, just as an example, that we
are going to save to a cassette tape image file.

``` visualbasic
]NEW

]10 PRINT "HELLO"

]20 END

]LIST

10  PRINT "HELLO"
20  END

]RUN
HELLO
```

We first need to load a tape image file into the cassette machine.
Enter command mode by pressing `F5`, then make a new tape
image file.

``` conf
command: cassette blank hello.wav
```

This will create a new, empty tape file image named `hello.wav`
in the current default directory. (We could have specified a full path
name for the file if we wanted to place it in a different directory.)
Notice that the emulator now displays the name of the tape image file.

Next, we tell Applesoft to save the program to the cassette. For this,
we just use the `SAVE` command. Note that this is not the
DOS `SAVE` command; the DOS command has a file name after
`SAVE`. We just use `SAVE` with no file name.

``` visualbasic
]SAVE
```

It will take 10 seconds or so for it to save. Notice that the
current position of the tape is counting up as the Apple saves
the program. When it is finished, you need to save the changes
to the file. Press `F5` and enter the emulator command to save
the tape image file.

``` conf
command: cassette save
```

Now do a NEW to clear the program, and LIST to prove that it's gone.

``` visualbasic
]NEW

]LIST
```

We can now eject the tape (close the file).

```
command: cassette eject out
```

#### Example of Loading from Tape

To load the saved program (from the previous section) into the Apple again,
we will need to first load the tape image file back into the cassette machine.
Press `F5` to enter command mode and load the image file.

``` conf
command: cassette load
```

This will bring up the Open File dialog box. Choose
`hello.wav` file you just saved. Notice the
emulator now displays the name of the tape image file, along with the
position and length of the tape image. Notice the emulator automatically
advances the tape to the first header section.

Next, we tell Applesoft to load the program from the cassette. For this,
we just use the `LOAD` command. Note that this is not the
DOS `LOAD` command; the DOS command has a file name after
`LOAD`. We just use `LOAD` with no file name.

``` visualbasic
]LOAD
```

It will take several seconds for it to load. Notice that the
current position of the tape is counting up as the Apple loads
the program. When it is finished, the program will be loaded.

``` visualbasic
]LIST

10  PRINT "HELLO"
20  END

]RUN
HELLO
```

---



### Paddles

The Epple \]\[ provides two paddles to the Apple \]\[ machine.
One paddle is controlled by moving the mouse left and right; the other
paddle is controlled by moving the mouse up and down. The paddle buttons
are emulated by the mouse buttons (left and right click).

In
[*Understanding the Apple II*](https://archive.org/details/understanding_the_apple_ii/page/n188),
on page 7-33, Jim Sather describes soldering fixed resistors across a game connector
to create two real-time clock references. This is emulated by the Epple \]\[. Paddle timers 2 and 3
are 100-microsecond and 1-millisecond references, respectively.

---



### Speaker

The Apple \]\[ could generate sound via a speaker that generated square wave
audio. A program could read memory location `$C030` to toggle the speaker and
generate a *click* sound.

The Epple \]\[ emulator will generate sounds from the emulated Apple and
send them to the audio device. It generates 8-bit mono sound, with a 20,410 Hz
sampling rate.
