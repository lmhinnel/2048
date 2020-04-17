#include <bits/stdc++.h>
using namespace std;

// Game tutorial: Random -> Check movement -> Draw board -> Repeat. If end game, then end :>

int Board[5][5], TempBoard[5][5], r, x, y, score = 0;
vector <int> DoMove;
char c;
const int aRandom[] = {2, 2, 2, 2, 2, 4};
bool GameOver = 0, FullBoard = 0, StillMove = 0;

void inDoMove();
void moveU();
void moveD();
void moveL();
void moveR();

void RandBoard();
void StoreBoard();
void Movement();
bool CheckMove();
void DrawBoard();
void CheckBoard();

int main()
{
    cout << "Start the game?\n";
    string str; getline(cin, str);
    if (str[0] == 'N' || str[0] == 'n' || str[0] == '0') return 0;
    srand(time(NULL));
    for (int i = 0; i < 2; i++) RandBoard();
    while (GameOver == 0)
    {
        system("cls");
        if (FullBoard == 0) RandBoard();
        CheckBoard();
        if (GameOver == 1) break;
        DrawBoard();
        cout << "Chose 8(up), 2(down), 4(left), 6(right) : ";
        cin >> c;
        while (c != '8' && c != '2' && c != '4' && c != '6')
        {
            cout << "\nPlease choose again: ";
            cin >> c;
        }
        cout << endl;

        while (1)
        {
            StoreBoard();
            if (CheckMove() == 1) break;
            else
            {
                cout << "\nPlease chose again: ";
                cin >> c;
            }
        }
        CheckBoard();
    }
    cout << "End game!\n";
    DrawBoard();
    cout << "Please cin '0' to return!\n";
    cin >> c;
    while(c != '0')
    {
        cout << "Write '0' please ->.->\n";
        cin >> c;
    }
    return 0;
}

// Create a random number
void RandBoard()
{
    r = rand() % 16 + 1;
    x = (r - 1) / 4 + 1, y = (r - 1) % 4 + 1;
    while (Board[x][y] != 0)
    {
        r = rand() % 16 + 1;
        x = (r - 1) / 4 + 1;
        y = (r - 1) % 4 + 1;
    }
    Board[x][y] = aRandom[rand() % 6];
}

// Move in DoMove
void inDoMove()
{
    if (DoMove.size() >= 2)
    {
        if (DoMove.size() < 4) DoMove.resize(4, 0);
        for (int i = 0; i < DoMove.size(); i++)
        if (DoMove[i] == 0) break;
        else if (DoMove[i] == DoMove[i + 1])
        {
            DoMove[i] *= 2;
            score += DoMove[i];
            DoMove.push_back(0);
            DoMove.erase(DoMove.begin() + i + 1);
        }
    }
    else DoMove.resize(4, 0);
}

// Movement and reload the board
void moveU()
{
    for (int j = 1; j <= 4; j++)
    {
        for (int i = 1; i <= 4; i++) if (Board[i][j] != 0) DoMove.push_back(Board[i][j]);
        inDoMove();
        for (int i = 1; i <= 4; i++) Board[i][j] = DoMove[i - 1];
        DoMove.clear();
    }
}
void moveD()
{
    for (int j = 1; j <= 4; j++)
    {
        for (int i = 4; i >= 1; i--) if (Board[i][j] != 0) DoMove.push_back(Board[i][j]);
        inDoMove();
        for (int i = 4; i >= 1; i--) Board[i][j] = DoMove[4 - i];
        DoMove.clear();
    }
}
void moveL()
{
    for (int i = 1; i <= 4; i++)
    {
        for (int j = 1; j <= 4; j++) if (Board[i][j] != 0) DoMove.push_back(Board[i][j]);
        inDoMove();
        for (int j = 1; j <= 4; j++) Board[i][j] = DoMove[j - 1];
        DoMove.clear();
    }
}
void moveR()
{
    for (int i = 1; i <= 4; i++)
    {
        for (int j = 4; j >= 1; j--) if (Board[i][j] != 0) DoMove.push_back(Board[i][j]);
        inDoMove();
        for (int j = 4; j >= 1; j--) Board[i][j] = DoMove[4 - j];
        DoMove.clear();
    }
}
void Movement()
{
    if (c == '8') moveU(); // Move up
    if (c == '2') moveD(); // Move down
    if (c == '4') moveL(); // Move left
    if (c == '6') moveR(); // Move right
}

// As if the move is available
void StoreBoard()
{
    for (int i = 1; i <= 4; i++)
        for (int j = 1; j <= 4; j++) TempBoard[i][j] = Board[i][j];

}
bool CheckMove()
{
    Movement();
    for (int i = 1; i <= 4; i++)
        for (int j = 1; j <= 4; j++) if (Board[i][j] != TempBoard[i][j]) return 1;
    return 0;
}

// Draw the board and the score
void DrawBoard()
{
    cout << "_________________________" << endl;
    for (int i = 1; i <= 4; i++)
    {
        for (int j = 1; j <= 4; j++)
            cout << "|" << setw(4) << Board[i][j] << " ";
        cout << "|\n";
    }
    cout << "_________________________\n";
    cout << "Score: " << score << endl << endl;
}

// Check endgame yet, fullboard yet
void CheckBoard()
{
    FullBoard = 1;
    StillMove = 0;
    // Check available space
    for (int i = 1; i <= 4; i++)
    {
        for (int j = 1; j <= 4; j++)
        if (Board[i][j] == 0)
        {
            FullBoard = 0;
            break;
        }
        if (FullBoard == 0) break;
    }
    // Check available move
    if (FullBoard == 1)
    {
        for (int i = 1; i <= 3; i++)
        {
            for (int j = 1; j <= 3; j++)
            {
                if (Board[i][j] == Board[i + 1][j] || Board[i][j] == Board[i][j + 1])
                {
                    StillMove = 1; break;
                }
            }
            if (StillMove == 1) break;
        }
        for (int i = 1; i <= 3; i++)
        {
            if (Board[i][4] == Board[i + 1][4] || Board[4][i] == Board[4][i + 1])
            {
                StillMove = 1; break;
            }
        }
    }
    else StillMove = 1;

    GameOver = (StillMove == 1)? false : true;
}
