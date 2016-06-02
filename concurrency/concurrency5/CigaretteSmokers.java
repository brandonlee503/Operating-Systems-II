/**
 * Brandon Lee
 * CS 444 Kevin McGrath
 * Concurrency 5 - CigaretteSmokers
 * 2 June 2016
 */
import java.util.concurrent.Semaphore;
import java.util.Random;

/**
 * Abstract representation of an agent entity.
 * Some helpful resources:
 * https://www.cs.mtu.edu/~shene/NSF-3/e-Book/SEMA/TM-example-smoker.html
 * http://njavatech.blogspot.com/2014/03/cigarette-smokers-problem-using.html
 * https://paulamarcu.wordpress.com/2014/08/16/cigarette-smokers-problem/
 */
class Agent extends Thread {
	private Semaphore chooses;
	private Semaphore tobacco;
	private Semaphore paper;
	private Semaphore matches;

	private Random rng = new Random();
	private int situation;

	/**
	 * Constructor for agent object
	 * @param   choice - Semaphore representation of agent selection
	 * @param   t - Tobacco semaphore representation
	 * @param   p - Paper semaphore representation
	 * @param   m - Matches semaphore representation
	 */
	public Agent(Semaphore choice, Semaphore t, Semaphore p, Semaphore m) {
		chooses = choice;
		tobacco = t;
		paper   = p;
		matches = m;
	}

	/**
	 * Method to simulate interacting threads as agent.
	 */
	public void run() {
		while (true) {
			try {

				// Wait before beginning
				sleep(1000);

				// Select random situation
				situation = rng.nextInt(3);
				System.out.println("Agent placing ingredients:");
				if (situation == 0) {
					System.out.println("Matches and Paper");
					chooses.acquire();
					matches.release();
					paper.release();
				} else if (situation == 1) {
					System.out.println("Matches and Tobacco");
					chooses.acquire();
					matches.release();
					tobacco.release();
				} else if (situation == 2) {
					System.out.println("Paper and Tobacco");
					chooses.acquire();
					paper.release();
					tobacco.release();
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}

/**
 * Abstract representation of an ingredient entity.
 */
class Ingredient extends Thread {

	// Identification for specific ingredient
	private int theIngredientID;
	private String name;

	// Semaphores
	private int[] semaphoreIndex;
	private Semaphore[] ingredientArray;
	private Semaphore mutex;
	private Semaphore theIngredient;

	/**
	 * Constructor for Ingredient object.
	 * @param   title - Name of the ingredient
	 * @param   ingredient - Semaphore for ingredient
	 * @param   semaphoreArray - Array of semaphores
	 * @param   m - Mutex
	 * @param   id - Ingredient ID
	 * @param   semIndex - Semaphore index array
	 */
	public Ingredient(String title, Semaphore ingredient, Semaphore[] semaphoreArray, Semaphore m, int id, int[] semIndex) {
		name = title;
		theIngredient = ingredient;
		ingredientArray = semaphoreArray;
		mutex = m;
		theIngredientID = id;
		semaphoreIndex = semIndex;
	}

	/**
	 * Method to reset semaphore array
	 * @param theSemaphoreArray - The array to flush
	 */
	public static void flushArray(Semaphore[] theSemaphoreArray) {
		for(int i = 0; i < theSemaphoreArray.length; i++) {
			int j = theSemaphoreArray[i].availablePermits();
			if (j > 2) {
				theSemaphoreArray[i].drainPermits();
			}
		}
	}

	/**
	 * Method to simulate interacting threads as ingredients.
	 */
	public void run() {
		while (true) {
			try {
				// Check ingredients placed and flush array
				theIngredient.acquire();
				flushArray(ingredientArray);
				System.out.println(name + " is being taken");

				// Increment array let next ingredient to be used
				mutex.acquire();
				semaphoreIndex[0] = semaphoreIndex[0] + theIngredientID;
				ingredientArray[semaphoreIndex[0]-1].release();
				mutex.release();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}

/**
 * Abstract representation of a smoker entity.
 */
class Smokers extends Thread {

		// Attributes for identifying specific smoker
		private String name;
		private int[] semaphoreIndex;

		// Semaphores
		private Semaphore choice;
		private Semaphore ingredient;

		/**
		 * Constructor for Smoker objects
		 * @param   title - Name of smoker
		 * @param   theChoice - Decision/selection smoker makes
		 * @param   semaphoreIndex - Specific selection in semaphoreArray
		 * @param   semaphoreArray - Array of semaphores
		 * @param   theIngredient - Specific smoker ingredient
		 */
		public Smokers(String title, Semaphore theChoice, Semaphore[] semaphoreIndex, int[] semaphoreArray, Semaphore theIngredient) {
			name = title;
			choice = theChoice;
			this.semaphoreIndex = semaphoreArray;
			ingredient = theIngredient;
		}

		/**
		 * Method to simulate interacting threads as smokers.
		 */
		public void run() {
			while (true) {

				// Attempt to smoke when the semaphore is given to specific smoker
				try {
					ingredient.acquire();
					} catch (InterruptedException e1) {
						e1.printStackTrace();
					}

					// Smoke, then let agent start a new round
					semaphoreIndex[0] = 0;
					System.out.println("###########################");
					System.out.println(name + " makes cigarette...");
					System.out.println(name + " is smoking!");
					System.out.println(name + " finishes smoking!");
					choice.release();
					try {
						sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
			}
		}

}

/**
 * Abstract representation of cigarette smokers problem.
 */
public class CigaretteSmokers {

	// Initialize agent entity
	private Agent theAgent;

	// Initialize semaphoreIndex to manage semaphore array
	private int[] semaphoreIndex;

	// Initialize 3 smokers with infinite resources of two ingredients
	private Smokers smoker1;
	private Smokers smoker2;
	private Smokers smoker3;

	// Initialize 3 ingredients for selection
	private Ingredient tobaccoIngredient;
	private Ingredient paperIngredient;
	private Ingredient matchesIngredient;

	// Initialize semaphores for the attributes initalized above
	private Semaphore[] array = new Semaphore[6];
	private Semaphore tobacco;
	private Semaphore paper;
	private Semaphore matches;
	private Semaphore mutex;
	private Semaphore chooses;

	/**
	 * Constructor for CigaretteSmokers problem
	 */
	public CigaretteSmokers() {

		// Set array and indexing
		semaphoreIndex = new int[1];
		semaphoreIndex[0] = 0;

		for(int i = 0; i < array.length; i++) {
			array[i] = new Semaphore(0);
		}

		// Set semaphores
		mutex = new Semaphore(1);
		chooses = new Semaphore(1);
		tobacco = new Semaphore(0);
		paper = new Semaphore(0);
		matches = new Semaphore(0);

		// Create an agent, the three ingredients, and three smokers
		theAgent = new Agent(chooses, tobacco, paper, matches);
		tobaccoIngredient = new Ingredient ("Tobacco", tobacco, array, mutex, 4, semaphoreIndex);
		paperIngredient = new Ingredient ("Paper", paper, array, mutex, 2, semaphoreIndex);
		matchesIngredient = new Ingredient ("Matches", matches, array, mutex, 1, semaphoreIndex);

		// Smokers passed unique semaphore element to simulate order through available resources
		smoker1 = new Smokers("Smoker1", chooses, array, semaphoreIndex, array[2]);
		smoker2 = new Smokers("Smoker2", chooses, array, semaphoreIndex, array[5]);
		smoker3 = new Smokers("Smoker3", chooses, array, semaphoreIndex, array[4]);

		// Start execution
		theAgent.start();
		tobaccoIngredient.start();
		paperIngredient.start();
		matchesIngredient.start();
		smoker1.start();
		smoker2.start();
		smoker3.start();
	}

	// Function to create new problem instance
	public static void main(String[] args) {
		new CigaretteSmokers();
	}
}
