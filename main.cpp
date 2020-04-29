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

const int boardSize = 4; // Size of board
const int spriteSize = 130; // Size of source sprite
const int spriteWin = 100; //
const int window_width = 500;
const int window_height = 700;
const int aRandom[] = {2, 2, 2, 2, 2, 4}; // In order to make the times random 2 more than random 4
const SDL_Color White = {255, 255, 255, 255};
const SDL_Rect textRec = {window_width / 2 - 50, window_height - 100, 100, 70};
// const int value[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536}; // Available value of a box
const string windowTitle = "2048 super kool"; // Name of window
const string backgroundPath = "bg.png"; // Background path
const string spritePath = "sprite.png"; // Sprite path
const string musicPath = "remon.ogg"; // Music path
const string fontpath = "circle3d.ttf"; // Font path

struct Game
{
    int Board[boardSize + 1][boardSize + 1]; // A square board from 1 to 4
    int TempBoard[boardSize + 1][boardSize + 1]; // Store the board before doing move
    int x, y; // Location of a box in Board
    long long score;
    int r; // A random number from aRandom to pick for a box
    vector <int> DoMove; // Step by step for each column or row
};

struct Graphic
{
    SDL_Window* Window; // Window
    SDL_Renderer* renderer; // To draw into Window
    SDL_Texture* background; // Background image
    SDL_Texture* spriteTexture; // Sprites image
    Mix_Music* music;
    TTF_Font* font;
    SDL_Texture* text;
    vector <SDL_Rect> spriteRec; // Sprite location in sprite.png
    vector <SDL_Rect> winRec; // Sprite location in Window
    SDL_Event event; // Event include space to exit, up, down, left, right
};

// Initial Game
SDL_Texture* createBackground (SDL_Renderer* renderer, string path); // Turn background.png to texture
SDL_Texture* createSprtieTexture(SDL_Renderer* renderer, string path); // Turn sprite.png to texture
void initSpriteRects(vector<SDL_Rect>& spriteRec, vector <SDL_Rect>& winRec); // Make spriteRec
void err(string& m);

// Create game when start a new game
void RandBoard(Game& game); // <necessary>
void createGame(Game& game); // <necessary>
bool initGraphic(Graphic& g); // Create everything <necessary>

// Draw score
void drawScore(int& score, Graphic& g);
// Draw board
void drawGame(Game & game, Graphic& g); // <necessary>

// Check the board if still have an available move after creating a random number
bool GameOver(Game& game); // <necessary>

// Solving
void StoreBoard(Game& game); // Store board into TempBoard before moving

void inDoMove(vector <int>& DoMove, long long& score); // Sort each row or column in each move
void moveU(Game& game);
void moveD(Game& game);
void moveL(Game& game);
void moveR(Game& game);

bool CheckMove(Game& game); // Check if after moving the board is different or not

void initEvent(Game& game, SDL_Event& event); // <necessary>

// When end game
void close(Graphic& g); // Destroy everything <necessary>

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

    createGame(game);
    bool moved = 1;
    Mix_PlayMusic(g.music, -1);
    while (true)
    {
        if(GameOver(game) == 1) break;
        if(moved == 1)
        {
            RandBoard(game);
            moved = 0;
        }
        drawGame(game, g);
        while (SDL_PollEvent(&g.event) != 0)
        {
            if (g.event.type == SDL_QUIT)
            {

                close(g);
                return 0;
            }
            if (g.event.type == SDL_KEYDOWN)
            {
                initEvent(game, g.event);
                moved = CheckMove(game);
            }
        }
    }
    SDL_Delay(2000);
    close(g);
    return 0;
}


// Initial Game
SDL_Texture* createBackground (SDL_Renderer* renderer, string path)
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
SDL_Texture* createSprtieTexture(SDL_Renderer* renderer, string path)
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

// Create game when start a new game
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
}
bool initGraphic(Graphic& g)
{
    g.Window = NULL;
    g.renderer = NULL;
    g.background = NULL;
    g.spriteTexture = NULL;
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
    g.background = createBackground(g.renderer, backgroundPath);
    g.spriteTexture = createSprtieTexture(g.renderer, spritePath);
    if (g.background == NULL || g.spriteTexture == NULL)
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
    g.music = Mix_LoadMUS(musicPath.c_str()); // Create music for game
    if (g.music == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    if (TTF_Init() != 0)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    g.font = TTF_OpenFont(fontpath.c_str(), 24);
    if (g.font == NULL)
    {
        string m = SDL_GetError();
        err(m);
        return false;
    }
    initSpriteRects(g.spriteRec, g.winRec);
    return true;
}

// Draw board
void drawScore (long long& score, Graphic& g)
{
    SDL_Surface* sur = NULL;
    stringstream ss; ss << score;
    string out = "Score: " + ss.str();
    sur = TTF_RenderText_Solid(g.font, out.c_str(), White);
    g.text = SDL_CreateTextureFromSurface(g.renderer, sur);
    SDL_FreeSurface(sur);
}
void drawGame(Game& game, Graphic& g)
{
    SDL_RenderClear(g.renderer);
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
        pos = i * boardSize - (boardSize - j) - 1;
        if (MTP == 0) SDL_RenderCopy(g.renderer, g.spriteTexture, &rec, &g.winRec[pos]);
    }
    drawScore(game.score, g);
    SDL_RenderCopy(g.renderer, g.text, NULL, &textRec);
    SDL_DestroyTexture(g.text);
    SDL_RenderPresent(g.renderer);
}

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
bool CheckMove(Game& game)
{
    for (int i = 1; i <= boardSize; i++)
        for (int j = 1; j <= boardSize; j++) if (game.Board[i][j] != game.TempBoard[i][j]) return 1;
    return 0;
}

void initEvent(Game& game, SDL_Event& event)
{
    StoreBoard(game);
    switch (event.key.keysym.sym)
    {
        case SDLK_UP:       moveU(game); break;
        case SDLK_DOWN:     moveD(game); break;
        case SDLK_LEFT:     moveL(game); break;
        case SDLK_RIGHT:    moveR(game); break;
        default: return;
    }
}

void close(Graphic& g)
{
    SDL_DestroyTexture(g.background);
    SDL_DestroyTexture(g.spriteTexture);
    SDL_DestroyTexture(g.text);
    SDL_DestroyRenderer(g.renderer);
    SDL_DestroyWindow(g.Window);
    Mix_FreeMusic(g.music);
    TTF_CloseFont(g.font);

    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
