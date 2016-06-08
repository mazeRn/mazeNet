package clientPackages.networking;

import generated.ErrorType;

import generated.LoginMessageType;
import generated.MazeCom;
import generated.MazeComType;
import generated.MoveMessageType;
import generated.ObjectFactory;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import clientPackages.client.ImplementationKI;
import clientPackages.client.InterfaceKI;
import clientPackages.client.Player;

public class Connection {

	private Socket socket;
	private XmlInStream inFromServer;
	private XmlOutStream outToServer;
	private ObjectFactory objectFactory;
	private final String name = "Jon_Snow_Dies_S5E10";
	private int id;
	private InterfaceKI iKI;

	public Connection(String ip, int port) throws UnknownHostException, IOException {
		this.objectFactory = new ObjectFactory();
		this.socket = new Socket(ip, port);
		try {
			this.inFromServer = new XmlInStream(this.socket.getInputStream());
		} catch (IOException e) {

		}
		
		try {
			this.outToServer = new XmlOutStream(this.socket.getOutputStream());
		} catch (IOException e) {

		}
		login(name);
	}

	public void sendMessage(MazeCom mc) {
		this.outToServer.write(mc);
	}

	public MazeCom receiveMessage() {
		MazeCom result = null;
		try {
			result = this.inFromServer.readMazeCom();
		} catch (IOException e) {

		}
		return result;
	}

	public void login(String name) {
		LoginMessageType loginMessage = objectFactory.createLoginMessageType();
		loginMessage.setName(name);
		MazeCom mc = objectFactory.createMazeCom();
		mc.setLoginMessage(loginMessage);
		mc.setMcType(MazeComType.LOGIN);
		sendMessage(mc);
		awaitMessage();
	}

	public boolean awaitMessage() {
		MazeCom mc = this.receiveMessage();

		switch (mc.getMcType().value()) {

		case "LOGIN":
			throw new RuntimeException("Received unexpected LOGIN Message from Server");
		case "LOGINREPLY":
			this.id = mc.getLoginReplyMessage().getNewID();
			this.iKI = new ImplementationKI(this.id);
			break;
		case "AWAITMOVE":
			MazeCom mc2 = objectFactory.createMazeCom();
			MoveMessageType moveMessage = iKI.getMove(mc.getAwaitMoveMessage());
			mc2.setMcType(MazeComType.MOVE);
			mc2.setMoveMessage(moveMessage);
			sendMessage(mc2);
			break;
		case "MOVE":
			throw new RuntimeException("Received unexpected MOVE Message from Server");
		case "ACCEPT":
			if (mc.getAcceptMessage().isAccept()) {

			} else {
				ErrorType errorCode = mc.getAcceptMessage().getErrorCode();
				switch (errorCode.value()) {
				case "NOERROR":throw new RuntimeException();
				case "ERROR":throw new RuntimeException();
				case "AWAIT_LOGIN":
					login(name);break;
				case "ILLEGAL_MOVE":
					awaitMessage();
					break;
				case "TIMEOUT":throw new RuntimeException();
				case "TOO_MANY_TRIES":throw new RuntimeException();
				default:throw new RuntimeException();
				}
			}
			break;
		case "WIN":
			if (mc.getWinMessage().getWinner().getId() == id){
				System.out.println("Wie erwartet gewonnen.");
			}else {
				System.out.println("Winner ist: "+mc.getWinMessage().getWinner().getValue());
			}return true;
		case "DISCONNECT":
			System.out.println("Disconnected");
			return true;
		default:
			throw new RuntimeException();
		}
		return false;
	}

}
