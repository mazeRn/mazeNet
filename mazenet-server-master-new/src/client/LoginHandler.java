package client;

import generated.LoginReplyMessageType;
import generated.MazeCom;
import networkingClient.ServerConnection;


public class LoginHandler {

	LoginHandler handler = new LoginHandler();
	
	public void send(ServerConnection connection){

		MazeCom loginMessage = MazeNetClient.getInstance().getMessageFactory().createLoginMessage(MazeNetClient.TEAM_NAME);
		connection.getServerOutputStream().write(loginMessage);
	}
	
	public void processLoginReply(AIMain aiMain, LoginReplyMessageType reply) {
		aiMain.setOurPlayerId(reply.getNewID());
	}
}
