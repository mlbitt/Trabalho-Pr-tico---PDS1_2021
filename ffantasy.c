#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NAVEGACAO 0
#define BATALHA 1
#define TAM_HERO 20
#define PASSO 20
#define PRA_CIMA 1
#define PRA_BAIXO 2
#define PRA_ESQUERDA 3
#define PRA_DIREITA 4 
#define ATACAR 0
#define ESPECIAL 1
#define FUGIR 2


const float FPS = 60;  

//numero de pixels da tela
const int SCREEN_W = 800; //largura
const int SCREEN_H = 800; //altura


//estrutura com parametros que formam um inimigo.
typedef struct {
    int posicao_x;
    int posicao_y;
    int level;
    int hp;
} INIMIGO;

 
//desenha o player com base na ultima tecla direcional pressionada. Curframe é dinamico e alterna entre as imagens do sprite
void desenhaBatNaveg(int direcao, int curFrame, int larguraBat, int alturaBat, int xBat, int yBat, ALLEGRO_BITMAP *bat_up, ALLEGRO_BITMAP *bat_down, ALLEGRO_BITMAP *bat_left, ALLEGRO_BITMAP *bat_right){
    if(direcao == PRA_CIMA)
    al_draw_bitmap_region(bat_up, curFrame * larguraBat, 0, larguraBat, alturaBat, xBat, yBat, 0);
  else if(direcao == PRA_BAIXO)
    al_draw_bitmap_region(bat_down, curFrame * larguraBat,0, larguraBat, alturaBat, xBat, yBat, 0);
  else if(direcao == PRA_ESQUERDA)
    al_draw_bitmap_region(bat_left, curFrame * larguraBat-2, 0, larguraBat, alturaBat, xBat, yBat, 0);
  else 
    al_draw_bitmap_region(bat_right, curFrame * larguraBat-2 ,0, larguraBat, alturaBat, xBat, yBat, 0);
}

    
//verifica se há algum inimigo nas proximidades
int verifica_inimigo(int xBat, int yBat, INIMIGO inimigo[], int *n_inimigo){
    int i,j, distancia_x, distancia_y;

    for (i=0; i<28; i++){
        distancia_x = abs(inimigo[i].posicao_x - xBat); //distancia longitudinal entre um inimigo e o batman
        distancia_y = abs(inimigo[i].posicao_y - yBat); //distancia latitudinal entre um inimigo e o batman

        if(distancia_x<=35 && distancia_y<=35){
            *n_inimigo = i; //retorna para a variavel inimigo_atual
            return BATALHA; // se encontrado um inimigo
        }
    }
    return NAVEGACAO; // se nao encontrado um inimigo 

}


void desenhaSeta(int opcao, ALLEGRO_BITMAP *seta, int enter){ // indicador de opção selecionada na batalha
    if (opcao == ATACAR){
        al_draw_bitmap(seta, 450, 200,0);
    }
    else if(opcao == ESPECIAL){
        al_draw_bitmap(seta, 450, 260,0);
    }
    else{
        al_draw_bitmap(seta, 450, 320,0);
    }

}


int processaEnter(int opcao, ALLEGRO_BITMAP *batarang, int *p_batarang, ALLEGRO_BITMAP *special, int *p_special, int *p_enter, int *xBat, int *yBat, int *turno, int *n_specials, int *fuga){ // executa ação selecionada
    int sorte = 1+ rand() % 10;

    if (opcao == ATACAR){
        *fuga=2;
        if (*p_batarang>150){
            al_draw_bitmap(batarang, *p_batarang, 580,0);
            *p_batarang -= 6;
        }
        else{
            *p_enter = 0;
            *p_batarang = 580;
    
        }
        return BATALHA;
    }
    else if(opcao==ESPECIAL && *n_specials > 0){
        *fuga=2;
        if (*p_special>120){
            al_draw_bitmap(special, *p_special, 520,0);
            *p_special -= 6;
        }
        else{
            *p_enter = 0;
            *p_special = 520;
            *turno = 1;
        }
        *n_specials --;
        return BATALHA;
    }
    else{
        *fuga--;
        *p_enter = 0;
        if (sorte>=7){
            *turno = 0;
            return NAVEGACAO;
        }
        else{
            *turno = 1;
        }
    }
}


void ataque_inimigo(ALLEGRO_BITMAP *knife, int *p_knife, int *turno){ // ataca o batman e muda o turno
    if (*p_knife<600){
            al_draw_bitmap(knife, *p_knife, 580,0);
            *p_knife += 6;
        }
    else{
        *p_knife = 160;
        *turno =0;
    }
   
}


int main(int argc, char **argv){
    
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

    //variaveis para printar imagens/personagen
    ALLEGRO_BITMAP *cenario_naveg = NULL;
    ALLEGRO_BITMAP *cenario_batalha = NULL;
    ALLEGRO_BITMAP *batcave = NULL;
    ALLEGRO_BITMAP *bat_up = NULL;
    ALLEGRO_BITMAP *bat_down = NULL;
    ALLEGRO_BITMAP *bat_left = NULL;
    ALLEGRO_BITMAP *bat_right = NULL;
    ALLEGRO_BITMAP *bat_batalha = NULL;
    ALLEGRO_BITMAP *coringa = NULL;
    ALLEGRO_BITMAP *menu = NULL;
    ALLEGRO_BITMAP *seta = NULL;
    ALLEGRO_BITMAP *batarang = NULL;
    ALLEGRO_BITMAP *knife = NULL;
    ALLEGRO_BITMAP *wasted = NULL;
    ALLEGRO_BITMAP *gameover = NULL;
    ALLEGRO_BITMAP *special = NULL;
    ALLEGRO_SAMPLE *bat_theme = NULL;


	//----------------------- rotinas de inicializopcao ---------------------------------------
    
    
 
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
    
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
    
 
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

    //instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 0);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}


 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
      
    //carrega o arquivo de audio
    al_install_audio();
    al_init_acodec_addon();
    
    al_reserve_samples(2);
    bat_theme = al_load_sample("bat_theme.ogg");
    al_play_sample(bat_theme, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

    //inicia o temporizador
	al_start_timer(timer);
    
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
    //registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
    al_register_event_source(event_queue, al_get_mouse_event_source());  	

    
    // bitmaps ----------------------------------------------------------
    
    cenario_naveg = al_load_bitmap("Bitmaps\\jungle.jpg"); //cenario de NAVEGACAO
    if(!cenario_naveg) {
        fprintf(stderr, "Falha ao criar a cenario de naveg bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------objetivo 
    batcave = al_load_bitmap("Bitmaps\\batcave.png"); 
    if(!batcave) {
        fprintf(stderr, "Falha ao criar a Batcave bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    int larguraBatcave = al_get_bitmap_width(batcave);
    int alturaBatcave = al_get_bitmap_height(batcave);
    int xBatcave = 45;
    int yBatcave = 15;

//-------------------------------------------------------------sprites batman no modo de NAVEGACAO 
    bat_up = al_load_bitmap("Bitmaps\\bat_up.png");
    if(!bat_up) {
        fprintf(stderr, "Falha ao criar o bat up bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    bat_down = al_load_bitmap("Bitmaps\\bat_down.png");
    if(!bat_down) {
        fprintf(stderr, "Falha ao criar o bat down bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    bat_left = al_load_bitmap("Bitmaps\\bat_left.png");
    if(!bat_left) {
        fprintf(stderr, "Falha ao criar o bat left bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    bat_right = al_load_bitmap("Bitmaps\\bat_right.png");
    if(!bat_right) {
        fprintf(stderr, "Falha ao criar o bat right bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    bat_batalha = al_load_bitmap("Bitmaps\\bat_battle.png");
    if(!bat_batalha) {
        fprintf(stderr, "Falha ao criar o bat batalha bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    int hpBat = 100;
    int larguraBat = 48;
    int alturaBat =  48;
    int xBat = 650;
    int yBat = 620;
    int dBat = 2; // Deslocamento //

  

//-------------------------------------------------------------
    cenario_batalha = al_load_bitmap("Bitmaps\\jungle_battle.jpg"); //cenario de NAVEGACAO
    if(!cenario_batalha) {
        fprintf(stderr, "Falha ao criar a cenario de batalha bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------
    coringa = al_load_bitmap("Bitmaps\\coringa.png"); //cenario de NAVEGACAO
    if(!coringa) {
        fprintf(stderr, "Falha ao criar coringa bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------
    menu = al_load_bitmap("Bitmaps\\menu.png"); //cenario de NAVEGACAO
    if(!menu) {
        fprintf(stderr, "Falha ao criar menu bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------
    seta = al_load_bitmap("Bitmaps\\seta.png"); //cenario de NAVEGACAO
    if(!seta) {
        fprintf(stderr, "Falha ao criar seta bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------
    batarang = al_load_bitmap("Bitmaps\\batarang.png"); //cenario de NAVEGACAO
    if(!batarang) {
        fprintf(stderr, "Falha ao criar batarang bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    int x_batarang=580;

//-------------------------------------------------------------
    special = al_load_bitmap("Bitmaps\\special.png"); //cenario de NAVEGACAO
    if(!batarang) {
        fprintf(stderr, "Falha ao criar special bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    int x_special = 520;

//-------------------------------------------------------------
    knife = al_load_bitmap("Bitmaps\\knife.png"); //cenario de NAVEGACAO
    if(!knife) {
        fprintf(stderr, "Falha ao criar knife bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    int x_knife=160;

//-------------------------------------------------------------
    gameover = al_load_bitmap("Bitmaps\\gameover.png"); //cenario de NAVEGACAO
    if(!gameover) {
        fprintf(stderr, "Falha ao criar gameover bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

//-------------------------------------------------------------
    wasted = al_load_bitmap("Bitmaps\\wasted.png"); //cenario de NAVEGACAO
    if(!wasted) {
        fprintf(stderr, "Falha ao criar wasted bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    
//------------------------------------------------------------FIM DOS BITMAPS/SPRITES



//------------parâmetros iniciais do game

    //gerar aleatoriamente 28 inimigos.
    INIMIGO inimigo[28];
    int i;
    srand(time(NULL));

    for (i=0; i<28; i++){ //obs: o while serve para que nenhum inimigo spawne no mesmo lugar que o batman.
        inimigo[i].posicao_x = rand() % 800;
        inimigo[i].posicao_y = rand() % 800;
        // o while abaixo serve para nao permitir que nenhume inimigo seja posicionado "em cima" do spawn do player
        while (inimigo[i].posicao_x >=580 && inimigo[i].posicao_x <= 680 && inimigo[i].posicao_y >= 550 && inimigo[i].posicao_y <= 650){
            inimigo[i].posicao_x = rand() % 800;
            inimigo[i].posicao_y = rand() % 800;
        }
        //o level do inimigo serve para determinar quantos pontos de vida e quanto dano o inimigo tera
        inimigo[i].level = 1+ rand() % 4;
        inimigo[i].hp = 20*inimigo[i].level;
        
    }


	int playing = 1;
    int modo_jogo = NAVEGACAO; //inicia o jogo em modo de navegação
    int direcao = PRA_CIMA; //inicio o jogo com o personagem virado para o norte

    // Variaveis para animação 
    int maxframes = 3; //numero de imagens q compoem o sprite na navegação = 3; 
    int maxframes2 = 2; //numero de imagens q compoem o sprite na batalha = 2;
    int curFrame = 0; //frame atual (current_frame)
    int frameCount = 0; // contagem de frame
    int frameDelay = 10; // intervalo entre as imagens do sprite na navegação
    int frameDelay2 = 30; // intervalo entre as imagens do sprite na batalha

    //variaveis para indicar movimentacao de acordo com a tecla pressionada
    int moveRight = 0; 
    int moveLeft = 0; 
    int moveUp = 0; 
    int moveDown = 0;

    //variaveis para o menu/modo batalha
    int opcao = ATACAR; //a opcão pre selecionada é a primeira: atacar
    int enter = 0; // indica quando a tecla enter é pressionada em modo de batalha selecionando uma opçao
    int turno = 0; // turno==0 indica que é a vez do player, turno==1 indica vez do inimigo
    int inimigo_atual=0; //posicao do inimigo no array de inimigos criado anteriormente
    int n_specials = 3;
    int fuga =2;
    ALLEGRO_COLOR cor_hpBat, cor_hpInimigo; //altera a cor da barra de vida de acordo com os pontos restantes


    //variaveis para fim do jogo
    char str_pontuacao[5], str_record[5];
    int inimigos_derrotados=0, record=0, pontuacao_atual = 0; //armazenam informações ao longo do jogo e o record importado do arquivo record.txt
    FILE *file_record;

    if(!(file_record = fopen("record.txt", "r"))){ //se o arquivo record não existe, é criado e o record é estabelecido como 0
        printf("O arquivo record.txt nao foi encontrada. Record = 0;");
        file_record = fopen("record.txt", "w");
    }
    else{ 
        fscanf(file_record, "%d", &record); //importa o inteiro record no arquivo
    }
    fclose(file_record); //encerra o arquivo aberto
    file_record = NULL;


    
    
	while(playing) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

        if (fuga == 0){
            hpBat = -1;
        }


        if(!(xBat <=100 && yBat<=50)) //caso o batman nao se encontre dentro dos limites do objetivo
            modo_jogo = verifica_inimigo(xBat,yBat, inimigo, &inimigo_atual); //procura por inimigos na proximidade, se encontrar entra em modo de batalha
        
        if (inimigo[inimigo_atual].hp <= 0){ // se o inimigo em batalha for derrotado
                inimigo[inimigo_atual].posicao_x = 9999; // envia ele pro espaço rsrs
                pontuacao_atual += (inimigo[inimigo_atual].level * 10); // pontos proporcionais ao level do inimigo derrotado sao obtidos
                inimigo[inimigo_atual].hp = 1; // muda a vida do inimigo para 1 apenas para que a condição nao torne-se um loop
        }
           

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
            if (hpBat >= 0){ //se o batman esta vivo

                if (modo_jogo == NAVEGACAO){ // se em navegação, desenha os objetos do jogo na nevegação
                    turno =0;
                    dBat = 2;
                    al_draw_bitmap(cenario_naveg, 0 , 0, 0);            
                    al_draw_bitmap(batcave, xBatcave , yBatcave, 0);
                    desenhaBatNaveg(direcao, curFrame, larguraBat, alturaBat, xBat, yBat, bat_up, bat_down, bat_left, bat_right);

                    sprintf(str_pontuacao, "%d", pontuacao_atual);
                    al_draw_text(size_32, al_map_rgb(0,0,255), 750, 20, ALLEGRO_ALIGN_CENTER, str_pontuacao);

                    if(xBat <=100 && yBat<=50){ // se o batman houver chegado no objetivo
                        dBat=0; // conjela o batman
                        al_draw_bitmap(gameover, 253,312,0);

                        if (pontuacao_atual > record){ // verifica se a pontuacao atual e maior que o record, se sim o record e atualizado
                            record = pontuacao_atual;
                            file_record = fopen("record.txt", "w");
                            fprintf(file_record, "%d", pontuacao_atual);
                        } 

                        al_draw_bitmap(gameover, 253,312,0);
                        
                        sprintf(str_record, "%d", record);

                        al_draw_text(size_32, al_map_rgb(0,0,255), 448, 390, ALLEGRO_ALIGN_CENTER, str_pontuacao);
                        al_draw_text(size_32, al_map_rgb(0,0,255), 448, 435, ALLEGRO_ALIGN_CENTER, str_record);
                        }

                }

                else{ // se em batalha...
                    dBat=0;
                    al_draw_bitmap(cenario_batalha, 0 , 0, 0); 

                    // variaveis utilizadas para alternar entre as imagens do sprite no modo de batalha
                    frameCount++;
                    if (frameCount >= frameDelay2){
                        curFrame = !(curFrame);
                        frameCount = 0;
                    }

                    al_draw_bitmap_region(bat_batalha, 0, curFrame*233, 186, 233, 550, 500, 0);
                    al_draw_bitmap_region(coringa, curFrame*116, 0, 115, 200, 115, 520, 0);
                    al_draw_bitmap(menu, 530,200,0);


                    // definições e desenho das barras de vida
                    if (hpBat <=40){
                        cor_hpBat = al_map_rgb(255,0,0);
                    }
                    else{
                        cor_hpBat = al_map_rgb(0,255,0);
                    }
                    al_draw_filled_rectangle(535,450, 535+hpBat*2, 485, cor_hpBat);


                    if (inimigo[inimigo_atual].hp<=40){
                        cor_hpInimigo = al_map_rgb(255,0,0);
                    }
                    else{
                        cor_hpInimigo = al_map_rgb(0,255,0);
                    }
                    al_draw_filled_rectangle(100,450, 100+inimigo[inimigo_atual].hp*2, 485, cor_hpInimigo);
                    

                    if (turno ==0){ // caso seja a vez do player
                        desenhaSeta(opcao, seta, enter);

                        if(enter){ //se a tecla enter foi pressionada
                            modo_jogo = processaEnter(opcao, batarang, &x_batarang, special, &x_special, &enter, &xBat, &yBat, &turno, &n_specials, &fuga); // executa a acao selecionada e muda o turno
                                if (opcao == FUGIR){
                                    inimigo[inimigo_atual].posicao_x = rand() % 800;
                                    inimigo[inimigo_atual].posicao_y = rand() % 800;
                                    fuga--;
                                    printf("\nfuga: %d\n", fuga);
                                }
                                else if (x_batarang == 154){ //no fim do trajeto do batarang, a vida do inimigo é reduzida.
                                    inimigo[inimigo_atual].hp -= 15;
                                    turno=1;
                                }
                                else if (x_special == 124){
                                    inimigo[inimigo_atual].hp -= 60;
                                }
                            
                        }
                
                    }


                    else{ // se for a vez do inimigo
                        ataque_inimigo(knife, &x_knife, &turno); // inimigo ataca
                        if (x_knife == 598){ // no fim do trajeto da faca, a vida do batman e reduzida
                            hpBat -= 10 + (rand() % (inimigo[inimigo_atual].level*5));
                        }
                    } 
                }

                
            }

            else if (hpBat <=0){ // se o batman estiver morto
                al_draw_bitmap(wasted, 150,325,0);
            }
            
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
		}


		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		

		//se uma tecla e pressionada
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            if (modo_jogo == NAVEGACAO){ // no modo de navegação as setas definem a direcao do movimento do batman

                switch(ev.keyboard.keycode){
                    case ALLEGRO_KEY_LEFT:
                        moveLeft = 1; 
                        break;

                    case ALLEGRO_KEY_RIGHT:
                        moveRight = 1; 
                        break;

                    case ALLEGRO_KEY_UP:
                        moveUp = 1;
                        break;

                    case ALLEGRO_KEY_DOWN:
                        moveDown = 1;
                        break;
                }
            }  

            else{ // no modo de batalha, as setas para cima e para baixo e enter alternam e seleciona as opções
                switch(ev.keyboard.keycode){
                    case ALLEGRO_KEY_UP:
                        if (opcao >0){
                            opcao --; 
                        }
                        break;

                    case ALLEGRO_KEY_DOWN:
                        if (opcao <2){
                            opcao++;
                        } 
                        break;
                    
                    case ALLEGRO_KEY_ENTER:
                        enter = 1;
                }
            }

			//imprime qual tecla foi pressionada
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
		}

        else if(ev.type == ALLEGRO_EVENT_KEY_UP) { // Quando a tecla é despressionada

                switch(ev.keyboard.keycode){
                    case ALLEGRO_KEY_LEFT:
                        moveLeft = 0; 
                        break;

                    case ALLEGRO_KEY_RIGHT:
                        moveRight = 0; 
                        break;

                    case ALLEGRO_KEY_UP:
                        moveUp = 0;
                        break;

                    case ALLEGRO_KEY_DOWN:
                        moveDown = 0;
                        break;
                }
        }


        //operacoes para alternar entre as imagens do personagem gerando animação 
        if(moveUp || moveDown || moveRight || moveLeft){ // se alguma seta foi pressionada em navegação
            frameCount++;
            if(frameCount >= frameDelay){
                curFrame++;
              if(curFrame >= maxframes)
                curFrame = 0;
    
              frameCount = 0;
            }
          }
          

        else if (modo_jogo == NAVEGACAO){
            curFrame = 1;
        }


     //depois que uma tecla e pressionada, enquanto ela não for despressionada o batman e deslocado em direção a seta
        if(moveUp){
            if (yBat - dBat >= 0){
                yBat -= dBat;
                direcao = PRA_CIMA;
            }
        }
        else if(moveDown){
            if (yBat + dBat < SCREEN_H-alturaBat){
                yBat += dBat;
                direcao = PRA_BAIXO;
            }
        }
        else if(moveLeft){
            if (xBat-dBat >= -5){
                xBat -= dBat;
                direcao = PRA_ESQUERDA;
            }
        }
        else if(moveRight){
            if (xBat + dBat <= SCREEN_W-larguraBat){
                xBat += dBat;
                direcao = PRA_DIREITA;
            }
        
        }           
        
        
	} //fim do while
    

	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
    al_destroy_bitmap(batcave);
    al_destroy_bitmap(bat_up);
    al_destroy_bitmap(bat_down);
    al_destroy_bitmap(bat_left);
    al_destroy_bitmap(bat_right);
    al_destroy_bitmap(cenario_naveg);
    al_destroy_bitmap(cenario_batalha);
    al_destroy_bitmap(bat_batalha);
    al_destroy_bitmap(coringa);
    al_destroy_bitmap(menu);
    al_destroy_bitmap(batarang);
    al_destroy_bitmap(seta);
    al_destroy_bitmap(knife);
    al_destroy_bitmap(wasted);
    al_destroy_bitmap(gameover);
    al_destroy_font(size_32);
    //al_destroy_sample(bat_theme);

    fclose (file_record);
	return 0;
}