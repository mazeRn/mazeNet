package clientPackages.client;

import java.io.IOException;

import java.util.Scanner;

import clientPackages.networking.Connection;



public class Game {
	public static void main(String[] args) {
		Scanner scan = new Scanner(System.in);
		System.out.println("Enter IP Address");
		String ip = scan.nextLine();
		System.out.println("Enter Port");
		int port = scan.nextInt();
		
		Connection connection=null ;
		try {
			connection = new Connection(ip, port);
		} catch (IOException e) {
			e.printStackTrace();
		}
		while (!connection.awaitMessage()){
			
		}
		scan.close();
	}
}
