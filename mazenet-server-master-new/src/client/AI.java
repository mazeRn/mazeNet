package client;

import java.util.HashMap;
import java.util.Map;

import client.MazeNetClient;
import client.Card;
import client.CardPositionUtils;
import client.Direction;
import client.PrioritizedLinkedList;
import generated.AwaitMoveMessageType;
import generated.BoardType;
import generated.MazeCom;
import generated.MazeComType;
import generated.MoveMessageType;
import generated.TreasureType;
import networkingClient.ServerConnection;
import server.Position;

class AI implements IArtificialIntelligence {

	public static final boolean DEBUG = false;

	public static final int BOARD_SIZE = 7;

	private long timeSinceAwait = 0;

	private HashMap<Integer, Position> playerMap;
	private HashMap<Position, Card> boardMap;
	private Position currentTreasurePosition;
	private Position forbiddenPosition;
	private Card shiftCard;

	public AI() {
		cleanup();
	}

	private static MazeCom buildMazeComMoveMessage(MoveMessageType moveMessage, int ourPlayerId) {
		assert moveMessage != null;

		MazeCom mc = MazeNetClient.getInstance().getDirectFactory().createMazeCom();

		mc.setMcType(MazeComType.MOVE);
		mc.setId(ourPlayerId);
		mc.setMoveMessage(moveMessage);

		return mc;
	}

	private static MoveMessageType buildMoveMessage(CardPositionUtils.MoveMessageTriple moveMessageTriple) {
		assert moveMessageTriple != null;
		if (moveMessageTriple.pinMovePosition == null || moveMessageTriple.shiftCardPosition == null || moveMessageTriple.shiftCard == null) {
			throw new IllegalArgumentException("At least one attribute of " + moveMessageTriple + " is null!");
		}

		MoveMessageType moveMessage = MazeNetClient.getInstance().getDirectFactory().createMoveMessageType();

		moveMessage.setNewPinPos(moveMessageTriple.pinMovePosition);
		moveMessage.setShiftCard(moveMessageTriple.shiftCard.getCardType());
		moveMessage.setShiftPosition(moveMessageTriple.shiftCardPosition);

		return moveMessage;
	}

	private static HashMap<Integer, Position> getRefreshedPlayerMap(HashMap<Position, Card> boardMap) {
		HashMap<Integer, Position> playerMap = new HashMap<>();

		for (Map.Entry<Position, Card> cardEntry : boardMap.entrySet()) {
			for (Integer playerId : cardEntry.getValue().getPlayersOnCard()) {
				playerMap.put(playerId, cardEntry.getKey());
			}
		}

		return playerMap;
	}

	private void cleanup() {
		playerMap = new HashMap<>(4);
		boardMap = new HashMap<>(BOARD_SIZE * BOARD_SIZE);
		currentTreasurePosition = null;
		forbiddenPosition = null;
		shiftCard = null;
	}

	@Override public void setAwaitMoveMessage(AwaitMoveMessageType awaitMoveMessage) {
		timeSinceAwait = System.nanoTime();

		BoardType board = awaitMoveMessage.getBoard();
		TreasureType currentTreasure = awaitMoveMessage.getTreasure();

		if (board.getForbidden() != null) {
			forbiddenPosition = new Position(board.getForbidden());
		}

		for (int y = 0; y < board.getRow().size(); y++) {
			BoardType.Row row = board.getRow().get(y);
			for (int x = 0; x < row.getCol().size(); x++) {
				Position p = new Position(y, x);
				Card c = Card.createInstance(row.getCol().get(x));
				boardMap.put(p, c);

				for (Integer playerId : c.getPlayersOnCard()) {
					playerMap.put(playerId, p);
				}

				if (c.getTreasure() != null && c.getTreasure().equals(currentTreasure)) {
					currentTreasurePosition = p;
				}
			}
		}

		shiftCard = Card.createInstance(board.getShiftCard());
	}

	@Override public void awaitMove(ServerConnection connection, int ourPlayerId) {
		if (!shiftCard.isPlayerOnCard(ourPlayerId) && !playerMap.containsKey(ourPlayerId)) {
			throw new RuntimeException("Could not find our player " + ourPlayerId);
		}

		CardPositionUtils.MoveMessageTriple moveMessageTriple = justGetTheBestMove(ourPlayerId);

		MoveMessageType moveMessage = buildMoveMessage(moveMessageTriple);
		MazeCom message = buildMazeComMoveMessage(moveMessage, ourPlayerId);

		long timeDifference = System.nanoTime() - timeSinceAwait;
		System.out.printf("AI took %.6f s (%d ns)%n", timeDifference / 1e9, timeDifference);

		// send out
		connection.getServerOutputStream().write(message);

		// then clean up
		cleanup();
	}

	private PrioritizedLinkedList<Position> getWeightedShiftPositions(Position playerPosition) {
		PrioritizedLinkedList<Position> weightedShiftPositions = new PrioritizedLinkedList<>();

		for (Position pos : Position.getPossiblePositionsForShiftcard()) {
			if (!pos.equals(forbiddenPosition) && !pos.getOpposite().equals(playerPosition) && !pos.getOpposite().equals(currentTreasurePosition)) {
				Card newShiftCard = boardMap.get(pos.getOpposite());
				int weight = 0;
				switch (newShiftCard.getPiece()) {

					case CORNER_PIECE:
						weight = 3;
						break;
					case STRAIGHT_PIECE:
						weight = 2;
						break;
					case T_PIECE:
						weight = 1;
						break;
				}

				weightedShiftPositions.add(weight, pos);
			}
		}
		return weightedShiftPositions;
	}

	private CardPositionUtils.MoveMessageTriple justGetTheBestMove(int ourPlayerId) {
		CardPositionUtils.MoveMessageTriple moveMessageTriple = new CardPositionUtils.MoveMessageTriple();

		PrioritizedLinkedList<Position> weightedShiftPositions = getWeightedShiftPositions(null);

		boolean playerOnShiftCard = shiftCard.isPlayerOnCard(ourPlayerId);
		int leastDistance = BOARD_SIZE * BOARD_SIZE;
		Position bestOldPlayerPosition = null;
		Position bestShiftPosition = null;
		Position bestPlayerPosition = null;
		Card bestShiftCard = null;
		// this is priority ordered
		for (int i = weightedShiftPositions.size(); i-- > 0; ) {
			Position tempShiftPosition = weightedShiftPositions.get(i);
			Position tempTreasurePosition;
			if (currentTreasurePosition == null) {
				tempTreasurePosition = tempShiftPosition;
			} else {
				tempTreasurePosition = currentTreasurePosition;
			}

			boolean shiftHorizontal = tempShiftPosition.getRow() == 0 || tempShiftPosition.getRow() == 6;
			boolean shiftPositive = tempShiftPosition.getRow() == 0 || tempShiftPosition.getCol() == 0;
			for (int d = 0; d < Direction.values().length; d++) { // rotate three times
				Card tempShiftCard = new Card(shiftCard);
				tempShiftCard.rotate(d);
				HashMap<Integer, Position> tempPlayerMap;
				HashMap<Position, Card> tempBoardMap = new HashMap<>(boardMap);
				Card prevCard = tempBoardMap.get(tempShiftPosition);
				tempBoardMap.put(tempShiftPosition, tempShiftCard);
				for (int j = 1; j < BOARD_SIZE; j++) {
					Position nextPositionInLine;
					if (shiftHorizontal) {
						int row = tempShiftPosition.getRow() + (shiftPositive ? j : -j);
						nextPositionInLine = new Position(row, tempShiftPosition.getCol());
					} else {
						int col = tempShiftPosition.getCol() + (shiftPositive ? j : -j);
						nextPositionInLine = new Position(tempShiftPosition.getRow(), col);
					}

					Card tempPrevCard = tempBoardMap.get(nextPositionInLine);
					tempBoardMap.put(nextPositionInLine, prevCard);
					prevCard = tempPrevCard;
				}

				// now move players
				tempPlayerMap = getRefreshedPlayerMap(tempBoardMap);

				HashMap<Position, Integer> reachables;
				Position tempPlayerPosition;

				// if player is on the shift card, get reachables from the tempShiftPosition
				if (playerOnShiftCard) {
					tempPlayerPosition = tempShiftPosition;
				} else {
					tempPlayerPosition = tempPlayerMap.get(ourPlayerId);
				}

				// check for player not hanging out on shift card
				if (tempPlayerPosition != null) {
					reachables = CardPositionUtils.getReachables(tempBoardMap, tempPlayerPosition);

					if (reachables.containsKey(tempTreasurePosition)) {
						// go to the treasure, if we can reach it
						moveMessageTriple.shiftCard = tempShiftCard;
						moveMessageTriple.shiftCardPosition = tempShiftPosition;
						moveMessageTriple.pinMovePosition = tempTreasurePosition;
						if (DEBUG) {
							System.out.println("Found direct connection!");
							System.out.printf("Player: %s --> %s, Shiftcard: [%s, %s]%n", tempPlayerPosition, moveMessageTriple.pinMovePosition, moveMessageTriple.shiftCard, moveMessageTriple.shiftCardPosition);
						}
						return moveMessageTriple;
					}

					for (Position p : reachables.keySet()) {
						// go to the card with the least distance to the treasure
						if (CardPositionUtils.calculateDistance(tempTreasurePosition, p) < leastDistance) {
							bestOldPlayerPosition = tempPlayerPosition;
							bestPlayerPosition = p;
							bestShiftPosition = tempShiftPosition;
							bestShiftCard = tempShiftCard;

							leastDistance = CardPositionUtils.calculateDistance(tempShiftPosition, p);
						}
					}
				}
			}
		}

		moveMessageTriple.pinMovePosition = bestPlayerPosition;
		moveMessageTriple.shiftCardPosition = bestShiftPosition;
		moveMessageTriple.shiftCard = bestShiftCard;
		if (DEBUG) {
			System.out.printf("Player: %s --> %s, Shiftcard: [%s, %s]%n", bestOldPlayerPosition, moveMessageTriple.pinMovePosition, moveMessageTriple.shiftCard, moveMessageTriple.shiftCardPosition);
		}

		return moveMessageTriple;
	}
}