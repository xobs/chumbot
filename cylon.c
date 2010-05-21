/*
 * Cylon lights connected to LCD pins 0-17, located on bank 1, pins 0-17.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#define HW_PINCTRL_MUXSEL1      0x80018110
#define HW_PINCTRL_MUXSEL2      0x80018120
#define HW_PINCTRL_MUXSEL3      0x80018130
#define HW_PINCTRL_MUXSEL3_SET  0x80018134
#define HW_PINCTRL_MUXSEL3_CLR  0x80018138

#define HW_PINCTRL_DOUT1        0x80018510
#define HW_PINCTRL_DOUT1_SET    0x80018514
#define HW_PINCTRL_DOUT1_CLR    0x80018518

#define HW_PINCTRL_DOE1         0x80018710
#define HW_PINCTRL_DOE1_SET     0x80018714
#define HW_PINCTRL_DOE1_CLR     0x80018718

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

static int cylon_pin[] = {
    16,
    14,  
    12,
    11,
    9,
    7,
    4,
    2,
    0,
    17,
    15,
    13,
    10,
    8,
    6,
    5,
    3,
    1,

    1,
    3,
    5,
    6,
    8,
    10,
    13,
    15,
    17,
    0,
    2,
    4,
    7,
    9,
    11,
    12,
    14,
    16,
};

static pthread_t cylon_thread;
static int exit_cylon;

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


int cylon_init(void) {
    // Change mux to use pins as GPIOs
    write_kernel_memory(HW_PINCTRL_MUXSEL2,     0xffffffff);
    write_kernel_memory(HW_PINCTRL_MUXSEL3_SET, 0x0000000f);
	write_kernel_memory(HW_PINCTRL_DOUT1,       0x00000000);
	write_kernel_memory(HW_PINCTRL_DOE1,        0x0003ffff);
    return 0;
}

void *cylon_process(void *ignored) {
	int cylon_progress = 0;
	while(!exit_cylon) {
		write_kernel_memory(HW_PINCTRL_DOUT1_SET, 1<<cylon_pin[cylon_progress]);

		// Increment the current pin, looping if necessary.
		cylon_progress++;
		if(cylon_progress >= sizeof(cylon_pin)/sizeof(*cylon_pin))
			cylon_progress = 0;

		write_kernel_memory(HW_PINCTRL_DOUT1_CLR, 1<<cylon_pin[cylon_progress]);
		usleep(30000);
	}
	return NULL;
}


int cylon_start(void) {
	exit_cylon = 0;
	pthread_create(&cylon_thread, NULL, cylon_process, NULL);
	pthread_detach(cylon_thread);
	return 0;
}

void cylon_stop(void) {
	exit_cylon = 1;
}
