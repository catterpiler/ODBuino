#include <easyscheduler.h>
#include <Arduino.h>

Schedular::Schedular()
{
  lastRun = 0;
  timesRun = 0;
}

// Check if task needs to run 
void Schedular::check(TaskFunctionCallback MethodeToCall,int Interval)
{
  if(lastRun > 0)
  {
    if(lastRun+Interval < millis() && (timesRun < timesToRun || timesToRun == 0))
    {
      MethodeToCall();
      lastRun = millis();
	  timesRun++;
    }
	
	
  };
}

// Start as soon as the the function fires
void Schedular::start()
{
  lastRun = millis()+1;
}

// Delays start by 'delayStartBy' milli seconds
void Schedular::start(int delayStartBy)
{
  lastRun = millis()+delayStartBy;
}

// Stops Task
void Schedular::stop()
{
  lastRun = 0;
  timesRun = 0;
}