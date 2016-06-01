package client;

import generated.AwaitMoveMessageType;
import networkingClient.ServerConnection;

/**
 * Runs the AI in a thread, by being called in {@link Runnable#run()}.
 * Passes information to the AI implementation by using {@link #setAwaitMoveMessage(AwaitMoveMessageType)}.
 */
public class AIMain implements Runnable {

	

	/** Clever AI for "production" */
	@SuppressWarnings("unused") public static final IArtificialIntelligence CLEVER_AI = new AI();

	/** the AI implementation */
	private final IArtificialIntelligence impl;
	/** the server connection */
	private final ServerConnection serverConnection;

	/** our player id - set with {@link #setOurPlayerId(int)}. */
	private int ourPlayerId = 0; // ourPlayerId < 1 is invalid

	/**
	 * Initializes AIMain with an ai implementation and the server connection.
	 *
	 * @param impl the AI implementation
	 * @param serverConnection the server connection
	 */
	public AIMain(IArtificialIntelligence impl, ServerConnection serverConnection) {
		this.impl = impl;
		this.serverConnection = serverConnection;
	}

	/**
	 * @see AIMain
	 */
	@Override public void run() {
		impl.awaitMove(serverConnection, ourPlayerId);
	}

	/**
	 * Initializes the run of the AI implementation by passing the information contained in {@link AwaitMoveMessageType}.
	 *
	 * @param awaitMoveMessage the XML type {@link AwaitMoveMessageType} from the server
	 * @see AIMain
	 */
	public void setAwaitMoveMessage(AwaitMoveMessageType awaitMoveMessage) {
		impl.setAwaitMoveMessage(awaitMoveMessage);
	}

	/**
	 * Sets our player ourPlayerId. Can only be assigned once.
	 *
	 * @param ourPlayerId should be our player ourPlayerId
	 */
	public void setOurPlayerId(int ourPlayerId) {
		// only assign our player id once
		if (this.ourPlayerId == 0) {
			this.ourPlayerId = ourPlayerId;
		}
	}
}