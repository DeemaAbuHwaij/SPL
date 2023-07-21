package bguspl.set.ex;

import bguspl.set.Env;

public class Timer implements Runnable{

    private Env env;
    public long currTime;

    Timer(Env env){
        this.env = env;
        currTime = env.config.turnTimeoutMillis;
    }

    public void run(){
        while(currTime > 0){
            boolean warn = currTime <= env.config.turnTimeoutWarningMillis;
            if(!warn) {
                currTime -= 1000;
                env.ui.setCountdown(currTime, warn);
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ignored) {}
            }
            else{
                currTime -= 10;
                env.ui.setCountdown(currTime, warn);
                try {
                    Thread.sleep(10);
                } catch (InterruptedException ignored) {}
            }
        }
    }
}
