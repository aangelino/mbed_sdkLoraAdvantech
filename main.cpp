#include "mbed.h"
#include "DebouncedInterrupt.h"

DigitalOut led1(LED1);
//InterruptIn mybutton(USER_BUTTON);

Serial pc(SERIAL_TX, SERIAL_RX);

// Interrupt Service Routine on button press.
void button_push_isr( void )
{
    pc.printf("\n\r button interrupt \n\r");
}


// main() runs in its own thread in the OS
int main() {

pc.printf("\n\r new run 3but \n\r");

/*debounce code*/
DebouncedInterrupt user_interrupt(PA_9);
user_interrupt.attach(button_push_isr, IRQ_RISE, 500, true);

    while (true) {
        led1 = !led1;
        //pc.printf("\n\r toggle led \n\r");
        wait(0.5);
    }
}

