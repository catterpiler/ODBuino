typedef void  (*TaskFunctionCallback)  ();

class Schedular 
{
  private:
    unsigned long lastRun;
    int timesRun;	
  public:
    // public methods
    Schedular();
    void start();
    void start(int delayStartBy); 
    void stop();                     
    void check(TaskFunctionCallback MethodeToCall,int Interval);
	void pause();
	int timesToRun;
	
};