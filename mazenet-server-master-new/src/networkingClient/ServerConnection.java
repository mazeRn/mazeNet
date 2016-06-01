package networkingClient;

import java.io.IOException;
import java.net.Socket;

import config.Settings;
import networking.XmlInStream;
import networking.XmlOutStream;

/**
 * Manages the server connection and therefor contains the socket.
 */
public class ServerConnection {

	/** if set to true, the connection to the server is being closed or is already closed */
	private boolean closing;
	/** the server connection socket */
	private Socket socket;
	/** the XML object stream from the server */
	private XmlInStream inFromServer;
	/** the XML object stream to the server */
	private XmlOutStream outToServer;

	/**
	 * Establishes server connection to socket string.
	 *
	 * @param socketAddress the server address in the usual format HOST:PORT
	 * @throws IOException if creating a Socket to the address fails
	 */
	public ServerConnection(String socketAddress) throws IOException {
		closing = false;

		int port = Settings.PORT;
		String addr = socketAddress;

		int colonI = addr.indexOf(':');
		if (colonI != -1) {
			addr = addr.substring(0, colonI);
			try {
				port = Integer.parseInt(addr.substring(colonI + 1));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("NumberFormatException, because port could not be parsed: " + addr.substring(colonI + 1), e);
			}
		}

		socket = new Socket(addr, port);

		inFromServer = new XmlInStream(socket.getInputStream());
		outToServer = new XmlOutStream(socket.getOutputStream());
	}

	/**
	 * @return the XmlInStream to read xml messages from the server
	 * @see networking.XmlInStream
	 */
	public XmlInStream getServerInputStream() {
		return inFromServer;
	}

	/**
	 * @return the XmlOutStream to write xml messages to the server
	 * @see networking.XmlOutStream
	 */
	public XmlOutStream getServerOutputStream() {
		return outToServer;
	}

	/**
	 * Stops all streams and the connection.
	 *
	 * @throws IOException when there is a problem with closing any of the entities
	 */
	public void close() throws IOException {
		closing = true;
		outToServer.close();
		inFromServer.close();
		socket.close();
	}

	/**
	 * Indicates, whether the connection to the server is closing or already closed.
	 *
	 * @return true, if we are closing the connection
	 */
	public boolean isClosing() {
		return closing;
	}
}