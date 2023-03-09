#include "raylib.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define LINHAS_MATRIZ_PRINCIPAL 28
#define COLUNAS_MATRIZ_PRINCIPAL 18
#define TAMANHO_BORDA 40
#define NUM_PROCESSOS_MENU 3

void inicializa_janela(void);
void sair(void);
void ranking(void);
//void ler_item(char*, char);

int menu(void);
int score(void);
int vidas(void);
int escadas(void);
int energia(void);
int fase(void);

char metais_pesados(int);
char metais_preciosos(int);

int global_contador_semente = 0;
int global_contador_metal_pesado = 1;
int global_contador_metal_precioso = 1;

float global_posicao_x = TAMANHO_BORDA, global_posicao_y = TAMANHO_BORDA;

Vector2 posicao_personagem = { (float) TAMANHO_BORDA, (float) TAMANHO_BORDA };

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
        inicializa_janela();
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

void inicializa_janela(void){

    // Configurações da tela
    const int screenWidth = 1200;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Tela principal");

    // Define o jogo para rodar a 60 quadros por segundo
    SetTargetFPS(60);

    // Converte imagem para textura
    Image borda = LoadImage("resources/imagens/borda.png");
    Texture2D textura_borda = LoadTextureFromImage(borda);  
    UnloadImage(borda);

    Image terra_escura = LoadImage("resources/imagens/fundo_terra.png");
    Texture2D textura_terra_escura = LoadTextureFromImage(terra_escura);  
    UnloadImage(terra_escura);

    Image terra_clara = LoadImage("resources/imagens/terra.png");
    Texture2D textura_terra_clara = LoadTextureFromImage(terra_clara);  
    UnloadImage(terra_clara);

    Image minerio = LoadImage("resources/imagens/minerio.png");
    Texture2D textura_minerio = LoadTextureFromImage(minerio);  
    UnloadImage(minerio);

    Image personagem = LoadImage("resources/imagens/jogador.png");
    Texture2D textura_personagem = LoadTextureFromImage(personagem);  
    UnloadImage(personagem);

    // Detectar botão de fechar janela ou tecla ESC
    while (!WindowShouldClose())
    {

    int matriz_principal[LINHAS_MATRIZ_PRINCIPAL][COLUNAS_MATRIZ_PRINCIPAL];
    int contador_pontuacao, contador_vidas, contador_escadas, contador_energia, contador_fase;
    int posX, posY;
    

    char item[30];
    //char minerio_escavado = 'S';

    contador_pontuacao = score();
    contador_vidas = vidas();
    contador_escadas = escadas();
    contador_energia = energia();
    contador_fase = fase();


    //ler_item(item, minerio_escavado);

    // Desenha a janela
    BeginDrawing();

    // Limpa a tela
    ClearBackground(RAYWHITE);

    // DEFINE O CABEÇALHO DA TELA PRINCIPAL
    DrawRectangle(0, 0, screenWidth, 40, BLACK);
        DrawText(TextFormat("SCORE: %d", contador_pontuacao), 40, 10, 20, GRAY);
        DrawText(TextFormat("VIDAS: %d", contador_vidas), 200, 10, 20, GREEN);
        DrawText(TextFormat("ENERGIA: %d", contador_energia), 370, 10, 20, YELLOW);
        DrawText(TextFormat("ESCADAS: %d", contador_escadas), 570, 10, 20, MAROON);
        DrawText(TextFormat("FASE: %d", contador_fase), 780, 10, 20, BLUE);
        DrawText(TextFormat("ITEM: %s", item), 920, 10, 20, PURPLE);


    // PREENCHE BORDAS DA TELA
        for(posX = TAMANHO_BORDA; posX < screenWidth - TAMANHO_BORDA; posX = posX + TAMANHO_BORDA){
            // EMBAIXO
                DrawTexture(textura_borda, posX, screenHeight - TAMANHO_BORDA, WHITE);
        }

        for(posY = TAMANHO_BORDA; posY < screenHeight; posY = posY + TAMANHO_BORDA){
            // LATERAL ESQUERDA
                DrawTexture(textura_borda, 0, posY, WHITE);
            //LATERAL DIREITA
                DrawTexture(textura_borda, screenWidth - TAMANHO_BORDA, posY, WHITE);
        }

    // DEFINE AS BORDAS E CABEÇALHO COMO INACESSÍVEIS AO PERSONAGEM (tá com problema aqui)
    int linha, coluna;

        for(coluna = 0; coluna < COLUNAS_MATRIZ_PRINCIPAL; coluna++){
            matriz_principal[0][coluna] = 1;
        }
        for(linha = 0; linha < LINHAS_MATRIZ_PRINCIPAL; linha++){
            matriz_principal[linha][0] = 1;
        }



    // PREENCHE AS DUAS PRIMEIRAS LINHAS DA MATRIZ COM ESPAÇO VAZIO (TERRA ESCURA)
    
    for(linha = 0; linha < LINHAS_MATRIZ_PRINCIPAL; linha++){
        for(coluna = 0; coluna < 2; coluna++){
            matriz_principal[linha][coluna] = 0;
            DrawTexture(textura_terra_escura, TAMANHO_BORDA + linha*TAMANHO_BORDA, TAMANHO_BORDA + coluna*TAMANHO_BORDA, WHITE);
        }
    }

    // PREENCHE O RESTANTE DA MATRIZ COM GERAÇÃO ALEATÓRIA DE ESPAÇOS
        // Garanto que 2/3 dela esteja ocupada com "1s" e 1/3 com "0s"
        // Notação: "0s" virarão espaços disponíveis para o personagem (terra escura) e "1s" terão pedras/terra

    srand(global_contador_semente); // se passar de nível, contador incrementa

    int aleatorio;
    
    for(linha = 0; linha < LINHAS_MATRIZ_PRINCIPAL; linha++){
        for(coluna = 2; coluna < COLUNAS_MATRIZ_PRINCIPAL; coluna++){
            aleatorio = (rand() % ((1)+1));
            matriz_principal[linha][coluna] = aleatorio;

            if(matriz_principal[linha][coluna] == 0){
                aleatorio = (rand() % ((1)+1));
                matriz_principal[linha][coluna] = aleatorio;
            }
            
        }
    }

    for(linha = 0; linha < LINHAS_MATRIZ_PRINCIPAL; linha++){
        for(coluna = 2; coluna < COLUNAS_MATRIZ_PRINCIPAL; coluna++){
            if(matriz_principal[linha][coluna] == 0){
                    // PREENCHE ZEROS COM TERRA ESCURA
                    DrawTexture(textura_terra_escura, TAMANHO_BORDA + linha*TAMANHO_BORDA, TAMANHO_BORDA + coluna*TAMANHO_BORDA, WHITE);
                }
            else{
                //DO ESPAÇO DISPONÍVEL COM UNS:
                    aleatorio = (rand() % (10));
                    //PREENCHE 80% COM TERRA CLARA
                    if(aleatorio >= 0 && aleatorio < 8){
                        DrawTexture(textura_terra_clara, TAMANHO_BORDA + linha*TAMANHO_BORDA, TAMANHO_BORDA + coluna*TAMANHO_BORDA, WHITE);
                    }
                    else if(aleatorio == 8){
                        //PREENCHE 10% COM METAIS PRECIOSOS
                        DrawTexture(textura_minerio, TAMANHO_BORDA + linha*TAMANHO_BORDA, TAMANHO_BORDA + coluna*TAMANHO_BORDA, WHITE);
                        // DEFINE A DISTRIBUIÇÃO DE PRATA, OURO E TITÂNIO
                        matriz_principal[linha][coluna] = metais_pesados(global_contador_metal_precioso);
                    }
                    else if(aleatorio == 9){
                        //PREENCHE 10% COM METAIS PESADOS
                        DrawTexture(textura_minerio, TAMANHO_BORDA + linha*TAMANHO_BORDA, TAMANHO_BORDA + coluna*TAMANHO_BORDA, WHITE);
                        // DEFINE 50% PARA URÂNIO E 50% PARA TITÂNIO
                        matriz_principal[linha][coluna] = metais_pesados(global_contador_metal_pesado);
                    }
            }
        }
    }

    // DEFINO QUE O PERSONAGEM COMEÇA SEMPRE NO X = 0 E Y = 0
    DrawTextureV(textura_personagem, posicao_personagem, WHITE);


    // DEFINE O MOVIMENTO DO PERSONAGEM

    if (IsKeyDown(KEY_RIGHT)){
        if((matriz_principal[(int)((posicao_personagem.x + 0.2)/TAMANHO_BORDA)][(int)(posicao_personagem.y/TAMANHO_BORDA)]) == 0){
            posicao_personagem.x += 4.0f;
        }
    }
    if (IsKeyDown(KEY_LEFT)){
        if((matriz_principal[(int)((posicao_personagem.x  - 0.6)/TAMANHO_BORDA)][(int)((posicao_personagem.y)/TAMANHO_BORDA)]) == 0){
            posicao_personagem.x -= 4.0f;
        }
    }
    if (IsKeyDown(KEY_UP)){
        if((matriz_principal[(int)(posicao_personagem.x/TAMANHO_BORDA)][(int)((posicao_personagem.y - 0.6)/TAMANHO_BORDA)]) == 0){
            //if(posicao_personagem.x && posicao_personagem.y)
        //} && contador_escadas > 0){
            posicao_personagem.y -= 4.0f;
        }
        /*else{
            DrawRectangle(screenWidth/2 - 200, screenHeight/2 - 100, 400, 200, RAYWHITE);
                DrawText(TextFormat("VOCE NAO TEM ESCADAS\nDISPONIVEIS!"), screenWidth/2 - 200, screenHeight/2 - 50, 25, BLACK);
        }*/
    }
    if (IsKeyDown(KEY_DOWN)){
        if((matriz_principal[(int)(posicao_personagem.x/TAMANHO_BORDA)][(int)((posicao_personagem.y  + 0.2)/TAMANHO_BORDA)]) == 0){
            posicao_personagem.y += 4.0f;
        }
    }

    EndDrawing();
    }

    // Desabilita as texturas
    UnloadTexture(textura_borda);
    UnloadTexture(textura_terra_escura); 
    UnloadTexture(textura_terra_clara); 
    UnloadTexture(textura_minerio); 
    UnloadTexture(textura_personagem); 

    // Fecha a janela
    CloseWindow();
}

char metais_pesados(int contador){

    //Define que a cada chamada de função altera-se de césio para urânio o metal pesado colocado na matriz

    if(contador == 1){
        return 'C';
        contador = 0;
    }
    else{
        return 'U';
        contador = 1;
    }
}

char metais_preciosos(int contador){

    //Define que a cada chamada de função altera-se de prata para ouro e titânio o metal precioso colocado na matriz

    if(contador == 1){
        return 'S';
        contador = 2;
    }
    else if(contador == 2){
        return 'G';
        contador = 0;
    }
    else{
        return 'T';
        contador = 1;
    }
}


int score(void){

    //retorna a pontuação
    int score = 000;

    if(score == 1000){
        //PULA DE NÍVEL
            //ATUALIZO A SEMENTE
            global_contador_semente++;
    }

    return score;
}

int vidas(void){

    //retorna a quantidade de vidas restantes
    int contador_vidas = 3;

    return contador_vidas;
}

int escadas(void){

    //retorna a quantidade de escadas restantes
    int escadas = 10;

    return escadas;
}

int energia(void){

    //retorna a quantidade de energia restantes
    int energia = 50;

    return energia;
}

int fase(void){

    //retorna a fase
    int fase = 1;

    return fase;
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

/* void ler_item(char * item, char minerio_escavado){

    char * intermediario = item;

    //retorna o utlimo item
    switch(minerio_escavado){
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
}
*/