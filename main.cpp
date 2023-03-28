#include "mbed.h"
#include "uLCD_4DGL.h"


PwmOut PWM1(D6);
AnalogIn Ain(A0);
AnalogIn Ain2(A2);
DigitalOut PinA(D8);
InterruptIn PinB(D9);
uLCD_4DGL uLCD(D1, D0, D2);

Thread thread1;
Thread t;

int bufferSize = 50;
int sampleIndex = 0;
int threshold_voltage = 2;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

void PWM_generator() {
    while (1)
  {
    PWM1.period_ms(5);
    PWM1.pulsewidth_us(Ain.read()*3.3*5000);
    //printf("%f\n\r", PWM1.read());
    ThisThread::sleep_for(50ms);
  }
}

void messaging() {
    uLCD.cls();
    uLCD.text_width(4); // 4X size text
    uLCD.text_height(4);
    uLCD.color(RED);
    uLCD.locate(1, 2);
    uLCD.printf("%d", sampleIndex);
}

void ISR() {
    queue.call(messaging);
}

int main()
{ 
  thread1.start(PWM_generator);
  t.start(callback(&queue, &EventQueue::dispatch_forever));  
  PinB.rise(&ISR);
  PinB.fall(&ISR);
  while (1) {
    float sum = 0;
    float ADCdata[50] = {0};
    for (int i = 0; i < bufferSize; i++)
    {
        ADCdata[i] = Ain2;
        sum += ADCdata[i];
        ThisThread::sleep_for(1ms);
    }
    sampleIndex += 50;
    float average = sum / 50 * 3.3;
    if (average > threshold_voltage) {
        PinA = 1;
    } else {
        PinA = 0;
    }
  }
}