/*
 * File:         main.c
 * Version:      0.1
 * Authors:      Maria Fernanda Negreli and Pedro Lima
 * Date Created: 2023-03-07
 */

// Includes
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Defines
#define GAME_FPS               60

#define SCREEN_WIDTH         1200
#define SCREEN_HEIGHT         800

#define LEVEL_MATRIX_ROWS      28
#define LEVEL_MATRIX_COLUMNS   18
#define LEVEL_EDGE_SIZE        40
#define NUM_PROCESSOS_MENU      3

// Function Prototypes
void open_window(void);
//void sair(void);
void ranking(void);
//void item_read(char*, char);

int menu(void);
int player_score(void);
int player_hp(void);
int player_ladder(void);
int player_energy(void);
int level(void);

char heavy_ores(int);
char valuable_ores(int);

// Global Variables
int global_seed_counter         = 0;
int global_heavy_ore_counter    = 1;
int global_valuable_ore_counter = 1;

float global_x_position = LEVEL_EDGE_SIZE;
float global_y_position = LEVEL_EDGE_SIZE;

Vector2 player_coordinates = { (float) LEVEL_EDGE_SIZE, (float) LEVEL_EDGE_SIZE };

typedef enum {
    INICIA_JOGO,
    RANKING,
    SAIR
} Menu;

static const char *enunciados_menu[] = {
    "-> NOVO JOGO <-",
    "RANKING DE PONTOS",
    "SAIR"
};

int main(void)
{
    int flag_entra_jogo;

    flag_entra_jogo = menu();

    if(flag_entra_jogo == 1){
        open_window();
    }
    else if(flag_entra_jogo == 2){
       ranking();
    }
    else if(flag_entra_jogo == 0){
        return 0;
    }
    
}

int menu(void)
{
    // Configurações da tela:
    const int screenWidth = 1200;
    const int screenHeight = 800;

    // Inicialização de variaveis
    int flag = 4; //Bandeira de retorno
    int grava_foco_mouse = -1;
    int iterador_processos = 0;
    bool booleano_textura_capa = false;
    Rectangle alterna_gravacao_focos_mouse[NUM_PROCESSOS_MENU] = { 0 };

    InitWindow(screenWidth, screenHeight, "Menu");

    // Converte imagem para textura
        Image capa_original = LoadImage("resources/imagens/capa.png"); 

        // Converte imagem para 32 bit (necessario para usar como textura) 
        ImageFormat(&capa_original, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);        
        Texture2D textura_capa = LoadTextureFromImage(capa_original);    

    Image capa_copia = ImageCopy(capa_original);

    for (int i = 0; i < NUM_PROCESSOS_MENU; i++)  alterna_gravacao_focos_mouse[i] = (Rectangle){ (float)(screenWidth/2-150.0/2), (float)(screenHeight/2 + 32*i), 150.0f, 30.0f };

    // Define o jogo para rodar a 60 quadros por segundo
    SetTargetFPS(60);
   
    while (!WindowShouldClose() && flag == 4)    
    {
        
        for (int i = 0; i < NUM_PROCESSOS_MENU; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(),  alterna_gravacao_focos_mouse[i]))
            {
                grava_foco_mouse = i;

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    iterador_processos = i;
                    booleano_textura_capa = true;
                }
                break;
            }
            else grava_foco_mouse = -1;
        }

        // Verifica o mouse
        if (IsKeyPressed(KEY_DOWN))
        {
            iterador_processos++;
            if (iterador_processos > (NUM_PROCESSOS_MENU - 1)) iterador_processos = 0;
            booleano_textura_capa = true;
        }
        else if (IsKeyPressed(KEY_UP))
        {
            iterador_processos--;
            if (iterador_processos < 0) iterador_processos = 7;
            booleano_textura_capa = true;
        }

        // Recarrega a textura da capa
        if (booleano_textura_capa)
        {
            UnloadImage(capa_copia);                
            capa_copia = ImageCopy(capa_original);     

            switch (iterador_processos)
            {
                case INICIA_JOGO: {
                    flag = 1; 
                    break;
                }
                case RANKING: {
                    flag = 2; 
                    break;
                }
                case SAIR: {
                    flag = 0; 
                    return flag;  
                    break;
                }
                
                default: 
                    break;
            }

            Color *pixels = LoadImageColors(capa_copia);    
            UpdateTexture(textura_capa, pixels);             
            UnloadImageColors(pixels);                  

            booleano_textura_capa = false;
        }
      
        BeginDrawing();

            DrawTexture(textura_capa, screenWidth - textura_capa.width, screenHeight/2 - textura_capa.height/2, WHITE);

            // Desenha retangulos, nao botoes!!!!!!
            for (int i = 0; i < NUM_PROCESSOS_MENU; i++)
            {
                DrawRectangleLines((int) alterna_gravacao_focos_mouse[i].x, (int)  alterna_gravacao_focos_mouse[i].y, (int)  alterna_gravacao_focos_mouse[i].width, (int)  alterna_gravacao_focos_mouse[i].height, ((i == iterador_processos) || (i == grava_foco_mouse)) ? RAYWHITE : BLACK); // define a cor da janela quando selecionada (primeira) e quando nao selecionada
                DrawText( enunciados_menu[i], (int)(  alterna_gravacao_focos_mouse[i].x +  alterna_gravacao_focos_mouse[i].width/2 - MeasureText(enunciados_menu[i], 10)/2), (int)  alterna_gravacao_focos_mouse[i].y + 11, 10, ((i == iterador_processos) || (i == grava_foco_mouse)) ? GREEN : MAROON); // define a cor do texto quando selecionado e nao selecionado
            }

        EndDrawing();
    }

    //Desabilita as texturas e imagens
    UnloadTexture(textura_capa);      
    UnloadImage(capa_original);        
    UnloadImage(capa_copia);          

    CloseWindow();               

    return flag;
}

void open_window(void)
{
    // Local Variables
    int level_matrix[LEVEL_MATRIX_ROWS][LEVEL_MATRIX_COLUMNS];
    int player_score_counter, player_hp_counter, player_ladder_counter, player_energy_counter, level_counter;
    int x_pos, y_pos;
    char item[30];

    int row, column;
    int rand_number;

    // Configurações da tela
    const int screenWidth = 1200;
    const int screenHeight = 800;

    // Sets the game to run at 60 frames per second
    SetTargetFPS(GAME_FPS);

    // Loads and converts each image to a texture
    Image edge = LoadImage("textures/sprites/edge.png");
    Texture2D texture_edge = LoadTextureFromImage(edge);  
    UnloadImage(edge);

    Image dark_ground = LoadImage("textures/sprites/ground_background.png");
    Texture2D texture_dark_ground = LoadTextureFromImage(dark_ground);  
    UnloadImage(dark_ground);

    Image light_ground = LoadImage("textures/sprites/ground.png");
    Texture2D texture_light_ground = LoadTextureFromImage(light_ground);  
    UnloadImage(light_ground);

    Image ore = LoadImage("textures/sprites/ore.png");
    Texture2D texture_ore = LoadTextureFromImage(ore);  
    UnloadImage(ore);

    Image player = LoadImage("textures/sprites/player.png");
    Texture2D texture_player = LoadTextureFromImage(player);  
    UnloadImage(player);

    // Checks if KEY_ESCAPE pressed or Close icon aren't pressed
    while (!WindowShouldClose())
    {

        player_score_counter = player_score();
        player_hp_counter = player_hp();
        player_ladder_counter = player_ladder();
        player_energy_counter = player_energy();
        level_counter = level();

        //item_read(item, mined_ore);

        // Draws the window
        BeginDrawing();

        // Clears window's background
        ClearBackground(RAYWHITE);

        // Defines window's header
        DrawRectangle(0, 0, SCREEN_WIDTH, 40, BLACK);
        DrawText(TextFormat("SCORE: %d", player_score_counter), 40, 10, 20, GRAY);
        DrawText(TextFormat("HEALTH POINTS: %d", player_hp_counter), 200, 10, 20, GREEN);
        DrawText(TextFormat("ENERGY: %d", player_energy_counter), 420, 10, 20, YELLOW);
        DrawText(TextFormat("LADDERS: %d", player_ladder_counter), 620, 10, 20, MAROON);
        DrawText(TextFormat("LEVEL: %d", level_counter), 830, 10, 20, BLUE);
        DrawText(TextFormat("ITEM: %s", item), 970, 10, 20, PURPLE);


        // Fills the window's edges
        for(x_pos = LEVEL_EDGE_SIZE; x_pos < SCREEN_WIDTH - LEVEL_EDGE_SIZE; x_pos = x_pos + LEVEL_EDGE_SIZE)
        {
            // Below
            DrawTexture(texture_edge, x_pos, SCREEN_HEIGHT - LEVEL_EDGE_SIZE, WHITE);
        }

        for(y_pos = LEVEL_EDGE_SIZE; y_pos < SCREEN_HEIGHT; y_pos = y_pos + LEVEL_EDGE_SIZE){
            // Left side
            DrawTexture(texture_edge, 0, y_pos, WHITE);
            // Right side
            DrawTexture(texture_edge, SCREEN_WIDTH - LEVEL_EDGE_SIZE, y_pos, WHITE);
        }

        // Defines the edges and header as inaccessible to the player (NOT WORKING)
        for(column = 0; column < LEVEL_MATRIX_COLUMNS; column++)
        {
            level_matrix[0][column] = 1;
        }
        for(row = 0; row < LEVEL_MATRIX_ROWS; row++)
        {
            level_matrix[row][0] = 1;
        }

        // Fills the two first level matrix' rows with a dark ground
        for(row = 0; row < LEVEL_MATRIX_ROWS; row++)
        {
            for(column = 0; column < 2; column++)
            {
                level_matrix[row][column] = 0;
                DrawTexture(texture_dark_ground, LEVEL_EDGE_SIZE + row*LEVEL_EDGE_SIZE, LEVEL_EDGE_SIZE + column*LEVEL_EDGE_SIZE, WHITE);
            }
        }

        /* 
         * Fills the rest of the level matrix with random numbers
         * 
         * It guarantees that 2/3 of it will be filled with "1s" and 1/3 with "0s"
         * 
         * Note: "0s" are free spaces for the player (dark ground) e "1s" will have cobble/ground
         */ 

        srand(global_seed_counter); // After clearing a level, the counter gets incremented
    
        for(row = 0; row < LEVEL_MATRIX_ROWS; row++)
        {
            for(column = 2; column < LEVEL_MATRIX_COLUMNS; column++)
            {
                rand_number = (rand() % ((1)+1));
                level_matrix[row][column] = rand_number;

                if(level_matrix[row][column] == 0)
                {
                    rand_number = (rand() % ((1)+1));
                    level_matrix[row][column] = rand_number;
                }
            }
        }

        for(row = 0; row < LEVEL_MATRIX_ROWS; row++)
        {
            for(column = 2; column < LEVEL_MATRIX_COLUMNS; column++)
            {
                // Fills "0s" with dark ground
                if(level_matrix[row][column] == 0)
                    DrawTexture(texture_dark_ground, LEVEL_EDGE_SIZE + row*LEVEL_EDGE_SIZE, LEVEL_EDGE_SIZE + column*LEVEL_EDGE_SIZE, WHITE);
                else
                {
                    // Related to the player's free space (1s):
                    rand_number = (rand() % (10));

                    // 80 % is light ground
                    if(rand_number >= 0 && rand_number < 8)
                        DrawTexture(texture_light_ground, LEVEL_EDGE_SIZE + row*LEVEL_EDGE_SIZE, LEVEL_EDGE_SIZE + column*LEVEL_EDGE_SIZE, WHITE);
                    else if(rand_number == 8)
                    {
                        // 10 % is valuable ores
                        DrawTexture(texture_ore, LEVEL_EDGE_SIZE + row*LEVEL_EDGE_SIZE, LEVEL_EDGE_SIZE + column*LEVEL_EDGE_SIZE, WHITE);

                        // Defines the distribution of silver, gold and titanium
                        level_matrix[row][column] = heavy_ores(global_valuable_ore_counter);
                    }
                    else if(rand_number == 9){
                        // 10 % is heavy ores
                        DrawTexture(texture_ore, LEVEL_EDGE_SIZE + row*LEVEL_EDGE_SIZE, LEVEL_EDGE_SIZE + column*LEVEL_EDGE_SIZE, WHITE);
                        // Defines 50% uranium and 50 % titanium
                        level_matrix[row][column] = heavy_ores(global_heavy_ore_counter);
                    }
                }
            }
        }

        // Defines that the player always starts at coordinates (0,0)
        DrawTextureV(texture_player, player_coordinates, WHITE);


        // Defines player's moviment
        if (IsKeyDown(KEY_RIGHT))
            if((level_matrix[(int)((player_coordinates.x + 0.2)/LEVEL_EDGE_SIZE)][(int)(player_coordinates.y/LEVEL_EDGE_SIZE)]) == 0)
                player_coordinates.x += 4.0f;
        if (IsKeyDown(KEY_LEFT))
            if((level_matrix[(int)((player_coordinates.x  - 0.6)/LEVEL_EDGE_SIZE)][(int)((player_coordinates.y)/LEVEL_EDGE_SIZE)]) == 0)
                player_coordinates.x -= 4.0f;
        if (IsKeyDown(KEY_UP))
            if((level_matrix[(int)(player_coordinates.x/LEVEL_EDGE_SIZE)][(int)((player_coordinates.y - 0.6)/LEVEL_EDGE_SIZE)]) == 0)
                player_coordinates.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN))
            if((level_matrix[(int)(player_coordinates.x/LEVEL_EDGE_SIZE)][(int)((player_coordinates.y  + 0.2)/LEVEL_EDGE_SIZE)]) == 0)
                player_coordinates.y += 4.0f;
        
        EndDrawing();
    }

    // Unloads all textures
    UnloadTexture(texture_edge);
    UnloadTexture(texture_dark_ground); 
    UnloadTexture(texture_light_ground); 
    UnloadTexture(texture_ore); 
    UnloadTexture(texture_player); 

    // Closes the window
    CloseWindow();
}

char heavy_ores(int counter)
{

    // Defines that at each function call, changes from cesium to uranium the heavy ore passed to this function
    if(counter == 1)
    {
        return 'C';
        counter = 0;
    }
    else
    {
        return 'U';
        counter = 1;
    }
}

char valuable_ores(int counter){

    // Defines that at each function call, changes from silver to gold, to titanium the valuable ore passed to this function

    if(counter == 1){
        return 'S';
        counter = 2;
    }
    else if(counter == 2){
        return 'G';
        counter = 0;
    }
    else{
        return 'T';
        counter = 1;
    }
}


int player_score(void){

    // Returns score
    int player_score = 000;

    if(player_score == 1000){
        // Levels-up means updating the level seed
            global_seed_counter++;
    }

    return player_score;
}

int player_hp(void){

    // Retorns player's remaining health points
    int player_hp_counter = 3;

    return player_hp_counter;
}

int player_ladder(void){

    // Returns player's remaining ladders
    int player_ladder = 10;

    return player_ladder;
}

int player_energy(void){

    // Returns player's remaining energy
    int player_energy = 50;

    return player_energy;
}

int level(void){

    // Returns level
    int level = 1;

    return level;
}

void ranking(void){

    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Ranking");

    while (!WindowShouldClose())
    {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
    }

    CloseWindow();
}

/* /*
void item_read(char * item, char mined_ore){

    char * intermediario = item;

    // Returns last item
    switch(mined_ore){
        case 'G':
        {
            intermediario = 'OURO';
            break;
        }
        case 'T':
        {
            intermediario = 'TITANIO';
            break;
        }
        case 'S':
        {
            intermediario = 'PRATA';
            break;
        }
        case 'U':
        {
            intermediario = 'URANIO';
            break;
        }
        case 'C':
        {
            intermediario = 'CESIO';
            break;
        }
        default:
        {
            intermediario = 'VAZIO';
        }
    }
}*/