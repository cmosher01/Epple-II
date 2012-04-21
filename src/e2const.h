/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef E2CONST_H
#define E2CONST_H

class E2Const
{
public:
	/*
		The NTSC standard defines the field rate as 60 fields per second. The number 60
		is based on the USA AC current frequency of 60 Hz. This, in turn, was based on the
		clock standard (60 seconds per minute and 60 minutes per hour).
	*/
	static const int NTSC_FIELD_HZ = 60;

	/*
		The NTSC standard defines 525 lines per frame, which was chosen to be a multiple
		of a small number of standard tubes at the time, to produce a rate between RCA's
		recommended 441 (used by NBC) and Philco's suggested 600-800 lines.
	*/
	static const int NTSC_LINES_PER_FRAME = 3*5*5*7;

	/*
		When color was added to the NTSC signal, studies by General Electric showed that
		minimum interference was achieved using a subcarrier frequency 455 times the field
		rate, which can also be obtained using standard tubes.
	*/
	static const int NTSC_COLOR_MULTIPLE = 5*7*13;

	/*
		Adding color to NTSC also required slowing down the frame rate, by dropping one
		field after every 1000.
	*/
	static const int NTSC_COLOR_DROP_FIELD = 1000;

	/*
		Calculate the color sub-channel rate, times 4.
		This will be the (approximate) Hz of the "14M"
		crystal oscillator in the Apple ][.
		14318181.818181818... Hz rounds to 14318182 Hz
		U.A.II, p.3-2
	*/
	static const int CRYSTAL_HZ = (int)(1.0F*NTSC_FIELD_HZ * NTSC_LINES_PER_FRAME * NTSC_COLOR_MULTIPLE * NTSC_COLOR_DROP_FIELD / (NTSC_COLOR_DROP_FIELD+1));

	/*
		U.A.II, p. 3-3
		Normal 6502 cycle == 14 crystal periods
		Long 6502 cycle == 16 crystal periods
	*/
	static const int CRYSTAL_CYCLES_PER_CPU_CYCLE = 14;
	static const int EXTRA_CRYSTAL_CYCLES_PER_CPU_LONG_CYCLE = 2;

	/*
		65 bytes per row (64 normal CPU cycles plus one long CPU cycle)
	*/
	static const int BYTES_PER_ROW = (int)((NTSC_COLOR_DROP_FIELD+1)*1.0F*CRYSTAL_HZ/(NTSC_FIELD_HZ/2*NTSC_COLOR_DROP_FIELD*NTSC_LINES_PER_FRAME*CRYSTAL_CYCLES_PER_CPU_CYCLE));
	static const int HORIZ_CYCLES = BYTES_PER_ROW;

	/*
		U.A.II, p. 3-2, "composite frequency... 1.0205 MHz"
		Actually 1020484 Hz.
	*/
	static const int AVG_CPU_HZ = (int)((1.0F*CRYSTAL_HZ*HORIZ_CYCLES)/(CRYSTAL_CYCLES_PER_CPU_CYCLE*HORIZ_CYCLES+EXTRA_CRYSTAL_CYCLES_PER_CPU_LONG_CYCLE));

	/*
		A normal NTSC field is 262.5 lines (half of a full frame's 525 lines).
		The Apple rounds this down to 262 lines.
	*/
	static const int NTSC_WHOLE_LINES_PER_FIELD = NTSC_LINES_PER_FRAME/2;

	static const int BYTES_PER_FIELD = BYTES_PER_ROW*NTSC_WHOLE_LINES_PER_FIELD;






	// exactly 1 million
	static const int MEGA = 1000000;

	static const int VISIBLE_BITS_PER_BYTE = 7;
	static const int VISIBLE_LINES_PER_CHARACTER = 8;

	/*
	 *                                1000+1 seconds   2 fields   1 frame         1000000 microseconds         63   50
	 * total horizontal line period = -------------- * -------- * ------------- * --------------------   =   ( -- + -- ) microseconds per line
	 *                                60*1000 fields   1 frame    3*5*5*7 lines   1 second                          90
	 *
	 *                                                                                    10   81
	 * horizontal blanking period = (1.5+4.7+.6+2.5+1.6) = 10.9 microseconds per line = ( -- + -- ) microseconds per line
	 *                                                                                         90
	 *
	 * visible line period = total horizontal line period minus horizontal blanking period =
	 * 
	 * 52   59
	 * -- + -- microseconds per line
	 *      90
	 *
	 *
	 * To avoid the over-scan area, the Apple ][ uses only the middle 75% of the visible line, or 4739/120 microseconds
	 * 
	 * Apple ][ uses half the clock rate, or 315/44 MHz, to oscillate the video signal.
	 * 
	 * The result is 315/44 MHz * 4739/120 microseconds/line, rounded down, = 282 full pixel spots across the screen.
	 * The Apple ][ displays 7 bits per byte hi-res or lo-res, (or 7 pixel-wide characters for text mode), so that
	 * gives 282/7, which rounds down to 40 bytes per line.
	 */
	static const int VISIBLE_BYTES_PER_ROW = (int)((((1.0F*(NTSC_COLOR_DROP_FIELD+1)/(NTSC_FIELD_HZ*NTSC_COLOR_DROP_FIELD)*2/NTSC_LINES_PER_FRAME*MEGA)-(1.5+4.7+.6+2.5+1.6)) * 3/4) * (CRYSTAL_HZ/2)) / MEGA / VISIBLE_BITS_PER_BYTE;

	/*
	 * NTSC total lines per frame (525) minus unusable lines (19 plus 20) = 486 usable lines
	 * To avoid the over-scan area, use the middle 80% of the vertical lines, giving 388 (rounded down) clearly visible lines
	 * Apple ][ uses only half the vertical resolution because it doesn't interlace, giving 194.
	 * Text characters are 8 pixels tall, so 194/8 rounded down gives 24 text lines.
	 * Multiply by 8 to give 192 lines total.
	 */
	static const int VISIBLE_ROWS_PER_FIELD = (NTSC_LINES_PER_FRAME-(20+19)) * 8/10 / 2 /VISIBLE_LINES_PER_CHARACTER*VISIBLE_LINES_PER_CHARACTER;

	static const int BLANKED_BYTES_PER_ROW = BYTES_PER_ROW-VISIBLE_BYTES_PER_ROW;
	static const int VISIBLE_BYTES_PER_FIELD = BYTES_PER_ROW*VISIBLE_ROWS_PER_FIELD;
	static const int SCANNABLE_ROWS = 0x100;
	static const int SCANNABLE_BYTES = SCANNABLE_ROWS*BYTES_PER_ROW;
	static const int RESET_ROWS = NTSC_WHOLE_LINES_PER_FIELD-SCANNABLE_ROWS;
	static const int RESET_BYTES = RESET_ROWS*BYTES_PER_ROW;



	static const int MIXED_TEXT_LINES = 4;
	static const int ROWS_PER_TEXT_LINE = 8;
	static const int MIXED_TEXT_CYCLE = (VISIBLE_ROWS_PER_FIELD-MIXED_TEXT_LINES*ROWS_PER_TEXT_LINE)*BYTES_PER_ROW;


	static int test()
	{
		if (NTSC_FIELD_HZ!=60) return NTSC_FIELD_HZ;
		if (NTSC_LINES_PER_FRAME!=525) return NTSC_LINES_PER_FRAME;
		if (NTSC_COLOR_MULTIPLE!=455) return NTSC_COLOR_MULTIPLE;
		if (NTSC_COLOR_DROP_FIELD!=1000) return NTSC_COLOR_DROP_FIELD;
		if (CRYSTAL_HZ!=14318182) return CRYSTAL_HZ;
		if (BYTES_PER_ROW!=65) return BYTES_PER_ROW;
		if (AVG_CPU_HZ!=1020484) return AVG_CPU_HZ;
		if (BYTES_PER_FIELD!=17030) return BYTES_PER_FIELD;
		if (VISIBLE_BYTES_PER_ROW!=40) return VISIBLE_BYTES_PER_ROW;
		if (VISIBLE_ROWS_PER_FIELD!=192) return VISIBLE_ROWS_PER_FIELD;
		if (RESET_BYTES!=390) return RESET_BYTES;
		if (BLANKED_BYTES_PER_ROW!=25) return BLANKED_BYTES_PER_ROW;
		if (VISIBLE_BYTES_PER_FIELD!=12480) return VISIBLE_BYTES_PER_FIELD;
		if (SCANNABLE_BYTES!=16640) return SCANNABLE_BYTES;
		return -1;
	}
};


#endif
