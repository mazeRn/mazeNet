package client;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import generated.AwaitMoveMessageType;
import generated.BoardType;
import generated.BoardType.Row;
import generated.CardType;
import generated.MoveMessageType;
import generated.PositionType;
import generated.TreasureType;
import generated.TreasuresToGoType;

public class ImplementationKI implements InterfaceKI {

	private static int[][] allShiftPos = { { 0, 1 }, { 0, 3 }, { 0, 5 },
			{ 1, 0 }, { 3, 0 }, { 5, 0 }, { 1, 6 }, { 3, 6 }, { 5, 6 },
			{ 6, 1 }, { 6, 3 }, { 6, 5 } };

	public ImplementationKI(int playerid) {
		this.playerid = playerid;
	}

	private int playerid;
	private List<BoardType.Row> board;
	private PositionType fordbidden;
	private CardType card;
	private List<TreasuresToGoType> treasuresToGo;
	private TreasureType target;
	private PositionType targetpos;

	// best new attributes
	private PositionType pinpos;
	private CardType shiftcard;
	private PositionType shiftpos;

	@Override
	public MoveMessageType getMove(AwaitMoveMessageType status) {
		this.board = status.getBoard().getRow();
		this.fordbidden = status.getBoard().getForbidden();
		this.card = status.getBoard().getShiftCard();
		this.treasuresToGo = status.getTreasuresToGo();
		this.target = status.getTreasure();
		setTarget();

		calculateBestMove();

		MoveMessageType ret = new MoveMessageType();
		ret.setNewPinPos(pinpos);
		ret.setShiftCard(shiftcard);
		ret.setShiftPosition(shiftpos);
		return ret;
	}

	private void calculateBestMove() {

		// interate all moves

		PositionType tmppinpos;
		CardType tmpshiftcard = this.card;
		PositionType tmpshiftpos = new PositionType();
		List<BoardType.Row> tmpBoard;

		// iterate openings
		for (int opening = 0; opening < 4; opening++) {

			// iterate card input position
			for (int[] pos : this.allShiftPos) {
				if (this.fordbidden.getCol() == pos[1]
						&& this.fordbidden.getRow() == pos[0]) {
					continue;
				}
				tmpshiftpos.setRow(pos[0]);
				tmpshiftpos.setCol(pos[1]);

				// iterate movements
				tmpBoard = changeBoard(tmpshiftcard, tmpshiftpos);

				for (PositionType possiblePos : iterateMove(tmpBoard,
						getPos(tmpBoard))) {
					rateMove(tmpBoard, possiblePos);
				}

			}

			shiftCard(tmpshiftcard);
		}

	}

	private List<BoardType.Row> changeBoard(CardType tmpshiftcard,
			PositionType tmpshiftpos) {
		List<BoardType.Row> ret = new ArrayList<BoardType.Row>();
		Collections.copy(ret, this.board);
		int row = tmpshiftpos.getRow();
		int col = tmpshiftpos.getCol();

		// put card into row
		if (row == 1 || row == 3 || row == 5) {
			if (col == 0) {
				ret.get(row).getCol().add(0, tmpshiftcard);
				CardType dropCard = ret.get(row).getCol().remove(7);
				if (dropCard.getPin() != null
						&& dropCard.getPin().getPlayerID().size() > 0) {
					tmpshiftcard.getPin().getPlayerID()
							.addAll(dropCard.getPin().getPlayerID());
				}
			} else {
				CardType dropCard = ret.get(row).getCol().remove(0);
				ret.get(row).getCol().add(6, tmpshiftcard);
				if (dropCard.getPin() != null
						&& dropCard.getPin().getPlayerID().size() > 0) {
					tmpshiftcard.getPin().getPlayerID()
							.addAll(dropCard.getPin().getPlayerID());
				}
			}
		} else {
			if (row == 0) {
				CardType dropCard = ret.get(6).getCol().get(col);
				if (dropCard.getPin() != null
						&& dropCard.getPin().getPlayerID().size() > 0) {
					tmpshiftcard.getPin().getPlayerID()
							.addAll(dropCard.getPin().getPlayerID());
				}
				for (int i = 6; i > 0; i--) {
					ret.get(i - 1).getCol()
							.set(col, ret.get(i).getCol().get(col));
				}
				ret.get(0).getCol().set(col, tmpshiftcard);

			} else {
				CardType dropCard = ret.get(0).getCol().get(col);
				if (dropCard.getPin() != null
						&& dropCard.getPin().getPlayerID().size() > 0) {
					tmpshiftcard.getPin().getPlayerID()
							.addAll(dropCard.getPin().getPlayerID());
				}
				for (int i = 6; i > 0; i--) {
					ret.get(i - 1).getCol()
							.set(col, ret.get(i).getCol().get(col));
				}
				ret.get(0).getCol().set(col, tmpshiftcard);
			}
		}

		return ret;
	}

	private void shiftCard(CardType card) {
		CardType.Openings openings = card.getOpenings();
		boolean tmpopening = openings.isRight();
		openings.setRight(openings.isTop());
		openings.setTop(openings.isLeft());
		openings.setLeft(openings.isBottom());
		openings.setBottom(tmpopening);
	}

	private List<PositionType> iterateMove(List<BoardType.Row> board,
			PositionType pos) {
		List<PositionType> ret = new ArrayList<PositionType>();
		CardType posCard = getPosCard(board, pos);
		ret.add(pos);
		PositionType newPos = new PositionType();
		newPos.setCol(pos.getCol());
		newPos.setRow(pos.getRow());

		if (posCard.getOpenings().isBottom() && pos.getRow() != 6) {
			newPos.setRow(pos.getRow() + 1);

			position: for (PositionType newpossiblePos : iterateMove(board, newPos)) {
				boolean alreadyadded = false;
				for (PositionType possiblePos : ret) {
					if (possiblePos.getCol() == newpossiblePos.getCol()
							&& possiblePos.getRow() == newpossiblePos.getRow()) {
						continue position;
					}
				}
				if (!alreadyadded) {
					ret.add(newpossiblePos);
				}
			}
		}
		if (posCard.getOpenings().isLeft() && pos.getCol() != 0) {
			newPos.setCol(pos.getCol() - 1);
			position: for (PositionType newpossiblePos : iterateMove(board, newPos)) {
				boolean alreadyadded = false;
				for (PositionType possiblePos : ret) {
					if (possiblePos.getCol() == newpossiblePos.getCol()
							&& possiblePos.getRow() == newpossiblePos.getRow()) {
						continue position;
					}
				}
				if (!alreadyadded) {
					ret.add(newpossiblePos);
				}
			}
		}
		if (posCard.getOpenings().isTop() && pos.getRow() != 0) {
			newPos.setRow(pos.getRow() - 1);
			position: for (PositionType newpossiblePos : iterateMove(board, newPos)) {
				boolean alreadyadded = false;
				for (PositionType possiblePos : ret) {
					if (possiblePos.getCol() == newpossiblePos.getCol()
							&& possiblePos.getRow() == newpossiblePos.getRow()) {
						continue position;
					}
				}
				if (!alreadyadded) {
					ret.add(newpossiblePos);
				}
			}
		}
		if (posCard.getOpenings().isRight() && pos.getCol() != 6) {
			newPos.setCol(pos.getCol() + 1);
			position: for (PositionType newpossiblePos : iterateMove(board, newPos)) {
				boolean alreadyadded = false;
				for (PositionType possiblePos : ret) {
					if (possiblePos.getCol() == newpossiblePos.getCol()
							&& possiblePos.getRow() == newpossiblePos.getRow()) {
						continue position;
					}
				}
				if (!alreadyadded) {
					ret.add(newpossiblePos);
				}
			}
		}

		return ret;

	}

	private CardType getPosCard(List<BoardType.Row> board, PositionType pos) {
		return board.get(pos.getRow()).getCol().get(pos.getCol());
	}

	private void rateMove(List<BoardType.Row> board, PositionType possiblePos) {

	}

	private PositionType getPos(List<Row> board) {
		for (int i = 0; i < board.size(); i++) {
			Row r = board.get(i);
			for (int j = 0; j < r.getCol().size(); j++) {
				CardType card = r.getCol().get(j);
				if (card.getPin().getPlayerID().contains(this.playerid)) {
					PositionType pos = new PositionType();
					pos.setCol(j);
					pos.setRow(i);
					return pos;
				}
			}
		}
		return null;
	}

	private void setTarget() {
		this.targetpos = new PositionType();

		int rowcount = 0;
		int colcount = 0;
		for (BoardType.Row row : this.board) {
			for (CardType card : row.getCol()) {
				if (card.getTreasure().equals(this.target)) {
					this.targetpos.setCol(colcount);
					this.targetpos.setRow(rowcount);
				}
				colcount++;
			}
			colcount = 0;
			rowcount++;
		}
	}

}
