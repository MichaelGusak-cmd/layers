#include <SDL.h>
#include <stdio.h>
#include <iostream>

bool quit;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//Starts up SDL, creates window, and initializes OpenGL
bool init();
//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys(unsigned char key, int x, int y, bool down);
//Per frame update
void update();
//Renders quad to the screen
void draw();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
SDL_Surface* screenSurface;
SDL_Renderer* renderer;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

SDL_Rect PlayerRect;

const int worldWidth = 11;
const int worldHeight = 7;
const int worldScale = 50;
const float worldxPos = 20;
const float worldyPos = 20;
SDL_Rect world[worldWidth][worldHeight]; //make 3d later for more layers

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
float deltaTime = 0;

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{

			/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
			renderer = SDL_CreateRenderer(gWindow, -1, 0);

			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}
	return success;
}
bool initGL() {
	return true;
}


float xVal[2] = { 200.0f, 0 }; //[0] = pos, [1] = velocity, [2] = acceleration
float yVal[2] = { 200.0f, 0 };

bool keys[4] = { false, false, false, false}; //w, s, a, d
void handleKeys(unsigned char key, int x, int y, bool down) {
	//Toggle quad
	float scale = 5.0f;
	int movement = -1;
	if (key == 'q')
	{
		if (down)
			quit = true;
		//gRenderQuad = !gRenderQuad;
	}
	else if (key == 'w') {
		movement = UP;
	}
	else if (key == 's') {
		movement = DOWN;
	}
	else if (key == 'a') {
		movement = LEFT;
	}
	else if (key == 'd') {
		movement = RIGHT;
	}
	if (movement >= 0) {
		if (!keys[movement] && down) keys[movement] = true;
		else if (keys[movement] && !down) keys[movement] = false;
	}
}

float absolute(float a) {
	if (a < 0) a *= -1.0f;
	return a;
}


bool boxCollision(SDL_Rect* source, SDL_Rect* target) {
	if (source->x < target->x + target->w &&
		source->x + source->w > target->x &&
		source->y < target->y + target->h &&
		source->y + source->h > target->y) {
		//collision
		return true;
	}
	return false;
}
#define POSITION 0
#define VELOCITY 1



void update() {
	float speed = 0.05f*deltaTime;
	float xPos = xVal[POSITION], yPos = yVal[POSITION];

	if (keys[0]) {
		yVal[VELOCITY] -= speed;
	}
	if (keys[1]) {
		yVal[VELOCITY] += speed;
	}
	if (keys[2]) {
		xVal[VELOCITY] -= speed;
	}
	if (keys[3]) {
		xVal[VELOCITY] += speed;
	}

	float dampening = 0.8f;
	xVal[VELOCITY] *= dampening;
	yVal[VELOCITY] *= dampening;

	xVal[POSITION] += xVal[VELOCITY]; //* deltaTime;
	yVal[POSITION] += yVal[VELOCITY]; //* deltaTime;

	PlayerRect.x = xVal[POSITION];
	PlayerRect.y = yVal[POSITION];
	//uniform space partition, 2d grid of 'terrain'
	//check if allowed to move in each direction
	//MAKE VELOCITY
	bool dirs[4] = { true, true, true, true };
	//for (all squares around the player) {
	SDL_Rect rect;
	rect.x = 250;
	rect.y = 150;
	rect.w = 200;
	rect.h = 200;

	if (boxCollision(&PlayerRect, &rect)) {
		std::cout << "COLLISION\n";
		//collision detected
		xVal[POSITION] = xPos;
		yVal[POSITION] = yPos;

		xVal[VELOCITY] = 0;
		yVal[VELOCITY] = 0;
	}

	
}
/*
const int worldWidth = 5;
const int worldHeight = 5;
const int worldScale = 10;
const float worldxPos = 40;
const float worldyPos = 20;
SDL_Rect world[worldWidth][worldHeight];
*/

//world's cells outline RGB
int wCol[3] = { 255, 50, 50 };
int w2Col[3] = { 200, 200, 10 };
void drawWorld() {
	//assuming its filled, will get nullptr err without init the world[][]
	int px = PlayerRect.x, py = PlayerRect.y;
	int pw = PlayerRect.w, ph = PlayerRect.h;
	for (int i = 0; i < worldWidth; i++) {
		for (int j = 0; j < worldHeight; j++) {
			if (px < worldxPos+(i+1.5)*worldScale && (double)px+(double)pw > worldxPos+(i-0.5)*worldScale
			 && py < worldyPos+(j+1.5)*worldScale && (double)py+(double)ph > worldyPos+(j-0.5)*worldScale)
				SDL_SetRenderDrawColor(renderer, w2Col[0], w2Col[1], w2Col[2], 255);
			else
				SDL_SetRenderDrawColor(renderer, wCol[0], wCol[1], wCol[2], 255);
			
			SDL_RenderDrawRect(renderer, &world[i][j]);
		}
	}
}

void draw() {
	//Fill the surface white
	//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x08, green, 0xFF));

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	drawWorld();

	SDL_Rect rect;
	rect.x = 250;
	rect.y = 150;
	rect.w = 200;
	rect.h = 200;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//SDL_RenderDrawRect(renderer, &rect);
	SDL_RenderFillRect(renderer, &rect);

	//Draw Player
	SDL_SetRenderDrawColor(renderer, 255, 127, 49, 255);
	SDL_RenderFillRect(renderer, &PlayerRect);

	//SDL_Rect rect2 = PlayerRect;
	//SDL_RenderDrawRect(renderer, &PlayerRect);


	//Update the surface
	//	CPU -- SDL_UpdateWindowSurface(gWindow);
	// GPU ("Draw" call being sent)
	SDL_RenderPresent(renderer);
}

void close() {
	//Destroy window	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{

		//Get window surface
		screenSurface = SDL_GetWindowSurface(gWindow);

		//init player
		PlayerRect.x = SCREEN_WIDTH / 2.0f;
		PlayerRect.y = SCREEN_HEIGHT / 2.0f;
		PlayerRect.w = 30;
		PlayerRect.h = 50;
		
		//init world
		for (int i = 0; i < worldWidth; i++) {
			for (int j = 0; j < worldHeight; j++) {
				world[i][j].x = worldxPos + (i * worldScale); // - i);
				world[i][j].y = worldyPos + (j * worldScale); // -j);
				world[i][j].w = worldScale;
				world[i][j].h = worldScale;

			}
		}
		/*
		const int worldWidth = 5;
		const int worldHeight = 5;
		const int worldScale = 10;
		const float worldxPos = 40;
		const float worldyPos = 20;
		SDL_Rect world[worldWidth][worldHeight];
		*/

		//Main loop flag
		quit = false;

		//Event handler
		SDL_Event e;

		//Enable text input
		SDL_StartTextInput();

		//While application is running
		while (!quit)
		{
			//deltaTime calculation from: https://gamedev.stackexchange.com/a/110831
			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
			
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				//Handle keypress with current mouse position
				else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				{
					bool down = (e.type == SDL_KEYDOWN);
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					std::cout << "Key " << e.key.keysym.sym << "\n";//text.text[0] << "\n";
					handleKeys(e.key.keysym.sym, x, y, down);
					//std::cout << "Key " << e.text.text[0] << "\n";
					//handleKeys(e.text.text[0], x, y, down);
				}
			}
			update();
			//Render quad
			draw();

			//Update screen
			//SDL_GL_SwapWindow(gWindow);
		}

		//Disable text input
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;
}