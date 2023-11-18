#include <stdio.h>
#include "variables.h"

// Evalua si el tablero está en una posición final, no hay más posiciones libres
int finDelJuego() {
	for (int i = 0; i < TAM_MAX; i++)
		for (int j = 0; j < TAM_MAX; j++)
			if (board[i][j] == 0)
				return (FALSE);
	return (TRUE);
}
// Chequea quien gana el juego
// Condición de victorio: Quien tiene más dados en el tablero
int checkWinner() {
	int sumaP1 = 0, sumaP2 = 0;
	for (int i = 0; i < TAM_MAX; i++)
		for (int j = 0; j < TAM_MAX; j++) {
			if (board[i][j] > 6)
				sumaP2 += board[i][j];
			else
				sumaP1 += board[i][j];
		}
	return ((sumaP1 > sumaP2 ? PLAYER1 : PLAYER2));
}

//    Evalua el puntaje de una configuración del tablero
// Chequea si es una posición válida
int isValid(int i, int j) {
	return (i >= 0 && i < TAM_MAX && j >= 0 && j < TAM_MAX);
}
//Suma dados adyacentes
int sumAdjacentPips(int x, int y, int *count) {
	int sum = 0;
	*count = 0;
	int dx[] = { -1, 0, 1, 0 };
	int dy[] = { 0, 1, 0, -1 };
	for (int i = 0; i < 4; i++) {
		int newX = x + dx[i];
		int newY = y + dy[i];
		if (isValid(newX, newY) && board[newX][newY] > 0) {
			sum += board[newX][newY];
			(*count)++;
		}
	}
	return sum;
}
// Asigna un valor al tablero
int evaluateBoard() {
	int score = 0;
	int player1Dice = 0;
	int player2Dice = 0;
	int potentialCaptureVal = 0;
	for (int i = 0; i < TAM_MAX; i++) {
		for (int j = 0; j < TAM_MAX; j++) {
			if (board[i][j] > 0 && board[i][j] <= 6) {
				player1Dice++;
			} else if (board[i][j] > 6) {
				player2Dice++;
			} else {
				int count;
				int adjacentSum = sumAdjacentPips(i, j, &count);
				if (adjacentSum <= 6 && count >= 2) {
					potentialCaptureVal += 2;
				}
			}
		}
	}
	score = player1Dice - player2Dice + potentialCaptureVal;
	return score;
}
// Si hay un adyacente en x,y de tablero, entonces se come. Estrategia:
// Se come el primer adyacente que se encuentra
// Si se comió, se retorna TRUE. FALSE en otro caso
int comeAdyacente(int x, int y, int tablero, int player)
{
	// Posicion de las piezas que pueden ser adyacentes
	int adyacentes[4][2] = { { x - 1, y }, { x + 1, y }, { x, y - 1 }, { x, y
			+ 1 } };
	// Que posiciones adyacentes tienen dados
	int hayDadosAdy[4] = { FALSE, FALSE, FALSE, FALSE };
	// Los dos dados adyacentes que se van a comer
	int adyXY[2][2] = { { VACIO, VACIO }, { VACIO, VACIO } };
	// Cantidad de adyacentes
	int count = 0;
	// Revisar las piezas adyacentes
	for (int i = 0; i < 4; i++) {
		int nx = adyacentes[i][0];
		int ny = adyacentes[i][1];
		// Es una posición valida y está ocupada?
		if (isValid(nx, ny)
				&& (
				(tablero == MAINBOARD && board[nx][ny] != VACIO
						&& (board[nx][ny] != 6 && board[nx][ny] != 12))
						||
						(tablero == MIRRORBOARD && mirror[nx][ny] != VACIO
								&& (mirror[nx][ny] != 6 && mirror[nx][ny] != 12)))) {
			hayDadosAdy[i] = TRUE;
			count++;
		}
	}
	// Se come 2 adyacentes a la posición x,y
	// Si hay más de 2, se pregunta
	// Si hay menos de 2, no se come
	// Si count > 2, hay que preguntar y elegir adyacente
	if (count > 2) {
		//Se tiene que elegir 2 adyacentes
		//Hay entre 3 o 4 adyacentes.
		int adyElegidos = 0, adyx, adyy;
		// SI juega la PC, se elije automáticamente 2 adyacentes cuando hay 3 o 4.
		// La PC juega en modojuego=1 y PLAYER2, modojuego=2 y PLAYER1 y modojuego=3 sin importar el player.
		if ((modoJuego == 1 && player == PLAYER2) ||
		(modoJuego == 2 && player == PLAYER1) ||
		(modoJuego == 3)) {
			for (int i = 0, j = 0; i < 4; i++)
				if (hayDadosAdy[i]) {
					if (j <= 1) {
						adyXY[j][0] = adyacentes[i][0];
						adyXY[j++][1] = adyacentes[i][0];
					}
					else
						break;
				}
		} else {
			do {
				printf("\nDados a comer elegidos: %d.", adyElegidos);
				printf("\nElegir adyacente a comer (Fila Columna ): ");
				scanf("%d %d", &adyx, &adyy);
				for (int i = 0; i < 4; i++)
					if (hayDadosAdy[i] && adyacentes[i][0] == adyx
							&& adyacentes[i][1] == adyy) {
						adyXY[adyElegidos][0] = adyx;
						adyXY[adyElegidos++][1] = adyy;
					}
			} while (adyElegidos < 2);
		}
	} else if (count == 2) {
		// Si count es 2, se come automáticamente los ady
		for (int i = 0, j = 0; i < 4; i++)
			if (hayDadosAdy[i]) {
				adyXY[j][0] = adyacentes[i][0];
				adyXY[j++][1] = adyacentes[i][1];
			}
	} else {
		//No hay adyacentes suficientes, no se puede comer
		return (FALSE);
	}
	//Si hay algo que comer, está en adyXY
	//Se suma y acumula en x, y y se elimina los dados que están en adyXY. Si la suma da más de 6, se trunca a 6 (o 12 si es PLAYER2)
	if (tablero == MAINBOARD) {
		board[x][y] =
		board[adyXY[0][0]][adyXY[0][1]] +
		(board[adyXY[0][0]][adyXY[0][1]] < 6 ? 0 : -6) +
		board[adyXY[1][0]][adyXY[1][1]] +
		(board[adyXY[1][0]][adyXY[1][1]] < 6 ? 0 : -6);
		//Si pasaron el tope, topear en 6 o 12
		if (board[x][y] > 6 && player == PLAYER1)
			board[x][y] = 6;
		else if (board[x][y] > 12 && player == PLAYER2)
			board[x][y] = 12;
		else if (player == PLAYER2)
			//Si no topeo, pero es PLAYER2, hay que poner el valor enttre 7 y 11
			board[x][y] += 6;
	} else {
		//Es el tablero MIRROR
		mirror[x][y] =
		mirror[adyXY[0][0]][adyXY[0][1]] +
		(mirror[adyXY[0][0]][adyXY[0][1]] < 6 ? 0 : -6) +
		mirror[adyXY[1][0]][adyXY[1][1]] +
		(mirror[adyXY[1][0]][adyXY[1][1]] < 6 ? 0 : -6);
		//Si pasaron el tope, topear en 6 o 12
		if (mirror[x][y] > 6 && player == PLAYER1)
			mirror[x][y] = 6;
		else if (mirror[x][y] > 12 && player == PLAYER2)
			mirror[x][y] = 12;
		else if (player == PLAYER2)
			//Si no topeo, pero es PLAYER2, hay que poner el valor enttre 7 y 11
			mirror[x][y] += 6;
	}
	// Ya se comio y se sumo. Eliminar el adyacente
	if (tablero == MAINBOARD) {
		board[adyXY[0][0]][adyXY[0][1]] = VACIO;
		board[adyXY[1][0]][adyXY[1][1]] = VACIO;
	} else {
		mirror[adyXY[0][0]][adyXY[0][1]] = VACIO;
		mirror[adyXY[1][0]][adyXY[1][1]] = VACIO;
	}
	return (TRUE);
}
