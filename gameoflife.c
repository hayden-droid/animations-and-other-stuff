#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define SECOND_TO_MICROSECOND 1000000
#define SIZE 20
#define PADDING 2
#define setCell(data, y, x, status) data[y][x] = status
#define getCell(data, y, x) ((x > 0 && x < SIZE && y > 0 && y < SIZE) ? data[y][x]:0)

typedef struct {
	int cursorX;
	int cursorY;
	int isRunning;
	int **data;
	int **tmp;
} GAME;


void control(GAME *game)
{
	int ch = getch();
	if (!game->isRunning)
	{
		switch(ch)
		{
			case KEY_UP:
				game->cursorY = game->cursorY - 1;
				break;
			case KEY_DOWN:
				game->cursorY = game->cursorY + 1;
				break;
			case KEY_LEFT:
				game->cursorX = game->cursorX - 1;
				break;
			case KEY_RIGHT:
				game->cursorX = game->cursorX + 1;
				break;
		}

		if (game->cursorY < 0)
			game->cursorY = 0;
		else if (game->cursorY >= SIZE)
			game->cursorY = SIZE - 1;

		if (game->cursorX < 0)
			game->cursorX = 0;
		else if (game->cursorX >= SIZE)
			game->cursorX = SIZE - 1;
	}

	switch(ch)
	{
		case ' ':
			if (!game->isRunning)
				setCell(game->data, game->cursorY, game->cursorX, !getCell(game->data, game->cursorY, game->cursorX));
			break;	
		case 'r':
			game->isRunning = !game->isRunning;
			break;
	}
}
int getNeighbour(int **data, int y, int x)
{
	int c = 0;
	if (getCell(data, y + 1, x))
		c++;	
	if (getCell(data, y - 1, x))
		c++;	
	if (getCell(data, y, x + 1))
		c++;	
	if (getCell(data, y, x - 1))
		c++;	
	if (getCell(data, y + 1, x + 1))
		c++;	
	if (getCell(data, y + 1, x - 1))
		c++;	
	if (getCell(data, y - 1, x + 1))
		c++;	
	if (getCell(data, y - 1, x - 1))
		c++;	
	return c;	
}

void update(GAME *game)
{
	int y, x;
	for (y = 0; y < SIZE; y++)
		for (x = 0; x < SIZE; x++)
			setCell(game->tmp, y, x, getCell(game->data, y, x));

	int c;
	for (y = 0; y < SIZE; y++)

		for (x = 0; x < SIZE; x++)
		{
			c = getNeighbour(game->tmp, y, x);	
			if (getCell(game->data, y, x))
			{
				if (c == 2 || c == 3)
					setCell(game->data, y, x, 1);
				else if (c > 3)
					setCell(game->data, y, x, 0);
				else if (c < 2)
					setCell(game->data, y, x, 0);
			}
			else
			{
				if (c == 3)
					setCell(game->data, y, x, 1);
			}
		}
}

void draw(GAME *game)
{
	int y, x;
	for (y = 0; y < SIZE; y++)
		for (x = 0; x < SIZE; x++)
		{
			if (game->data[y][x] == 1)
				mvaddch(y, x*PADDING, '0');
			else
				mvaddch(y, x*PADDING, '.');

		}
	if (!game->isRunning)
		mvaddch(game->cursorY, game->cursorX*PADDING, 'v');
	mvprintw(1, SIZE*PADDING + 10, "Status");
	mvprintw(2, SIZE*PADDING + 10, "Simulation is running: %d", game->isRunning);
}

GAME *initGAME()
{
	GAME *new_game = malloc(sizeof(GAME));
	new_game->cursorX = new_game->cursorY = 0;
	new_game->isRunning = 0;
	new_game->data = malloc(SIZE * sizeof(int *));
	new_game->tmp = malloc(SIZE * sizeof(int *));
	int y, x;
	for (y = 0; y < SIZE; y++)
	{
		new_game->data[y] = malloc(SIZE * sizeof(int));
		new_game->tmp[y] = malloc(SIZE * sizeof(int));
		for (x = 0; x < SIZE; x++)
		{
			new_game->tmp[y][x] = 0;	
			new_game->data[y][x] = 0;	
		}
	}
	return new_game;
}

int main()
{
	GAME *game = initGAME();
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(0);
	int t = 0;
	while (1)
	{
		control(game);
		if (game->isRunning)
		{
			if (t%60 == 0)
			{
				update(game);
				usleep(0.3 * SECOND_TO_MICROSECOND);
			}
			t++;
		}
		draw(game);
		refresh();
	}
	endwin();
	return 0;
}

