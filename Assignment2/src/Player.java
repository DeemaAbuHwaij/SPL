package bguspl.set.ex;

import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.Level;

import bguspl.set.Env;
//import com.sun.org.apache.xerces.internal.impl.dv.dtd.ENTITYDatatypeValidator;
//import javafx.scene.layout.Priority;
//import javafx.util.converter.IntegerStringConverter;

/**
 * This class manages the players' threads and data
 *
 * @inv id >= 0
 * @inv score >= 0
 */
public class Player implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;

    /**
     * The id of the player (starting from 0).
     */
    public final int id;

    /**
     * The thread representing the current player.
     */
    private Thread playerThread;

    /**
     * The thread of the AI (computer) player (an additional thread used to generate key presses).
     */
    private Thread aiThread;

    /**
     * True iff the player is human (not a computer player).
     */
    private final boolean human;

    /**
     * True iff game should be terminated due to an external event.
     */
    public volatile boolean terminate;

    /**
     * The current score of the player.
     */
    private int score;
    public BlockingQueue<Integer> playerPressedKey; // the slots of the keys that the player pressed

    public volatile boolean dealersResponse;

    public volatile boolean dealerHasResponded;

    public volatile boolean asleep;


    /**
     * The class constructor.
     *
     * @param env    - the environment object.
     * @param dealer - the dealer object.
     * @param table  - the table object.
     * @param id     - the id of the player.
     * @param human  - true iff the player is a human player (i.e. input is provided manually, via the keyboard).
     */
    public Player(Env env, Dealer dealer, Table table, int id, boolean human ) {
        this.env = env;
        this.table = table;
        this.id = id;
        this.human = human;
        playerPressedKey = new LinkedBlockingQueue<>();
        dealersResponse = false;
        dealerHasResponded = false;
        asleep = false;
    }

    /**
     * The main player thread of each player starts here (main loop for the player thread).
     */
    @Override
    public void run() {
        playerThread = Thread.currentThread();
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + "starting.");
        if (!human) createArtificialIntelligence();
        while (!terminate) {
            if(dealerHasResponded) {
                if (dealersResponse)
                    point();
                else
                    penalty();
                dealerHasResponded = false;
                dealersResponse = false;
            }
        }
        if (!human) try { aiThread.join(); } catch (InterruptedException ignored) {}
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * Creates an additional thread for an AI (computer) player. The main loop of this thread repeatedly generates
     * key presses. If the queue of key presses is full, the thread waits until it is not full.
     */
    private void createArtificialIntelligence() {
        // note: this is a very very smart AI (!)
        aiThread = new Thread(() -> {
            env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
            while (!terminate) {
                while (!table.isIn) {}

                List<Integer> fullSlots = new LinkedList<>();
                for (int i = 0; i < table.slotToCard.length; i++)
                    if (table.slotToCard[i] != null)
                        fullSlots.add(i);

                Random r = new Random();


                int slotChoose1 = r.nextInt(fullSlots.size());
                int slot1 = fullSlots.remove(slotChoose1);
                while(table.slotToCard[slot1] == null){}
                int card1 = table.slotToCard[slot1];
                keyPressed(slot1);

                int slotChoose2 = r.nextInt(fullSlots.size());
                int slot2 = fullSlots.remove(slotChoose2);
                while(table.slotToCard[slot2] == null){}
                int card2 = table.slotToCard[slot2];
                keyPressed(slot2);

                int slotChoose3 = r.nextInt(fullSlots.size());
                int slot3 = fullSlots.remove(slotChoose3);
                while(table.slotToCard[slot3] == null){}
                int card3 = table.slotToCard[slot3];
                keyPressed(slot3);

                int[] cards = {card1, card2, card3};

                if (!env.util.testSet(cards)) {
                    if (table.slotToPlayers.get(slot1).containsKey(id))
                        keyPressed(slot1);

                    if (table.slotToPlayers.get(slot2).containsKey(id))
                        keyPressed(slot2);

                    if (table.slotToPlayers.get(slot3).containsKey(id))
                        keyPressed(slot3);

                }
            }
            env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
        }, "computer-" + id);
        aiThread.start();
    }

    /**
     * Called when the game should be terminated due to an external event.
     */
    public void terminate() {
        terminate = true;

    }

    /**
     * This method is called when a key is pressed.
     *
     * @param slot - the slot corresponding to the key pressed.
     */
    public void keyPressed(int slot) {
        synchronized (table) {
            if (!table.playerToTokens.containsKey(id)) // the first time the player pressed on the key
                table.playerToTokens.put(id, new ConcurrentHashMap<>());

            if (asleep || !table.isIn)
                return;

            if (table.playerToTokens.get(id).size() == 3 && !table.playerToTokens.get(id).contains(slot))
                return;

            if (table.playerToTokens.get(id).size() <= 3) {
                if (!table.playerToTokens.get(id).contains(slot))
                    table.placeToken(id, slot);
                else
                    table.removeToken(id, slot);
            }
            synchronized (this) {
                if (table.playerToTokens.get(id).size() == 3) {
                    table.playersQ.add(id);
                    synchronized (table.playersQ){table.playersQ.notifyAll();} // wake dealer up
                    try {
                        if(!table.isIn)
                            return;
                        wait();
                    } catch (InterruptedException ignored) {}
                }
            }
        }

    }
    /**
     * Award a point to a player and perform other related actions.
     *
     * @post - the player's score is increased by 1.
     * @post - the player's score is updated in the ui.
     */
    public void point() {
        int ignored = table.countCards(); // this part is just for demonstration in the unit tests
        env.ui.setScore(id, ++score);

        asleep = true;
        long freezeTime = env.config.pointFreezeMillis;
        while(freezeTime > 0) {
            env.ui.setFreeze(id, freezeTime);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ignored1) {}
            freezeTime -= 1000;
        }
        env.ui.setFreeze(id, 0);
        asleep = false;
    }

    /**
     * Penalize a player and perform other related actions.
     */
    public void penalty() {
        asleep = true;
        long freezeTime = env.config.penaltyFreezeMillis;
        while(freezeTime > 0) {
            env.ui.setFreeze(id, freezeTime);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ignored) {}
            freezeTime -= 1000;
        }
        env.ui.setFreeze(id, 0);
        asleep = false;
    }

    public int Score() {
        return score;
    }
}
