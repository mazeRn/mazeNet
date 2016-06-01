package client;

import client.AIMain;
import generated.LoginReplyMessageType;
import generated.MazeCom;
import networkingClient.ServerConnection;

public class LoginHandler_old {

	private static LoginHandler_old instance = new LoginHandler_old();

	public static LoginHandler_old getInstance() {
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