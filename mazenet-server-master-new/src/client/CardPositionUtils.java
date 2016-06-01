package client;

import java.util.HashMap;
import java.util.Map;

import server.Position;

/**
 * General board utils.
 */
public final class CardPositionUtils {

	private CardPositionUtils() {
	}

	/**
	 * Determines all reachable positions with weight and returns that information.
	 *
	 * @param boardMap a {@link Map} with {@link Position} as key and {@link Card} as value
	 * @param startPos the start position
	 * @return a {@link HashMap} with all reachable {@link Position}s as key and the weight ({@link Integer}) as value
	 */
	public static HashMap<Position, Integer> getReachables(Map<Position, Card> boardMap, Position startPos) {
		HashMap<Position, Integer> reachables = new HashMap<>(boardMap.size());
		getReachablesRecursive(boardMap, reachables, startPos, 0, null);
		return reachables;
	}

	private static void getReachablesRecursive(Map<Position, Card> boardMap, HashMap<Position, Integer> reachables, Position pos, int currentWeight, Direction from) {
		reachables.put(pos, currentWeight);

		int nextWeight = currentWeight + 1;
		Card currentCard = boardMap.get(pos);

		for (Direction dir : Direction.values()) {
			// do not even try the origin direction
			if (!dir.equals(from)) {
				Position nextPos = dir.addPosition(pos);
				// check, if board has that position
				if (boardMap.containsKey(nextPos)) {
					Card nextCard = boardMap.get(nextPos);
					// check, whether cards can connect
					if (currentCard.canConnect(nextCard, dir)) {
						// check for existing entry with smaller or equal weight
						if (!reachables.containsKey(nextPos) || reachables.get(nextPos) > nextWeight) {
							getReachablesRecursive(boardMap, reachables, nextPos, nextWeight, dir.getOpposite());
						}
					}
				}
			}
		}
	}

	/**
	 * Calculates the square distance between two positions.
	 *
	 * @param pos1 the first {@link Position}
	 * @param pos2 the second {@link Position}
	 * @return the distance as {@link Integer}
	 */
	public static int calculateDistance(Position pos1, Position pos2) {
		return Math.abs(pos1.getCol() - pos2.getCol()) + Math.abs(pos1.getRow() - pos2.getRow());
	}

	/**
	 * Data class that holds all relevant information for a {@link generated.MoveMessageType}.
	 */
	public static class MoveMessageTriple {

		public Position pinMovePosition;
		public Position shiftCardPosition;
		public Card shiftCard;

		@Override public String toString() {
			return String.format("MoveMessageTriple[pinMovePosition: %s, shiftCardPosition: %s, shiftCard: %s]", pinMovePosition, shiftCardPosition, shiftCard);
		}
	}
}