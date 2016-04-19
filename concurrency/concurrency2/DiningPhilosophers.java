import java.util.concurrent.Semaphore;
import java.util.concurrent.ThreadLocalRandom;

// After reading this fun summary of Dining Philosophers, I've decided to implement the solution through Chandy/Misra:
// http://adit.io/posts/2013-05-11-The-Dining-Philosophers-Problem-With-Ron-Swanson.html

/**
 * Abstract representation of a fork entity.
 */
class Fork {
    private boolean dirty;
    private int user;
    private Semaphore mutex;
    public int id;

    /**
     * Constructor for fork object.
     * @param id - Identification number for fork.
     * @param userID - Identification number of the user of the fork.
     */
    public Fork(int id, int userID) {
        this.dirty = true;
        this.user = userID;
        this.mutex = new Semaphore(1);
        this.id = id;
    }

    /**
     * Method to perform mutex lock.
     */
    public void lock() {
        try {
            mutex.acquire();
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    /**
     * Method to perform mutex unlock.
     */
    public void unlock() {
        mutex.release();
    }

    /**
     * Method to perform getting a fork
     * @param userID The user getting the fork.
     */
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

    /**
     * Method to perform setting or putting a fork. Basically to conform with Chandy/Misra,
     * we set the fork as dirty and pass it over.
     */
    public synchronized void setFork() {
        this.dirty = true;
        notifyAll();
    }
}

/**
 * Abstract representation of a philosopher entity.
 */
class Philosopher extends Thread {
    private int pid;
    public Fork leftFork;
    public Fork rightFork;

    /**
     * Constructor for philosopher entity.
     * @param pid - Identification number for philosopher.
     * @param leftFork - Fork to the left of philosopher.
     * @param rightFork - Fork to the right of philosopher.
     */
    public Philosopher(int id, Fork leftFork, Fork rightFork) {
        this.pid = id;
        this.leftFork = leftFork;
        this.rightFork = rightFork;
    }

    /**
     * Method to simulate interacting threads as philosophers at the table.
     */
    public void run() {
        while (true) {
            think();
            leftFork.getFork(pid);
            rightFork.getFork(pid);
            eat();
            leftFork.setFork();
            rightFork.setFork();
        }
    }

    /**
     * Method to perform individual thread action of thinking.
     */
    public void think() {
        try {
            System.out.println("Philosopher " + pid + " is thinking!");
            // http://stackoverflow.com/questions/363681/generating-random-integers-in-a-specific-range
            int randomTime = (ThreadLocalRandom.current().nextInt(1, 20 + 1)) * 1000;
            Thread.sleep(randomTime);
            System.out.println("Philosopher " + pid + " has finished thinking!");
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    /**
     * Method to perform individual thread action of consuming.
     * Fork mutex locks are locked during execution to achieve concurrency.
     */
    public void eat() {
        try {
            leftFork.lock();
            rightFork.lock();
            System.out.println("Philosopher " + pid + " is eating!");

            // http://stackoverflow.com/questions/363681/generating-random-integers-in-a-specific-range
            int randomTime = (ThreadLocalRandom.current().nextInt(2, 9 + 1)) * 1000;
            Thread.sleep(randomTime);
            leftFork.unlock();
            rightFork.unlock();
            System.out.println("Philosopher " + pid + " has finished eating!");
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }
}

/**
 * Abstract representation of the Dining Philosophers problem.
 */
public class DiningPhilosophers {
    public Fork[] forks;
    private Philosopher[] philosophers;

    /**
     * Constructor for DiningPhilosophers entity.
     */
    public DiningPhilosophers() {
        forks        = new Fork[5];
        philosophers = new Philosopher[5];

        for (int i = 0; i < forks.length; i++) {
            int fid = 0;
            int setFork = ((i + 4) % 5);
            if (i < setFork) {
                fid = i;
            } else {
                fid = setFork;
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
        new DiningPhilosophers();
    }
}
