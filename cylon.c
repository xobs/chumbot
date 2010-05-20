/*
 * Author:  Michael Ortiz
 * Email:   mtortiz.mail@gmail.com
 * 
 * Desc:    Functions for car pwm signal control. Includes a tuning driver.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#define HW_PINCTRL_MUXSEL3      0x80018130
#define HW_PINCTRL_MUXSEL3_SET  0x80018134
#define HW_PINCTRL_MUXSEL3_CLR  0x80018138

#define HW_PWM_CTRL             0x80064000
#define HW_PWM_CTRL_SET         0x80064004
#define HW_PWM_CTRL_CLR         0x80064008
#define HW_PWM_ACTIVE0          0x80064010
#define HW_PWM_PERIOD0          0x80064020
#define HW_PWM_ACTIVE1          0x80064030
#define HW_PWM_PERIOD1          0x80064040
#define HW_PWM_ACTIVE2          0x80064050
#define HW_PWM_PERIOD2          0x80064060
#define HW_PWM_ACTIVE3          0x80064070
#define HW_PWM_PERIOD3          0x80064080
#define HW_PWM_ACTIVE4          0x80064090
#define HW_PWM_PERIOD4          0x800640a0

static int *mem = 0;
static int fd = 0;
static int *prev_mem_range = 0;

/******************************************************************************
 * read_kernel_memory
 *
 * params:
 *      long    offset - the offset of memory location to read
 *
 * desc:
 *      this function will return the 4-bytes at the specified
 *      memory offset of kernel memory
 *****************************************************************************/
static int read_kernel_memory(long offset) {
    int result;

    int *mem_range = (int *)(offset & ~0xFFFF);
    if( mem_range != prev_mem_range ) 
    {
        prev_mem_range = mem_range;

        if(mem)
            munmap(mem, 0xFFFF);
        if(fd)
            close(fd);

        fd = open("/dev/mem", O_RDWR);
        if( fd < 0 ) {
            perror("Unable to open /dev/mem");
            fd = 0;
            return -1;
        }

        mem = mmap(0, 0xffff, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset&~0xFFFF);
        if( -1 == (int)mem ) {
            perror("Unable to mmap file");

            if( -1 == close(fd) )
                perror("Also couldn't close file");

            fd=0;
            return -1;
        }
    }

    int scaled_offset = (offset-(offset&~0xFFFF));
    result = mem[scaled_offset/sizeof(long)];

    return result;
}

/******************************************************************************
 * write_kernel_memory
 *
 * params:
 *      long    offset - the offset of memory location to write
 *
 * desc:
 *      this function will write the 4-byte value at the specified
 *      memory offset of kernel memory
 *****************************************************************************/
static int write_kernel_memory(long offset, long value) {
    int old_value = read_kernel_memory(offset);
    int scaled_offset = (offset-(offset & 0xFFFF0000L));
    if(mem)
        mem[scaled_offset/sizeof(long)] = value;
    return old_value;
}


/******************************************************************************
 * pwm_init
 *
 * params:
 *      na
 *
 * desc:
 *      configures the pwm pins
 *****************************************************************************/
int pwm_init()
{
    //Change mux to use pins and set to pwm output for PWM3 and PWM4
    write_kernel_memory(HW_PINCTRL_MUXSEL3_CLR, 0x3f000000);
    write_kernel_memory(HW_PWM_CTRL_SET,        0x0000001c);
    return 0;
}

/******************************************************************************
 * pwm_turn
 *
 * params:
 *      int value - pwm register setting for the "turning" pwm signal 
 *
 * desc:
 *      sets the "turning" pwm register to the specified value
 *****************************************************************************/
int pwm_right(int value)
{
	value = (value << 16) & 0xffff0000L;
	//fprintf(stderr, "Setting HW_PWM_ACTIVE3: %08x: ", value);
    write_kernel_memory(HW_PWM_ACTIVE3, value);
	//fprintf(stderr, "%08x\n", read_kernel_memory(HW_PWM_ACTIVE3));

	// Divide the 24 MHz crystal by 8, and set a period of 0xea60.
	// This will give us a period of 20ms.
	write_kernel_memory(HW_PWM_PERIOD3, 0x003bea60);
	return 0;
}

/******************************************************************************
 * pwm_drive
 *
 * params:
 *      int value - pwm register setting for the "drive" pwm signal 
 *
 * desc:
 *      sets the "drive" pwm register to the specified value
 *****************************************************************************/
int pwm_left(int value)
{
	//fprintf(stderr, "Setting HW_PWM_ACTIVE4: %08x: ", value<<16);
    write_kernel_memory(HW_PWM_ACTIVE2, value << 16);
	//fprintf(stderr, "%08x\n", read_kernel_memory(HW_PWM_ACTIVE3));

	// Divide the 24 MHz crystal by 8, and set a period of 0xea60.
	// This will give us a period of 20ms.
    write_kernel_memory(HW_PWM_PERIOD2, 0x003bea60);
    return 0;
}
