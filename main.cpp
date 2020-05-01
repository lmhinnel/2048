#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

// Game tutorial: Random -> Check movement -> Draw board -> Repeat. If end game, then end :>
// Use Space to return ONE move

// Const ------------------------------------------------------------------------//

const int boardSize = 4; // Size of board

const int spriteSize = 130; // Size of source sprite
const int spriteWin = 100; // Size of Window sprite

const int window_width = 500;
const int window_height = 700;

const int aRandom[] = {2, 2, 2, 2, 2, 4}; // In order to make the times random 2 more than random 4

const SDL_Color White = {255, 255, 255, 255};
const SDL_Rect textRec = {window_width / 2 - 100, window_height - 120, 200, 70}; // Score text rec for score only

const string windowTitle = "2048 super kool"; // Name of window

const string backgroundPath = "bg.png"; // Background path
const string spritePath = "sprite.png"; // Sprite path

const string musicPath[] = {"remon.ogg", "bee.ogg", "db.ogg", "dtna.ogg"}; // Music path

const string fontpath = "circle3d.ttf"; // Font path for score
const string fontpath2 = "vni27.ttf"; // Font path for text play again

// Struc ------------------------------------------------------------------------//

struct Game
{
    int Board[boardSize + 1][boardSize + 1]; // A square board from 1 to 4
    int TempBoard[boardSize + 1][boardSize + 1]; // Store the board before doing move
    int x, y; // Location of a box in Board
    long long score;
    int r; // A random number from aRandom to pick for a box
    vector <int> DoMove; // Step by step for each column or row
    bool gaming; // Still wanna play
};

struct Graphic
{
    SDL_Window* Window; // Window
    SDL_Renderer* renderer; // To draw into Window
    SDL_Texture* background; // Background image
    SDL_Texture* spriteTexture; // Sprites image
    SDL_Texture* tutorial; // Game tutorial image
    Mix_Music* music; // Game music
    TTF_Font* font; // Game font
    SDL_Texture* text; // Game text for both score and textEnd
    vector <SDL_Rect> spriteRec; // Sprite location in sprite.png
    vector <SDL_Rect> winRec; // Sprite location in Window
    SDL_Event event; // Event include space to undo, up, down, left, right
};

// Initial Game -----------------------------------------------------------------//
SDL_Texture* createTexture (SDL_Renderer* renderer, string path); // Turn .png to texture
void initSpriteRects(vector<SDL_Rect>& spriteRec, vector <SDL_Rect>& winRec); // Make spriteRec
void err(string& m);

//-------------------------------------------------------------------------------//

// Create game when start a new game --------------------------------------------//

void RandBoard(Game& game); // <necessary>
void createGame(Game& game); // <necessary>
bool initGraphic(Graphic& g); // Create everything <necessary>

// Draw -------------------------------------------------------------------------//

void drawScore(long long& score, Graphic& g);// Draw score

void drawGame(Game & game, Graphic& g); // Draw board <necessary>

bool GameOver(Game& game); // Check the board if still have an available move after creating a random number <necessary>

// Solving ---------------------------------------------------------------------//

void StoreBoard(Game& game); // Store board into TempBoard before moving

void inDoMove(vector <int>& DoMove, long long& score); // Sort each row or column in each move

void moveU(Game& game); // Move up
void moveD(Game& game); // Move down
void moveL(Game& game); // Move left
void moveR(Game& game); // Move right

void moveUndo(Game& game); // Undo one move

bool CheckMove(Game& game); // Check if after moving the board is different or not

// Event ----------------------------------------------------------------------//

void initEvent(Game& game, Graphic& g, bool& moved); // <necessary>

void toolhack(Game& game, Graphic& g); // trick

// When end game --------------------------------------------------------------//

void textEnd (Graphic& g); // play again by pressing ENTER
void close(Graphic& g); // Destroy everything <necessary>

//-----------------------------------------------------------------------------//



//=============================================================================//

int main(int agrc, char* agrv[])
{
    Game game;
    Graphic g;
    srand(time(NULL));
    if (!initGraphic(g))
    {
        close(g);
        return EXIT_FAILURE;
    }

    game.gaming = 1;
    bool run = 1;

    while(game.gaming)
    {
        createGame(game);
        bool moved = 1;

        if (run == 1)
        {
            g.music = Mix_LoadMUS(musicPath[rand() % 4].c_str());
            if(Mix_PlayMusic(g.music, -1) != 0)
            {
                string m; m = Mix_GetError(); err(m); break;
            }
        }
        else
        {
            Mix_FreeMusic(g.music);
            g.music = NULL;
        }

        g.font = TTF_OpenFont(fontpath.c_str(), 26);

        while (run)
        {
            if(moved == 1) // If Board has moved then random a number for an empty box
            {
                RandBoard(game);
                moved = 0;
            }
            drawGame(game, g); // Draw the board
            if(GameOver(game) == 1) // If after random, there are no available move then gameover, run = 0; Ask if the player wants to play again
            {
                run = 0;
                break;
            }
            while (SDL_PollEvent(&g.event) != 0) // Analyze event
            {
                if (g.event.type == SDL_QUIT) // Quit game if click X button
                {

                    close(g);
                    return 0;
                }
                if (g.event.type == SDL_KEYDOWN) // Solving
                {
                    initEvent(game, g, moved);
                }
            }
        }
        while (SDL_PollEvent(&g.event) != 0) // After endgame
        {
            if (g.event.type == SDL_QUIT)
            {
                close(g); return 0;
            }
            if (g.event.type == SDL_KEYDOWN && g.event.key.keysym.sym == SDLK_RETURN) // If wanna play again
            {
                run = 1;
                break;
            }
            if (g.event.type == SDL_KEYDOWN && g.event.key.keysym.sym == SDLK_SPACE) // If not, or click X button
            {
                run = 0;
                game.gaming = 0;
                break;
            }
        }

        textEnd(g); // Instruction to play agian
    }
    close(g); // Delay 2s then close
    return 0;
}

//=============================================================================//



//-----------------------------------------------------------------------------//


SDL_Texture* createTexture (SDL_Renderer* renderer, string path)
{
    SDL_Surface* sur = IMG_Load(path.c_str());
    if (sur == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_FreeSurface(sur);
    return tex;
}

void initSpriteRects(vector<SDL_Rect>& spriteRec, vector <SDL_Rect>& winRec)
{
    SDL_Rect rec = {0, 0, spriteSize, spriteSize};

    for (int i = 0; i <= (spriteSize - 1) * boardSize; i += spriteSize)
    for (int j = 0; j <= (spriteSize - 1) * boardSize; j += spriteSize)
    {
        rec.x = j;
        rec.y = i;
        spriteRec.push_back(rec);
    }
    rec = {0, 0, spriteWin, spriteWin};
    int space = (window_width - boardSize * spriteWin) / 2;
    for (int i = space; i < window_width - space; i += spriteWin)
    for (int j = space; j < window_width - space; j += spriteWin)
    {
        rec.x = j;
        rec.y = i;
        winRec.push_back(rec);
    }
}

void err(string& m)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERROR", m.c_str(), NULL);
}

//-----------------------------------------------------------------------------//

void RandBoard(Game& game)
{
    game.r = rand() % (boardSize * boardSize) + 1;
    game.x = (game.r - 1) / boardSize + 1; game.y = (game.r - 1) % boardSize + 1;
    while (game.Board[game.x][game.y] != 0)
    {
        game.r = rand() % (boardSize * boardSize) + 1;
        game.x = (game.r - 1) / boardSize + 1;
        game.y = (game.r - 1) % boardSize + 1;
    }
    game.Board[game.x][game.y] = aRandom[rand() % 6];
}

void createGame(Game& game)
{
    for (int i = 0; i <= boardSize; i++)
        for (int j = 0; j <= boardSize; j++)
    {
        game.Board[i][j] = 0;
        game.TempBoard[i][j] = 0;
    }
    RandBoard(game);
    game.score = 0;
    game.gaming = 1;
}

bool initGraphic(Graphic& g)
{
    g.Window = NULL;
    g.renderer = NULL;
    g.background = NULL;
    g.spriteTexture = NULL;
    g.tutorial = NULL;
    g.music = NULL;
    g.font = NULL;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    g.Window = SDL_CreateWindow(windowTitle.c_str(),SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
    if (g.Window == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    g.renderer = SDL_CreateRenderer(g.Window, - 1, SDL_RENDERER_ACCELERATED);
    if (g.renderer == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    g.background = createTexture(g.renderer, backgroundPath);
    g.spriteTexture = createTexture(g.renderer, spritePath);
    if (g.background == NULL || g.spriteTexture == NULL )
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) != 0) // Open audio
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    for (int i = 0; i < 4; i++)
    {
        g.music = Mix_LoadMUS(musicPath[i].c_str()); // Create music for game
        if (g.music == NULL)
        {
            string m = SDL_GetError();
            err(m);
            return false;
        }
    }
    if (TTF_Init() != 0)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    g.font = TTF_OpenFont(fontpath.c_str(), 26);
    if (g.font == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    initSpriteRects(g.spriteRec, g.winRec);
    return true;
}

//-----------------------------------------------------------------------------//

void drawScore (long long& score, Graphic& g)
{
    SDL_Surface* sur = NULL;
    stringstream ss; ss << score; // turn score to string
    string out = "Score: " + ss.str();
    sur = TTF_RenderText_Solid(g.font, out.c_str(), White);
    g.text = SDL_CreateTextureFromSurface(g.renderer, sur); // turn text into texture
    SDL_FreeSurface(sur);
}

void drawGame(Game& game, Graphic& g)
{
    SDL_RenderClear(g.renderer); // Clear the reder before drawing again
    SDL_Rect rec = {0, 0, window_width, window_height};
    SDL_RenderCopy(g.renderer, g.background, &rec, NULL);
    bool MTP; // if a box is empty
    int pos = 0;
    for (int i = 1; i <= boardSize; i++)
    for (int j = 1; j <= boardSize; j++)
    {
        MTP = 0;
        switch (game.Board[i][j])
        {
            case 2:      rec = g.spriteRec[0];   break;
            case 4:      rec = g.spriteRec[1];   break;
            case 8:      rec = g.spriteRec[2];   break;
            case 16:     rec = g.spriteRec[3];   break;
            case 32:     rec = g.spriteRec[4];   break;
            case 64:     rec = g.spriteRec[5];   break;
            case 128:    rec = g.spriteRec[6];   break;
            case 256:    rec = g.spriteRec[7];   break;
            case 512:    rec = g.spriteRec[8];   break;
            case 1024:   rec = g.spriteRec[9];   break;
            case 2048:   rec = g.spriteRec[10];  break;
            case 4096:   rec = g.spriteRec[11];  break;
            case 8192:   rec = g.spriteRec[12];  break;
            case 16384:  rec = g.spriteRec[13];  break;
            case 32768:  rec = g.spriteRec[14];  break;
            case 65536:  rec = g.spriteRec[15];  break;
            default : MTP = 1;
        }
        pos = i * boardSize - (boardSize - j) - 1; // position of the box in winRec[]
        if (MTP == 0) SDL_RenderCopy(g.renderer, g.spriteTexture, &rec, &g.winRec[pos]); // cut spriteTexture into rec and then draw in Window at the position of winRec
    }
    drawScore(game.score, g); // Update the score and change g.text
    SDL_RenderCopy(g.renderer, g.text, NULL, &textRec); // Null to get full texture of text
    SDL_DestroyTexture(g.text); // Free memory
    SDL_RenderPresent(g.renderer); // Show on Win
}

//-----------------------------------------------------------------------------//

bool GameOver(Game& game)
{
    // Check available space
    for (int i = 1; i <= boardSize; i++)
    {
        for (int j = 1; j <= boardSize; j++)
        if (game.Board[i][j] == 0) // If this box is empty then the board is not full yet, return GameOver = False
        {
            return false;
        }
    }
    // Check if there are two boxes have a same number when the board is full
    for (int i = boardSize; i >= 1; i--)
    {
        for (int j = boardSize; j >= 1; j--)
        {
            if (game.Board[i][j] == game.Board[i - 1][j] || game.Board[i][j] == game.Board[i][j - 1])
            {
                return false;
            }
        }
    }
    return true;
}

void StoreBoard(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
        for (int j = 1; j <= boardSize; j++) game.TempBoard[i][j] = game.Board[i][j];
}

void inDoMove(vector <int>& DoMove, long long& score)
{
    if (DoMove.size() >= 2)
    {
        if (DoMove.size() < boardSize) DoMove.resize(boardSize, 0);
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

void moveU(Game& game)
{
    for (int j = 1; j <= boardSize; j++)
    {
        for (int i = 1; i <= boardSize; i++) if (game.Board[i][j] != 0) game.DoMove.push_back(game.Board[i][j]);
        inDoMove(game.DoMove, game.score);
        for (int i = 1; i <= boardSize; i++) game.Board[i][j] = game.DoMove[i - 1];
        game.DoMove.clear();
    }
}
void moveD(Game& game)
{
    for (int j = 1; j <= boardSize; j++)
    {
        for (int i = boardSize; i >= 1; i--) if (game.Board[i][j] != 0) game.DoMove.push_back(game.Board[i][j]);
        inDoMove(game.DoMove, game.score);
        for (int i = boardSize; i >= 1; i--) game.Board[i][j] = game.DoMove[4 - i];
        game.DoMove.clear();
    }
}
void moveL(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
    {
        for (int j = 1; j <= boardSize; j++) if (game.Board[i][j] != 0) game.DoMove.push_back(game.Board[i][j]);
        inDoMove(game.DoMove, game.score);
        for (int j = 1; j <= boardSize; j++) game.Board[i][j] = game.DoMove[j - 1];
        game.DoMove.clear();
    }
}
void moveR(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
    {
        for (int j = boardSize; j >= 1; j--) if (game.Board[i][j] != 0) game.DoMove.push_back(game.Board[i][j]);
        inDoMove(game.DoMove, game.score);
        for (int j = boardSize; j >= 1; j--) game.Board[i][j] = game.DoMove[boardSize - j];
        game.DoMove.clear();
    }
}

void moveUndo(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
        for (int j = 1; j <= boardSize; j++) game.Board[i][j] = game.TempBoard[i][j];
}

bool CheckMove(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
        for (int j = 1; j <= boardSize; j++) if (game.Board[i][j] != game.TempBoard[i][j]) return 1;
    return 0;
}

//-----------------------------------------------------------------------------//

void initEvent(Game& game, Graphic& g, bool& moved)
{
    switch (g.event.key.keysym.sym)
    {
        case SDLK_UP:           StoreBoard(game); moveU(game); moved = CheckMove(game); break;
        case SDLK_DOWN:         StoreBoard(game); moveD(game); moved = CheckMove(game); break;
        case SDLK_LEFT:         StoreBoard(game); moveL(game); moved = CheckMove(game); break;
        case SDLK_RIGHT:        StoreBoard(game); moveR(game); moved = CheckMove(game); break;
        case SDLK_SPACE:        if(game.score == 0) break; else { moveUndo(game); moved = 0; break; } // undo key
        case SDLK_LCTRL:        toolhack(game, g); moved = 0; break; // tool key
        default: return;
    }
}

//-----------------------------------------------------------------------------//

void toolhack(Game& game, Graphic& g)
{
    int k = 1;
    for (int i = 1; i <= boardSize; i++)
        for (int j = 1; j <= boardSize; j++) game.Board[i][j] = k*=2;
    game.score = 9999999999;
}

//-----------------------------------------------------------------------------//

void textEnd (Graphic& g)
{
    g.font = TTF_OpenFont(fontpath2.c_str(), 22);
    string out = "Press ENTER to play again.";
    SDL_Surface* sur = TTF_RenderText_Solid(g.font, out.c_str(), White);
    g.text = SDL_CreateTextureFromSurface(g.renderer, sur);
    SDL_FreeSurface(sur);

    SDL_Rect rec = {0, window_height / 2 - 25, window_width, 25 * 2};
    SDL_SetRenderDrawColor(g.renderer, 0, 0 , 0, 255); // set colour black
    SDL_RenderFillRect(g.renderer, &rec); // Draw a filled rec
    SDL_SetRenderDrawColor(g.renderer, 0, 0, 0, 0); // set colour transparent

    SDL_RenderCopy(g.renderer, g.text, NULL, &rec);
    SDL_DestroyTexture(g.text);
    SDL_RenderPresent(g.renderer);
}

void close(Graphic& g)
{
    SDL_Delay(2000);
    Mix_FreeMusic(g.music);
    TTF_CloseFont(g.font);
    SDL_DestroyTexture(g.spriteTexture);
    SDL_DestroyTexture(g.text);
    SDL_DestroyTexture(g.background);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyWindow(g.Window);

    g.music = NULL;
    g.font = NULL;
    g.spriteTexture = NULL;
    g.text = NULL;
    g.background = NULL;
    g.renderer = NULL;
    g.Window = NULL;

    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
