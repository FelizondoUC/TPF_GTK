#include <stdio.h>
#include "variables.h"
#include "jugabilidad.h"

// JUEGA HUMANO. Elegir fila y columna
void juegaHumano(int player) {
	int i, j;
	do {
		printf("\nIntroduce tu jugada (Fila Columna del 0 al 4): ");
		printf("\n(Para salir Fila o Columna = -1)");
		scanf("%d %d", &i, &j);
	} while (i < -1 || i >= TAM_MAX || j < -1 || j >= TAM_MAX
			|| board[i][j] != VACIO);

	if ( i==-1 || j ==-1) {
		printf("\n¡Lo sentimos has perdido! Elegiste rendirte.");
		exit(0);
	}

	if (player == PLAYER1) // Player1, dados 1-6
		board[i][j] = 1;
	else
		// Player2, dados 7-12
		board[i][j] = 7;
	// Si se puede, comer
	comeAdyacente(i, j, MAINBOARD, player);
}
