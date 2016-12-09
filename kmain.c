#include "io.h"

#define VIDEO_MEMORY 0x000B8000
#define FB_GREEN 2
#define FB_DARK_GREY 8

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{   
    char *fb = (char *)VIDEO_MEMORY;
    
    fb[(i*2)] = c;
    fb[(i*2) + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_write(char *buffer, unsigned int length)
{
	for (unsigned int i = 0; i < length; i++)
	{
		fb_write_cell(i, buffer[i], 0, 15);
		fb_move_cursor(i);
	}
}

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same order,
 * but they start at different values.
 */

#define SERIAL_COM1_BASE				0x3F8

#define SERIAL_DATA_PORT(base)			(base)
#define SERIAL_FIFO_COMMAND_PORT(base)	(base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)	(base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base)	(base + 4)
#define SERIAL_LINE_STATUS_PORT(base)	(base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow.
 */
#define SERIAL_LINE_ENABLE_DLAB			0x80

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial port
 *  is 115200 bits/s. The argument is a divisor of that number, hence the
 *  resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com		The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
	outb(SERIAL_LINE_COMMAND_PORT(com),
		 SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com),
		 (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com),
		 divisor & 0x00FF);
}

/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set to have a data
 *  length of 8 bits, no parity bits, one stop bit and break control disabled.
 *
 *  @param com	The serial port to configure
 */
void serial_configure_line(unsigned short com)
{
	/* Bit:		| 7 | 6 | 5 4 3 | 2 | 1 0 |
	 * Content:	| d | b |  prty | s |  dl |
	 * Value:	| 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
	 */
	outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

// NOTE: I ADDED THE BELOW METHOD MYSELF (NEEDS TESTED)
/** serial_configure_buffers:
 *  Configures the buffers that hold the data transmitted via the serial port.
 *  This implementation configures the buffers to enable FIFO, clear both
 *  receiver and transmission FIFO queues, and use 14 bytes as size of the
 *  queue.
 *
 *  @param com	The serial port to configure
 */
void serial_configure_buffers(unsigned short com)
{
	/* Bit:		| 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
	 * Content: | lvl | bs | r | dma | clt | clr | e |
	 * Value:	| 1 1 | 0  | 0 | 0   | 1   | 1   | 1 | = 0xC7
	 */
	outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

// NOTE: I ADDED THE BELOW METHOD MYSELF (NEEDS TESTED)
/** serial_configure_modem:
 *  Configures the modem control register (which is used for simple hardware
 *  flow control via the Ready To Transmit (RTS) and Data Terminal Ready (DTR)
 *  pins). This implementation does not enable interrupts as it doesn't handle
 *  any received data.
 *
 *  @param com	The serial port to configure
 */
void serial_configure_modem(unsigned short com)
{
	/* Bit:		| 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
	 * Content:	| r | r | af | lb | ao2 | ao1 | rts | dtr |
	 * Value:	| 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1   | = 0x03
	 * (RTS = 1 and DTS = 1)
	 */
	 outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not for the given COM
 *  port.
 *
 *  @param com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *			1 if the transmit FIFO queue is empty
 */
int serial_is_transmit_fifo_empty(unsigned int com)
{
	/* 0x20 = 0010 0000 */
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(char *buffer, unsigned int length)
{	
	for (unsigned int i = 0; i < length; i++)
	{
		while (serial_is_transmit_fifo_empty(SERIAL_COM1_BASE) == 0);
	
		outb(SERIAL_COM1_BASE, buffer[i]);
	}
}

void kmain()
{
    char *text = "Kavan's OS wrote this line!";
    fb_write(text, 28);
    
    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffers(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);
    serial_write(text, 28);
}
