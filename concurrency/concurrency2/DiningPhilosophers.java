import java.util.concurrent.Semaphore;
import java.util.concurrent.ThreadLocalRandom;

// After reading this fun summary of Dining Philosophers, going to implement solution through Chandy/Misra:
// http://adit.io/posts/2013-05-11-The-Dining-Philosophers-Problem-With-Ron-Swanson.html
class Fork {
    private boolean dirty;
    private int user;
    private Semaphore mutex;
    public int id;

    public Fork(int id, int userID) {
        this.dirty = true;
        this.user = user;
        this.mutex = new Semaphore(1);
        this.id = id;
    }

    public void lock() {
        try {
            mutex.acquire();
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    public void unlock() {
        mutex.release();
    }

    public synchronized void getFork(int userID) {
        while (this.user != userID) {
            try {
                if (dirty) {
                    try {
                        mutex.acquire();
                        System.out.println("Passing Fork " + id + " from " + user + " to " + userID);
                        this.dirty = false;
                        this.user = userID;
                    } catch (InterruptedException ex) {
                        Thread.currentThread().interrupt();
                    } finally {
                        mutex.release();
                    }
                } else {
                    System.out.println("Attempted to pass fork " + id + " from " + user + " to " + userID + " but failed!");
                    wait();
                }
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public synchronized void setFork() {
        this.dirty = true;
        notifyAll();
    }
}

class Philosopher extends Thread {
    private int pid;
    public Fork leftFork;
    public Fork rightFork;

    public void run() {
        while (true) {
            think();
            leftFork.getFork(pid);
            rightFork.getFork(pid);
            eat();
            System.out.println("Philosopher " + pid + " has finished eating!");
            leftFork.setFork();
            rightFork.setFork();
        }
    }

    public void eat() {
        try {
            leftFork.lock();
            rightFork.lock();
            System.out.println("Philosopher " + pid + " is eating!");
            // TODO Fix sleeping here
            Thread.sleep((long) (Math.random() * 5000));
            leftFork.unlock();
            rightFork.unlock();
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    public void think() {
        int randomTime = (ThreadLocalRandom.current().nextInt(1, 20 + 1)) * 1000;
        // System.out.println("The randomNumber: " + randomTime);
        try {
            Thread.sleep(randomTime);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    public Philosopher(int id, Fork leftFork, Fork rightFork) {
        this.pid = id;
        this.leftFork = leftFork;
        this.rightFork = rightFork;
    }
}

public class DiningPhilosophers {
    public Fork[] forks;
    private Philosopher[] philosophers;

    public DiningPhilosophers() {
        forks        = new Fork[5];
        philosophers = new Philosopher[5];

        for (int i = 0; i < forks.length; i++) {
            int fid = 0;
            if (i < ((i + 4) % 5)) {
                fid = i;
            } else {
                fid = ((i + 4) % 5);
            }

            Fork fork = new Fork(i, fid);
            System.out.println("Fork " + i + " is held by Philosopher " + fid);
            forks[i] = fork;
        }

        for (int i = 0; i < philosophers.length; i++) {
            philosophers[i] = new Philosopher(i, forks[i], forks[(i + 1) % 5]);
            philosophers[i].start();
        }
    }

    public static void main(String[] args) {
        System.out.println("Dining Philosophers2");
        System.out.println("States, T = thinking, W = Waiting, E = Eating, _ = Finished");
        new DiningPhilosophers();
    }
}
