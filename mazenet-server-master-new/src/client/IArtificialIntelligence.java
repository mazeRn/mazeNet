package client;

import generated.AwaitMoveMessageType;
import networkingClient.ServerConnection;

/**
 * Interface for implementing AI classes.
 */
public interface IArtificialIntelligence {

	/**
	 * {@link AwaitMoveMessageType} contains all the data from the XML that is being sent by the server.
	 * Save the information locally in the AI class and do the actual work on {@link #awaitMove(ServerConnection, int)}.
	 *
	 * @param awaitMoveMessage data coming from the MazeNet server
	 */
	void setAwaitMoveMessage(AwaitMoveMessageType awaitMoveMessage);

	/**
	 * Is called in an AI worker thread and should send the server a {@link generated.MoveMessageType} message by using the XML output stream.
	 *
	 * @param connection the server connection to reply to
	 * @param id our player id
	 * @see networking.ServerConnection#getServerOutputStream()
	 */
	void awaitMove(ServerConnection connection, int id);
}