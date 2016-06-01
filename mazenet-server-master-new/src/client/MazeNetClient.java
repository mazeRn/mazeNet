package client;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Scanner;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import client.AIMain;
import config.Settings;
import generated.ObjectFactory;
import networking.MazeComMessageFactory;
import networkingClient.ServerConnection;
import networkingClient.ServerListener;
import server.Game;
import server.userInterface.BetterUI;

public class MazeNetClient {

	/** Official team name */
	public static final String TEAM_NAME = "Fantastic Four";

	private static final Lock lock = new ReentrantLock();
	private static final Condition exitCondition = lock.newCondition();
	private static MazeNetClient instance;
	private static boolean exit = false;
	private static Throwable exitException = null;
	private final Thread serverListenerThread;
	private MazeComMessageFactory messageFactory;
	private ObjectFactory directFactory;
	private ServerConnection connection;

	private MazeNetClient(ServerConnection connection) {
		instance = this;
		messageFactory = new MazeComMessageFactory();
		directFactory = new ObjectFactory();
		this.connection = connection;

		AIMain artificialIntelligence = new AIMain(AIMain.CLEVER_AI, connection);

		ServerListener serverListener = new ServerListener(connection, artificialIntelligence);
		serverListenerThread = new Thread(serverListener);
	}

	public static void main(String[] args) {
		if (args.length < 1) {
			System.err.println("Missing host in command line!");
			System.err.println("  Use java -jar ... host[:port]");
			return;
		} else if (args[0].equalsIgnoreCase("--startServer")) {
			args[0] = "localhost";
			Game.main(new String[] {});
			if (Settings.USERINTERFACE instanceof BetterUI) {
				BetterUI ui = (BetterUI) Settings.USERINTERFACE;
				try {
					Method actionPerformedMethod = ui.getClass().getDeclaredMethod("MIStartActionPerformed", java.awt.event.ActionEvent.class);
					actionPerformedMethod.setAccessible(true);
					actionPerformedMethod.invoke(ui, new Object[] { null });
				} catch (ReflectiveOperationException e) {
					System.err.println("Reflection failed. Not starting internal server");
					e.printStackTrace();
				}
			}
		}

		{
			System.out.println("Press Enter, if server is ready...");
			Scanner systemInScanner = new Scanner(System.in);
			systemInScanner.nextLine();
		}

		MazeNetClient client = null;
		try {
			ServerConnection connection = new ServerConnection(args[0]);
			client = new MazeNetClient(connection);

			client.start();
		} catch (Exception t) {
			unhandledException(t);
		}

		lock.lock();
		try {
			while (!exit) {
				try {
					exitCondition.await();
				} catch (InterruptedException e) {
					exit = true;
					System.err.println("Program interrupt");
				}
			}
		} finally {
			lock.unlock();
		}

		if (client != null) {
			try {
				client.forceStop();
			} catch (IOException ignore) {
			}
		}

		if (exitException != null) {
			System.err.println("Critical exception, stopping client");
			exitException.printStackTrace();
		}
	}

	public static void unhandledException(Throwable t) {
		lock.lock();
		try {
			exitException = t;
			exit = true;
			exitCondition.signal();
		} finally {
			lock.unlock();
		}
	}

	public static MazeNetClient getInstance() {
		return instance;
	}

	public MazeComMessageFactory getMessageFactory() {
		return messageFactory;
	}

	public ObjectFactory getDirectFactory() {
		return directFactory;
	}

	private void start() {
		serverListenerThread.start();
		LoginHandler.getInstance().sendLogin(connection);
	}

	private void forceStop() throws IOException {
		connection.close();
		if (serverListenerThread.isAlive()) {
			serverListenerThread.interrupt();
		}
	}
}