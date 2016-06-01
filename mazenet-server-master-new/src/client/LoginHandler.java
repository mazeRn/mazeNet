package client;

import client.AIMain;
import generated.LoginReplyMessageType;
import generated.MazeCom;
import networkingClient.ServerConnection;

public class LoginHandler {

	private static LoginHandler instance = new LoginHandler();

	public static LoginHandler getInstance() {
		return instance;
	}

	public void sendLogin(ServerConnection connection) {
		MazeCom loginMessage = MazeNetClient.getInstance().getMessageFactory().createLoginMessage(MazeNetClient.TEAM_NAME);
		connection.getServerOutputStream().write(loginMessage);
	}

	public void processLoginReply(AIMain aiMain, LoginReplyMessageType loginReplyMessage) {
		aiMain.setOurPlayerId(loginReplyMessage.getNewID());
	}

}