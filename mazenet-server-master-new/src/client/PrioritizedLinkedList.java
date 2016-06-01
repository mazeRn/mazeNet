package client;

import java.util.ArrayList;
import java.util.LinkedList;

public class PrioritizedLinkedList<T> {

	private LinkedList<T> linkedList;
	private ArrayList<Integer> priorityIndexList;

	public PrioritizedLinkedList() {
		linkedList = new LinkedList<>();
		priorityIndexList = new ArrayList<>();
	}

	public PrioritizedLinkedList(PrioritizedLinkedList<T> other) {
		this.priorityIndexList = new ArrayList<>(other.priorityIndexList);
		this.linkedList = new LinkedList<>(other.linkedList);
	}

	public void add(int prio, T element) {
		assert prio >= 0;

		if (prio < priorityIndexList.size()) {
			// means that prio exists in the list already

			if (prio < priorityIndexList.size() - 1) {
				// add as last of the prio sublist
				linkedList.add(priorityIndexList.get(prio), element);
			} else {
				// add as last of the whole list, because there is no bigger prio
				linkedList.add(element);
			}

			// shift all prio indices, starting with our prio
			for (int i = prio; i < priorityIndexList.size(); ++i) {
				priorityIndexList.set(i, priorityIndexList.get(i) + 1);
			}
		} else {
			int currentMaxIndex = 0;
			if (!priorityIndexList.isEmpty()) {
				currentMaxIndex = priorityIndexList.get(priorityIndexList.size() - 1);
			}

			for (int i = priorityIndexList.size(); i < prio; i++) {
				priorityIndexList.add(currentMaxIndex);
			}

			priorityIndexList.add(currentMaxIndex + 1);
			linkedList.add(element);
		}
	}

	public T remove(int index) {
		for (int i = 0; i < priorityIndexList.size(); i++) {
			if (priorityIndexList.get(i) >= index) {
				priorityIndexList.set(i, priorityIndexList.get(i) - 1);
			}
		}

		return linkedList.remove(index);
	}

	public T get(int index) {
		return linkedList.get(index);
	}

	public int getMaxPrio() {
		return priorityIndexList.size() - 1;
	}

	public int size() {
		return linkedList.size();
	}

	public void clear() {
		linkedList.clear();
		priorityIndexList.clear();
	}

	@Override public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("PrioritizedLinkedList{");

		boolean iFirst = true;
		for (int i = 0; i < priorityIndexList.size(); ++i) {
			if (iFirst) {
				iFirst = false;
			} else {
				sb.append(", ");
			}
			sb.append('\"');
			sb.append(i);
			sb.append("\": [");
			boolean jFirst = true;
			for (int j = (i > 0 ? priorityIndexList.get(i - 1) : 0); j < priorityIndexList.get(i); ++j) {
				if (jFirst) {
					jFirst = false;
				} else {
					sb.append(", ");
				}
				sb.append(linkedList.get(j));
			}
			sb.append(']');
		}

		sb.append('}');
		return sb.toString();
	}
}