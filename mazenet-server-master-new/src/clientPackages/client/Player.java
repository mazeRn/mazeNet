package clientPackages.client;

import generated.AwaitMoveMessageType;
import generated.MoveMessageType;
import generated.PositionType;

public class Player implements InterfaceKI {

	@Override
	public MoveMessageType getMove(AwaitMoveMessageType status) {
		MoveMessageType message = new MoveMessageType();
		PositionType shiftPosition = new PositionType();
		shiftPosition.setCol(1);
		shiftPosition.setRow(0);
		message.setShiftPosition(shiftPosition);
		PositionType pinPosition = new PositionType();
		pinPosition.setCol(1);
		shiftPosition.setRow(0);
		message.setNewPinPos(pinPosition);
		message.setShiftCard(status.getBoard().getShiftCard());
		return message;
	}

}
