package bguspl.set.ex;

import bguspl.set.Env;

import java.util.*;
import java.util.logging.Level;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;
    public final Player[] players;

    private Thread[] playersThread;

    public volatile boolean playersStarted;

    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;

    /**
     * True iff game should be terminated due to an external event.
     */
    public volatile boolean terminate;

    private Timer timer;

    /**
     * The time when the dealer needs to reshuffle the deck due to turn timeout.
     */
    private long reshuffleTime = Long.MAX_VALUE;

    public Dealer(Env env, Table table, Player[] players) {
        this.env = env;
        this.table = table;
        this.players = players;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
        playersThread = new Thread[players.length];
    }

    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
        placeCardsOnTable();
        for (int i = 0; i < playersThread.length; i++) {
            playersThread[i] = new Thread(players[i]); // create the player thread
            playersThread[i].start(); // start it
        }
        while (!shouldFinish()) {
            placeCardsOnTable();
            updateTimerDisplay(true);
            table.isIn = true;
            timerLoop();
            removeAllCardsFromTable();
            for (int i = 0; i < players.length; i++) {
                synchronized (players[i]) {
                    players[i].notifyAll();
                }
                players[i].dealerHasResponded = false;
            }
        }
        terminate();
        for (int i = 0; i < playersThread.length; i++) {
            playersThread[i].interrupt();
        }
        announceWinners();
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * The inner loop of the dealer thread that runs as long as the countdown did
     * not time out.
     */
    private void timerLoop() {
        timer = new Timer(env);
        Thread timerThread = new Thread(timer);
        timerThread.start();
        while (!terminate && timer.currTime > 0) { // terminate for the deck if there is legal sets
            // sleepUntilWokenOrTimeout(timer.currTime);
            if (timer.currTime > 0 && !table.playersQ.isEmpty()) {
                int idPlayer = table.playersQ.poll();
                removeCardsFromTable(idPlayer);
                placeCardsOnTable();
                // table.isIn = true;
                synchronized (players[idPlayer]) {
                    players[idPlayer].notifyAll();
                }
            }

        }
        /*
         * try{
         * timerThread.join();
         * }catch(InterruptedException ignored){}
         */
        table.isIn = false;
    }

    /**
     * Called when the game should be terminated due to an external event.
     */
    public void terminate() {
        terminate = true;
        for (Player player : players) {
            player.terminate();
        }
    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private boolean shouldFinish() {
        return terminate || env.util.findSets(deck, 1).size() == 0;
    }

    /**
     * Checks cards should be removed from the table and removes them.
     */
    private void removeCardsFromTable(int idPlayer) { // if there is legal set
        List<Integer> playerTokens = new ArrayList<>(); // the player's token cards
        for (Integer slot : table.playerToTokens.get(idPlayer).keySet())
            playerTokens.add(slot);

        boolean isLegal = checkLegal(idPlayer, playerTokens);
        if (isLegal) {
            players[idPlayer].dealersResponse = true;
            for (Integer slot : playerTokens)
                if (table.slotToCard[slot] != null)
                    table.removeCard(slot);
            updateTimerDisplay(true);
            timer.currTime = env.config.turnTimeoutMillis;
        }
        players[idPlayer].dealerHasResponded = true;
    }

    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {
        List<Integer> index = new ArrayList<Integer>();
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (table.slotToCard[i] == null) {
                index.add(i);
            }
        }
        Collections.shuffle(index);
        Collections.shuffle(deck);
        while (!index.isEmpty() && !deck.isEmpty()) {
            int slot = index.remove(0);
            int card = deck.remove(0);
            table.placeCard(card, slot);
        }
    }

    /**
     * Sleep for a fixed amount of time or until the thread is awakened for some
     * purpose.
     */
    private void sleepUntilWokenOrTimeout(long remainingTime) {
        try {
            synchronized (table.playersQ) {
                table.isIn = true;
                table.playersQ.wait(remainingTime);
                table.isIn = false;
            }
        } catch (InterruptedException e) {
        }
    }

    /**
     * Reset and/or update the countdown and the countdown display.
     */
    private void updateTimerDisplay(boolean reset) {
        if (reset)
            env.ui.setCountdown(env.config.turnTimeoutMillis, false);
        ;
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        List<Integer> indexOfTable = new ArrayList<Integer>();
        for (int i = 0; i < table.slotToCard.length; i++) {
            if (table.slotToCard[i] != null) {
                indexOfTable.add(i);
            }
        }
        Collections.shuffle(indexOfTable);
        while (!indexOfTable.isEmpty()) {
            int slot = indexOfTable.remove(0);
            deck.add(table.slotToCard[slot]);
            table.removeCard(slot);
        }
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() { // just to show the winners when the game is finished
        List<Player> winners = new ArrayList<>();
        int max = findMax();
        for (int i = 0; i < players.length; i++) {
            if (players[i].Score() == max)
                winners.add(players[i]);
        }
        int[] winnersReal = new int[winners.size()];
        for (int i = 0; i < winners.size(); i++) {
            winnersReal[i] = winners.remove(0).id;
        }
        env.ui.announceWinner(winnersReal);
    }

    public int findMax() {
        int max = 0;
        for (int i = 0; i < players.length; i++) {
            if (players[i].Score() > max)
                max = players[i].Score();
        }
        return max;
    }

    public boolean checkLegal(int playedId, List<Integer> playerPressedKey) {
        if (playerPressedKey.size() != 3) {
            synchronized (players[playedId]) {
                players[playedId].notifyAll();
            }
            return false;
        }
        int[] chosenSlots = new int[3]; // tokens slots
        for (int i = 0; i < 3; i++)
            chosenSlots[i] = playerPressedKey.get(i);

        int[] chosenCards = new int[3];

        for (int i = 0; i < chosenSlots.length; i++)
            if (table.slotToCard[chosenSlots[i]] != null)
                chosenCards[i] = table.slotToCard[chosenSlots[i]];
            else
                chosenCards[i] = -1;

        return env.util.testSet(chosenCards);
    }

}
