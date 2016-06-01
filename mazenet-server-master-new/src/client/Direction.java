package client;

import server.Position;

/**
 * Enumeration for the different directions.
 */
public enum Direction {
	// order important for rotating (counter-clockwise)
	UP(0, -1),
	LEFT(-1, 0),
	DOWN(0, 1),
	RIGHT(1, 0);

	/** the relative x offset */
	private int x;
	/** the relative y offset */
	private int y;

	Direction(int x, int y) {
		this.x = x;
		this.y = y;
	}

	/**
	 * @return the opposite direction
	 */
	public Direction getOpposite() {
		switch (this) {
			case UP:
				return DOWN;
			case DOWN:
				return UP;
			case LEFT:
				return RIGHT;
			case RIGHT:
				return LEFT;
			default:
				throw new RuntimeException("Unknown direction " + this);
		}
	}

	/**
	 * Adds the relative direction on the given position.
	 *
	 * @param in the absolute position
	 * @return a new position that is relative next to "in" by this direction
	 */
	public Position addPosition(Position in) {
		return new Position(in.getRow() + y, in.getCol() + x);
	}
}