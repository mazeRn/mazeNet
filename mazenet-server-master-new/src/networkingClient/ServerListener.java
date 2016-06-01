package networkingClient;

import javax.xml.bind.UnmarshalException;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import client.LoginHandler;
import client.MazeNetClient;
import client.AIMain;
import generated.MazeCom;

/**
 * Contains all the methods to handle incoming messages and pass them to the AI executor, which executes AI tasks in an extra thread. The ServerListener itself is implemented as a {@link Runnable} and should be run in a single {@link Thread}.
 */
public class ServerListener implements Runnable {

	private final ServerConnection connection;
	private final AIMain aiMain;
	private ExecutorService aiExecutor;

	/**
	 * Instantiates a new server listener that also creates a new AI executor.
	 *
	 * @param connection the server connection
	 * @param aiMain the {@link AIMain} to pass AI tasks on
	 */
	public ServerListener(ServerConnection connection, AIMain aiMain) {
		this.connection = connection;
		this.aiMain = aiMain;
		aiExecutor = Executors.newSingleThreadExecutor();
	}

	/**
	 * Runs as long as the server connection is alive. Shuts down the AI executor when finished.
	 */
	@Override public void run() {
		try {
			listenLoop:
			while (true) {
				final MazeCom mazeCom = connection.getServerInputStream().readMazeCom();
				MessageResult result = handleMessage(mazeCom);
				switch (result) {
					case OK:
						// nothing
						break;
					case DISCONNECT:
						connection.close();
						break listenLoop;
					case INVALID_MOVE:
						throw new InvalidMoveException();
					default:
					case ERROR:
						throw new InvalidMessageException(mazeCom);
				}
			}
		} catch (UnmarshalException e) {
			RuntimeException e2 = new RuntimeException("Error when reading XML from server!", e);
			MazeNetClient.unhandledException(e2);
		} catch (IOException e) {
			if (!connection.isClosing()) {
				MazeNetClient.unhandledException(e);
			}
		} catch (Exception e) {
			MazeNetClient.unhandledException(e);
		} finally {
			// shutdown executor service
			aiExecutor.shutdownNow();
		}
	}

	private MessageResult handleMessage(MazeCom mazeComMessage) {
		switch (mazeComMessage.getMcType()) {
			case LOGINREPLY:
				LoginHandler.getInstance().processLoginReply(aiMain, mazeComMessage.getLoginReplyMessage());
				return MessageResult.OK;
			case AWAITMOVE:
				aiMain.setAwaitMoveMessage(mazeComMessage.getAwaitMoveMessage());
				aiExecutor.execute(aiMain);
				return MessageResult.OK;
			case ACCEPT:
				if (mazeComMessage.getAcceptMessage().isAccept()) {
					return MessageResult.OK;
				} else {
					return MessageResult.INVALID_MOVE;
				}
			case WIN:
			case DISCONNECT:
				return MessageResult.DISCONNECT;
			case MOVE:
			case LOGIN:
			default:
				return MessageResult.ERROR;
		}
	}

	private enum MessageResult {
		OK,
		INVALID_MOVE,
		ERROR,
		DISCONNECT,
	}

	private class InvalidMessageException extends RuntimeException {

		public InvalidMessageException(MazeCom mazeComMessage) {
			super("Invalid message type " + mazeComMessage.getMcType().value());
		}
	}

	private class InvalidMoveException extends RuntimeException {

	}
}