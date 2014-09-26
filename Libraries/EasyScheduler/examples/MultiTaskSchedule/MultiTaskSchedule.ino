/*
 
 Quick and Easy Scheduler

 This module makes it easy to schedule repeating tasks with only
 a few lines of code.

 Features:
  # Simplifies code
  # Does not block the 'loop' function
  # Runs a defined function(Task) you created at a set interval.
  # It can delay the run of a task on start-up
  # Set the number of times you want it to run.
  # Stop,Start and pause scheduled tasks

 created 12 Jan 2013
 by Bertus Kruger
 http://blog.protoneer.co.nz

*/
 
#include <EasyScheduler.h>

int led = 13;
Schedular Task1;
Schedular Task2;

void setup() {                
  pinMode(led, OUTPUT);
  Task1.start();
  Task2.start();
}

void loop() {
    Task1.check(shortBlink,500); 
    Task2.check(longBlink,5000); 
}

void shortBlink()
{
  // Flash the onboard led
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
}

void longBlink()
{
  // Flash the onboard led
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
}