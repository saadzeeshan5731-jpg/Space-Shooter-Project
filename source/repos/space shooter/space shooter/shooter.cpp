#include "raylib.h"
#include <iostream>
#include <cstdlib>  
#include<fstream>
 

using namespace std;

const int width_sc = 1000;                      //screen dimensions
const int height_sc = 600;
const int Header_h = 50;
const int start = Header_h;                    //starting point for enemies                       


// BULLETS
const int Bmax = 100;
int bulletsX[Bmax] = { 0 };                         //bullet positions
int bulletsY[Bmax] = { 0 };                                           //bullet positions
bool bulletStatus[Bmax] = { false };
int bullet_idx = 0;                                  //current bullet index
int bulletSpeed = 8;                                 //bullet speed

// ENEMIES
const int Ecount = 15;                                             //number of enemies
struct Enemy {                                                     //variables for enemies
    Rectangle rect;
    float speed;
    bool active;
    Color color;
    int scoreValue;
};
Enemy enemies[Ecount]; // Array of enemies

// BOSS
Rectangle bossRect = { width_sc / 2 - 50, 100, 120, 60 };       //boss shape
int bossHP = 50;
float bossSpeed = 8;                                              //boss speed
bool bossDirectionRight = true;                         //boss movement direction


const int BOSSBMAX = 10;
Rectangle bossBullets[BOSSBMAX];                                 //boss fight bullets
bool bossBulletActive[BOSSBMAX] = { false };
float bossBulletSpeed = 10;


int score = 0;
int lives = 3;                                          //player variables
int current_level = 1;
int gState = 0;
int highScore = 0;
const char* HighScore ="C:\\Users\\lenovo\\source\\repos\\space shooter\\space shooter\\highScore.txt"; // path of the high score file specified
void SaveScore() {
    if (score > highScore)                                                                              // function to save the high score
    {
        highScore = score;

        ofstream outFile("HighScore");
        if (outFile.is_open()) {
            outFile << highScore << endl;

            outFile.close();
        }
    }
   
}
void LoadScore() {                                                               // function to load the high score after the game 
    ifstream inFile("HighScore");
    if (inFile.is_open()) {
        if (!(inFile >> highScore))
        {
            highScore = 0;
        }
        inFile.close();
    }
    else
        highScore = 0;
   
}


int pos_x = width_sc / 2;
int pos_y = height_sc - 60;
int width = 45;
int height = 20;
int playerSpeed = 8;


//all game functions
void Shoot(int playerX, int playerY, int pWidth);         //function prototypes
void UpdateBullets();                                      //update bullets
void ResetGame();                                         //reset game variables
void InitializeEnemies();                                //initialize enemies
void RespawnEnemy(int i);                                //respawn enemy when dead
void UpdateBoss();                                       //update boss position
void BossShoot();                                        //boss shooting
void UpdateBossBullets();                               //boss bullets updated according to position

void Shoot(int playerX, int playerY, int Width) {
	bulletsX[bullet_idx] = playerX + Width / 2;                //position of bullet from center of player ship
	bulletsY[bullet_idx] = playerY;                            //position of bullet at player ship
    bulletStatus[bullet_idx] = true;
    bullet_idx++;
	//move to next bullet
    if (bullet_idx >= Bmax)
    {
        bullet_idx = 0;                                     //reset the bullets when greater than max or equal to it
    }
}

void UpdateBullets() {
    for (int i = 0; i < Bmax; i++) {
        if (bulletStatus[i]) {
            bulletsY[i] -= bulletSpeed;                                     //shooted in vertical direction
            if (bulletsY[i] < 0)
            {
                bulletStatus[i] = false;
            }
        }
    }
}


void RespawnEnemy(int i) {
	enemies[i].rect.y = start - enemies[i].rect.height - (rand() % 500);          //respawn above the screen but after some distance
	enemies[i].rect.x = rand() % (width_sc - (int)enemies[i].rect.width);            //random x position on grid
}

void InitializeEnemies() {
	srand(time(0));        //seed for random generator
	int spawnset = 0;                   //to set distance between enemies

    for (int i = 0; i < Ecount; i++) {

        enemies[i].rect.width = 30;                     //enemies dimensions
        enemies[i].rect.height = 30;
		enemies[i].speed = 0.75 + (current_level * 0.5);    //enemy speed increases with level increment
        enemies[i].active = true;

		enemies[i].rect.x = rand() % (width_sc - (int)enemies[i].rect.width + 1);     //random x position on screen

		enemies[i].rect.y = Header_h - enemies[i].rect.height - (20 + rand() % 100);                               //initial y position above the screen
		enemies[i].rect.y -= spawnset;                  //set distance between enemies when initialized

		spawnset += 80;      //increase distance for next enemy by 80(optional how much to take)

        if (i % 3 == 0) {
            enemies[i].color = RED;
			enemies[i].scoreValue = 50; //red enemies have more score
        }
        else if (i % 3 == 1) {
            enemies[i].color = BLUE;
			enemies[i].scoreValue = 40;            //blue enemies have medium score
        }
        else {
            enemies[i].color = GREEN;
			enemies[i].scoreValue = 20;        //green enemies have less score
        }
    }
}



void ResetGame() {
    score = 0;
    lives = 3;                                              //when gameover the variables reset
    current_level = 1;
    pos_x = width_sc / 2;
    pos_y = height_sc - 60;

	bullet_idx = 0;                                           //reset bullet index

    for (int i = 0; i < Bmax; i++)
    {
        bulletStatus[i] = false;
	}                                                           //reset all bullets
    bossHP = 50;
	bossRect = { width_sc / 2 - 50, 100, 120, 60 };            //reset boss position

	InitializeEnemies();                                      //reinitialize enemies
}

void UpdateBoss() {                                                 
	if (bossDirectionRight) {                                //move right
        bossRect.x += bossSpeed;
		if (bossRect.x + bossRect.width >= width_sc)                  //boss direction when hits screen edge changes accordingly
        {
            bossDirectionRight = false;
        }
    }
    else {
		bossRect.x -= bossSpeed;                                               //move left
        if (bossRect.x <= 0)
        {
            bossDirectionRight = true;
        }
    }

    if (rand() % 40 == 0)
    {
		BossShoot();                                                          //random shooting by boss
    }
}

void BossShoot() {
    for (int i = 0; i < BOSSBMAX; i++) {
        if (!bossBulletActive[i]) {
			bossBullets[i] = { bossRect.x + bossRect.width / 2 - 5, bossRect.y + bossRect.height, 10, 15 };       //boss bullet shape
            bossBulletActive[i] = true;
            break;
        }
    }
}

void UpdateBossBullets() {
	for (int i = 0; i < BOSSBMAX; i++) {      //update boss bullets
        if (bossBulletActive[i]) {
            bossBullets[i].y += bossBulletSpeed;

            if (bossBullets[i].y > height_sc)
                bossBulletActive[i] = false;

			Rectangle playerRect = { pos_x, pos_y, width, height };           //player rectangle for collision

			if (CheckCollisionRecs(playerRect, bossBullets[i]))             //collision detection
            {
                lives--;
                bossBulletActive[i] = false;
            }
        }
    }
}

//main loop GUI
int main() {
    InitWindow(width_sc, height_sc, "SPACE SHOOTER!");
    LoadScore();
	InitAudioDevice();

    SetTargetFPS(60);
    Music backgroundMusic = LoadMusicStream("assets/game.wav.wav");
    Texture2D playerTexture = LoadTexture("assets/extra.png");
    Texture2D enemyTexture = LoadTexture("assets/red.png");
	Texture2D enemy2Texture = LoadTexture("assets/yellow.png");                              //texture loading of player and enemies and boss
    Texture2D enemy3Texture = LoadTexture("assets/green.png");
	Texture2D bossTexture = LoadTexture("assets/boss.png");
    PlayMusicStream(backgroundMusic);
    Sound Gunshot=LoadSound("assets/gunshot.wav");
    InitializeEnemies();
    srand(time(0));

    while (WindowShouldClose()!=true) {
        float x = GetFrameTime();  //frame rate
        UpdateMusicStream(backgroundMusic);
        
        
        if (gState == 0) {
            if (IsKeyPressed(KEY_SPACE))
            {
                gState = 1;
                PlaySound(Gunshot);
            }
        }

        if (lives <= 0)
        {
            if (gState != 2)
            {
                SaveScore();
           }
            gState = 2;
           
        }
        if (IsKeyPressed(KEY_R) && (gState == 1 || gState == 2 || gState == 3)) {
            ResetGame();
            gState = 1;
        }

        if (gState == 1) {
			//program was crashing with conio.h and _kbhit so used raylib key functions
           
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))       /*char key;
                                                                  if(_kbhit)
                                                                  kbhit=_getch(); for movement */
            {
                pos_x += playerSpeed;
            }
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
            {
                pos_x -= playerSpeed;
            }

            if (pos_x < 0)
            {
                pos_x = 0;
            }
            if (pos_x > width_sc - width)
            {
                pos_x = width_sc - width;
            }


            if (IsKeyPressed(KEY_SPACE))
            {
                Shoot(pos_x, pos_y, width);
            }
            UpdateBullets();

            Rectangle playerRect = { pos_x, pos_y, width, height };


            if (current_level < 3) {
				for (int i = 0; i < Ecount; i++) {                             //update enemies
					enemies[i].rect.y += enemies[i].speed * x * 60;              //enemy speed

                    if (enemies[i].rect.y > height_sc) {
                        lives--;
                        RespawnEnemy(i);
                    }

                    if (CheckCollisionRecs(playerRect, enemies[i].rect)) {
                        lives--;
                        RespawnEnemy(i);
                    }

					for (int j = 0; j < Bmax; j++) {                //player bullets hit enemies
                        if (bulletStatus[j]) {
							Rectangle bulletRect = { bulletsX[j] - 2, bulletsY[j] - 5, 5, 10 };   //bullet shape

                            if (CheckCollisionRecs(bulletRect, enemies[i].rect)) {
                                score += enemies[i].scoreValue;
                                bulletStatus[i] = false;
                                RespawnEnemy(i);
                                break;
                            }
                        }
                    }
                }

				if (score >= current_level * 800) {                         //level up condition
                    current_level++;
                    if (current_level == 3) {
						                      //boss level
                    }
                }
            }


            if (current_level == 3) {
                UpdateBoss();                                //Boss Fight
                UpdateBossBullets();

                // Player bullets hit boss
                for (int b = 0; b < Bmax; b++) {
                    if (bulletStatus[b]) {
                        Rectangle bRect = { (float)bulletsX[b], (float)bulletsY[b], 5, 10 };
						if (CheckCollisionRecs(bRect, bossRect)) {       //collision detection
                            bossHP-=3;
							score += 10;
							bulletStatus[b] = false;         //deactivate bullet
                        }                                    
                    }
                }

                if (bossHP <= 0) {
                    gState = 3; // WIN!
                }
            }
        }


        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangle(0, 0, width_sc, Header_h, DARKGRAY);
        DrawLine(0, Header_h, width_sc, Header_h, RAYWHITE);

        DrawText("SPACE SHOOTER", 10, 15, 20, RAYWHITE);
		DrawText("LEVEL", 250, 15, 20, LIGHTGRAY);                             //header information
        DrawText(TextFormat("%i", current_level), 330, 15, 20, YELLOW);
        DrawText("SCORE", 420, 15, 20, LIGHTGRAY);
        DrawText(TextFormat("%05i", score), 500, 15, 20, WHITE);
        DrawText("HIGH SCORE", 600, 15, 20, LIGHTGRAY);
        DrawText(TextFormat("%05i", highScore), 740, 15, 20, LIGHTGRAY);
        DrawText("LIVES", 830, 15, 20, LIGHTGRAY);
        DrawText(TextFormat("%i", lives), 900, 15, 20, RED);

        if (gState == 0) {
            DrawText("THE CHAOTIC TRIO", width_sc / 2 - 200, 110, 40, WHITE);
            DrawText("INSTRUCTIONS", width_sc / 2 - 200, 160, 40, RED);
            DrawText("Use A/D to move", 300, 330, 20, GREEN);
            DrawText("Press SPACE to Fire", 300, 360, 20, GREEN);
            DrawText("Hit Debris for Score", 300, 390, 20, GREEN);
            DrawText("Press R to Restart", 300, 420, 20, GREEN);
            DrawText("Press Space to Start", 300, 450, 20, GREEN);

        }

        if (gState == 1) {

            // DRAW PLAYER
			Rectangle playerSrcRec = { 0.0f, 0.0f, (float)playerTexture.width, (float)playerTexture.height }; //dimensions of player ship
			Rectangle playerDestRec = { (float)pos_x, (float)pos_y, (float)width, (float)height };                                       //destination rectangle for player ship
            DrawTexturePro(playerTexture, playerSrcRec, playerDestRec, { 0,0 }, 0.0f, WHITE);  //draw player ship

            // DRAW BULLETS
            for (int i = 0; i < Bmax; i++) {
                if (bulletStatus[i])
                    DrawRectangle(bulletsX[i], bulletsY[i], 5, 10, YELLOW);
            }

            if (current_level < 3) {
                for (int i = 0; i < Ecount; i++) {

                    if (enemies[i].rect.y >= Header_h + 5) {

                        
                        Texture2D* enemyTex = &enemy3Texture;

                        if (i % 3 == 0)
                        {
                            enemyTex = &enemyTexture; // Red enemy
                        }
                        else if (i % 3 == 1)
                        {
                            enemyTex = &enemy2Texture; // Yellow enemy
                        }
                        else 
                            enemyTex = &enemy3Texture; // Green enemy

                       
						Rectangle enemySrcRec = { 0.0f, 0.0f, (float)enemyTex->width, (float)enemyTex->height };       //enemy dimensions
						DrawTexturePro(*enemyTex, enemySrcRec, enemies[i].rect, { 0,0 }, 0.0f, WHITE);     //draw enemies
                     
                    }

                }
            }

            if (current_level == 3) {
				Rectangle bossRec = { 0.0f, 0.0f, (float)bossTexture.width, (float)bossTexture.height };      //boss dimensions
				DrawTexturePro(bossTexture, bossRec, bossRect, { 0,0 }, 0.0f, WHITE);                                          //draw boss

                for (int i = 0; i < BOSSBMAX; i++) {
                    if (bossBulletActive[i])
                        DrawRectangleRec(bossBullets[i], RED);
                }

                DrawText(TextFormat("BOSS HP: %i", bossHP), width_sc / 2 - 50, 60, 20, RED);
            }
        }

        if (gState == 2) {
			DrawText("GAME OVER!", width_sc / 2 - 150, height_sc / 2 - 50, 60, RED);         //game over condition
        }

        if (gState == 3) {
			DrawText("YOU WIN!", width_sc / 2 - 150, height_sc / 2 - 50, 60, GREEN);//win condition
        }

        EndDrawing();
    }
    UnloadTexture(playerTexture);
	UnloadTexture(enemyTexture);                   //unload textures from memory
    UnloadTexture(enemy2Texture);
    UnloadTexture(enemy3Texture);
	UnloadTexture(bossTexture);
    StopMusicStream(backgroundMusic);
    UnloadSound(Gunshot);
    UnloadMusicStream(backgroundMusic);
    SaveScore();
   

    CloseAudioDevice();


    CloseWindow();
    return 0;
}