package client;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import client.MazeNetClient;
import generated.CardType;
import generated.TreasureType;

/**
 * The Card class that is used in our algorithm.
 * It can be created from it's factory method {@link #createInstance(CardType)} with a XML {@link generated.CardType}.
 */
public class Card {

	/** Contains the player pins, if there are any. */
	private CardType.Pin pins;
	/** Contains the treasure, if there is any. */
	private TreasureType treasure;
	/** Describes the type of this card. */
	private CardPiece piece = null;
	/** Holds the open directions of this card. Can be rotated with {@link #rotate(int)}. */
	private Direction[] openDirections = new Direction[0];

	/**
	 * Can only be instantiated by the factory method {@link #createInstance}.
	 */
	private Card() {
	}

	/**
	 * Copy constructor. Does not copy attributes.
	 */
	public Card(Card other) {
		this.pins = other.pins;
		this.treasure = other.treasure;
		this.piece = other.piece;
		this.openDirections = Arrays.copyOf(other.openDirections, other.openDirections.length);
	}

	/**
	 * Creates a new card from a {@link generated.CardType}.
	 *
	 * @param cardType the XML version of a card
	 * @return a new instance of {@link Card}
	 */
	public static Card createInstance(generated.CardType cardType) {
		Card c = new Card();

		c.pins = cardType.getPin();
		c.treasure = cardType.getTreasure();

		List<Direction> openDirections = new ArrayList<>(4);
		if (cardType.getOpenings().isTop()) {
			openDirections.add(Direction.UP);
		}
		if (cardType.getOpenings().isBottom()) {
			openDirections.add(Direction.DOWN);
		}
		if (cardType.getOpenings().isLeft()) {
			openDirections.add(Direction.LEFT);
		}
		if (cardType.getOpenings().isRight()) {
			openDirections.add(Direction.RIGHT);
		}

		c.openDirections = openDirections.toArray(new Direction[openDirections.size()]);

		if (openDirections.size() == 3) {
			c.piece = CardPiece.T_PIECE;
		} else if (openDirections.size() == 2) {
			if (openDirections.get(0).getOpposite().equals(openDirections.get(1))) {
				c.piece = CardPiece.STRAIGHT_PIECE;
			} else {
				c.piece = CardPiece.CORNER_PIECE;
			}
		} else {
			throw new RuntimeException("Unknown card piece " + c);
		}

		return c;
	}

	/**
	 * Rotates the card counter-clockwise. Uses the {@link Direction} class.
	 *
	 * @param iterations where 1 means 90°, 2 => 180°, ...
	 */
	public void rotate(int iterations) {
		for (int i = 0; i < openDirections.length; ++i) {
			openDirections[i] = Direction.values()[(openDirections[i].ordinal() + iterations) % 4];
		}
	}

	/**
	 * @param playerId the player id
	 * @return true, if playerId is on the card
	 */
	public boolean isPlayerOnCard(int playerId) {
		return pins.getPlayerID().contains(playerId);
	}

	/**
	 * Copies the list of players on the card as {@link Integer} array.
	 *
	 * @return all player ids on the card
	 */
	public Integer[] getPlayersOnCard() {
		return pins.getPlayerID().toArray(new Integer[pins.getPlayerID().size()]);
	}

	/**
	 * Removes the player from the card, if it exists. Does not throw, if player does not exist on the card.
	 *
	 * @param playerId the player id
	 */
	public void removePlayerFromCard(int playerId) {
		if (isPlayerOnCard(playerId)) {
			pins.getPlayerID().remove(Integer.valueOf(playerId));
		}
	}

	/**
	 * Adds the player to the card, if it is not already on it. Does not throw, if the player is already on the card. Also does not check, if the player id is valid.
	 *
	 * @param playerId the player id
	 */
	public void addPlayerToCard(int playerId) {
		if (!isPlayerOnCard(playerId)) {
			pins.getPlayerID().add(playerId);
		}
	}

	/**
	 * @return returns the {@link CardType} for XML
	 */
	public CardType getCardType() {
		CardType c = MazeNetClient.getInstance().getDirectFactory().createCardType();

		CardType.Openings openings = MazeNetClient.getInstance().getDirectFactory().createCardTypeOpenings();
		openings.setBottom(false);
		openings.setLeft(false);
		openings.setRight(false);
		openings.setTop(false);

		for (Direction dir : openDirections) {
			switch (dir) {
				case UP:
					openings.setTop(true);
					break;
				case LEFT:
					openings.setLeft(true);
					break;
				case DOWN:
					openings.setBottom(true);
					break;
				case RIGHT:
					openings.setRight(true);
					break;
				default:
					throw new RuntimeException("Unknown direction " + dir);
			}
		}

		c.setOpenings(openings);
		c.setPin(pins);
		c.setTreasure(treasure);

		return c;
	}

	/**
	 * @return the card piece
	 */
	public CardPiece getPiece() {
		return piece;
	}

	/**
	 * @return the treasure on this card
	 */
	public TreasureType getTreasure() {
		return treasure;
	}

	/**
	 * @param dir the direction, which is checked
	 * @return true, if this card has an open end in that direction
	 */
	public boolean isOpen(Direction dir) {
		for (Direction checkDir : openDirections) {
			if (checkDir.equals(dir)) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Determines, if both cards are open on the given direction.
	 *
	 * @param other the other card next to this
	 * @param dir the direction
	 * @return true, if the cards have open ends between each other
	 */
	public boolean canConnect(Card other, Direction dir) {
		return this.isOpen(dir) && other.isOpen(dir.getOpposite());
	}

	/**
	 * @return A well debuggable {@link String}
	 */
	@Override public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("Card[");

		if (piece == null) {
			sb.append("Unknown piece");
		} else {
			sb.append(piece.toString());
		}
		sb.append("; ");

		boolean first = true;
		for (Direction dir : openDirections) {
			if (!first) {
				sb.append(", ");
			} else {
				first = false;
			}
			sb.append(dir.toString());
		}

		sb.append(']');
		return sb.toString();
	}

	/**
	 * Contains all different card pieces from the game.
	 */
	public enum CardPiece {
		CORNER_PIECE,
		STRAIGHT_PIECE,
		T_PIECE,
	}
}