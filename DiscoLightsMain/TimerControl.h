class TimerControl
{
  public:
    TimerControl::TimerControl(int);
    void configTimer();
    void startTimerInterrupt();
    void stopTimerInterrupt();
  private:
    int NoNeed;
};


