#include <arch/types.h>
#include <arch/cpu.h>
#include <drivers/serial.h>
#include <drivers/fb.h>
#include <drivers/console.h>
#include <kernel/device.h>

void kernel_main(void) {
    serial_write("kernel_main started\n");
    
    //initialize framebuffer
    fb_init();
    
    if (fb_available()) {
        //initialize console
        con_init();
        con_clear();
        
        con_set_fg(FB_RGB(0, 255, 128));
        con_print("DeltaOS Kernel\n");
        con_print("==============\n\n");
        
        con_set_fg(FB_WHITE);
        con_print("Console: initialized\n");
        con_print("Timer: running @ 100Hz\n");
        
        //demonstrate device manager works
        struct device *con = device_find("console");
        if (con && con->ops->write) {
            con->ops->write(con, "Device manager: working!\n", 25);
        }
    }
    
    //main kernel loop
    while (1) {
        arch_halt();
    }
}