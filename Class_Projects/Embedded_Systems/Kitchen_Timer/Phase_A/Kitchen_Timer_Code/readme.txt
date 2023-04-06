
NoConcurrency
- 4 tasks running in a polling method (not efficient at all)

Simple_Interrupt
- buttons have their own interrutps
- 7-seg code runs on the main thread

Interrupt_Driven
- all tasks have their own interrupt routine

Interrupt_Pooling_Driven
- a combination between pooling and interrupt driven
- all tasks have their own interrupt routine but they use isr flags to communicate with main thread

SimpleCounter
- buttons used to increment a counter that is displayed on the 7-seg
- debounce problems

ImprovedCounter [DeBounce todo]
- address the debounce problem by software
- there are pros and cons with this approach

Clock_v1
- counts in seconds
- button 1 increments counter
- button 2 resets counter and turns clock timer off

Clock_v2
- counts in minutes
- adjust 7-seg to turn on decimal point
- Pins were adjusted according to PCB Design (DIG1..4 and DS/SH_CP)
- fully functional

FreeRTOS_Example_1
- set 3 tasks running in parallel
  - Serial task
  - Blink Red LED Task
  - Blink Green LED Task

FreeRTOS_Example_2
- test the use of queues to change information between tasks

Clock_FreeRTOS
- Implementatino of Clock_v2 with FreeRTOS



