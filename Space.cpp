//******************************************************************************
//Gabriel Rodrigues do Prado Rossales 6608843
//
//
//******************************************************************************
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <time.h>
#include <string>

#define DEF_TAM 20
#define PI 3.14159



// Declaração de variáveis globais
GLfloat missel1_y = 0, missel2_y = 0;
GLfloat aviao_x = 0, missel1_tx = 0, missel2_tx = 0;

//defesa
struct def{
    GLfloat g,x,y;
};
//inimigo
struct enemy{
    GLfloat x,y;
    int type;
};


bool inicializado = false, paused;//inicializado flag pra iniciar as variaveis, pause marca end game
int score=0,lives=3; 

std::vector<struct def> defesa; //contem as defesas
std::vector<struct enemy> invasores;//contem os invasores
std::vector<struct enemy> tiros;
bool missel1_moving = false, missel2_moving = false;//comeca com os misseis parados

int msec_missel1 = 0, msec_missel2 = 0;
//inicializa as posicoes das defesas e inimigos
void inicializa(){
    struct def temp;
    struct enemy temp1;
    srand(time(NULL));
    for(int i=0, k=-13.5;i<DEF_TAM/5;i++,k+=8){
            temp.g=1.0f;
            temp.x=k;
            temp.y=2;
            defesa.push_back(temp);
            temp.y=3;
            defesa.push_back(temp);
            temp.x=k+1;
            defesa.push_back(temp);
            temp.x=k+2;
            defesa.push_back(temp);
            temp.y=2;
            defesa.push_back(temp);

    }
    for(int i=0;i<5;i++){
        for(float j=0, k=-13;j<10;j++,k+=2.5){
            temp1.type = i+1;
            temp1.x=k;
            temp1.y=25.5-i*2.5;
            invasores.push_back(temp1);
        }
    }

}
//move os inimigos 
void move_invader(int passo){
    if(paused)return;
    bool overflow = false; //flag pra caso encostar nos cantos
    for(int i=0;i<invasores.size();i++){
        if(invasores[i].x + (1.0*passo)/100 >18){
            overflow = true;
            break;
        }
        if(invasores[i].x + (1.0*passo)/100 < -20){
            overflow = true;
            break;
        }
    }
    if(!overflow){//se nao encostou faz normal
        for(int i=0;i<invasores.size();i++){

            invasores[i].x += (1.0*passo)/100;
        }
        glutPostRedisplay();
        glutTimerFunc(750+2*(passo>0?-passo:passo), move_invader, passo);
    }
    else{//se encostou desce e muda o sentido do movimento na proxima chamada
        for(int i=0;i<invasores.size();i++){
            invasores[i].y --;
        }
        glutPostRedisplay();
        if(passo >0){ 
            glutTimerFunc(750-2*passo, move_invader, -(passo+5));
        }else{
            glutTimerFunc(750+2*passo, move_invader, -(passo-5));
        }
    }
}
//usado para escrever texto, argumentos sao a string, tamanho string, posicao x, posicao y, 
//tamanho da fonte(usado para dividir o tam original) e cor em rgb
void drawText(const char *text, int length, int x, int y,float size,float r,float g, float b){
    glMatrixMode(GL_PROJECTION);
    double *matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0,800,0,600,-5,5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glColor3f(r,g,b);
    glTranslatef(x,y,0.0f);
    glScalef(1.0f*size,1.0f*size,1.0f);
    glRasterPos2i(x,y);
    for(int i=0;i<length;i++){
        glutStrokeCharacter(GLUT_STROKE_ROMAN,(int)text[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

//faz cada tiro do inimigo se mover
void move_tiro(int passo){
    for(int i=0;i<tiros.size();i++){
        tiros[i].y -= (1.0*passo)/100;
    }

    glutPostRedisplay();
    glutTimerFunc(10, move_tiro, passo);
}
//move missel 1
void move_missel1(int passo){
    if(!missel1_moving)return;
    missel1_y += (1.0*passo)/100;
    glutPostRedisplay();

    glutTimerFunc(10, move_missel1, passo);
}
//move missel 2
void move_missel2(int passo){
    if(!missel2_moving)return;

    missel2_y += (1.1*passo)/100;
    glutPostRedisplay();

    glutTimerFunc(10, move_missel2, passo);
}

// Função para desenhar a base do objeto           
void DesenhaAviao(){

    glColor3f(0.5f,0.6f,0.86f);
    glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0f,-1.0f);//asa
    glVertex2f(1.0f,-1.0f);
    glVertex2f(0.0f,0.0f);		
    glVertex2f(0.0f,0.5f);//asa frente pequena
    glVertex2f(0.5f,-0.1f);
    glVertex2f(-0.5f,-0.1f);		
    glVertex2f(0.0f,0.8f);//ponta caca
    glVertex2f(0.2f,0.5f);
    glVertex2f(-0.2f,0.5f);		
    glVertex2f(0.0f,-1.4f);//rabeta caca
    glVertex2f(0.2f,-1.0f);
    glVertex2f(-0.2f,-1.0f);		
    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(0.1f,-1.5f);
    glVertex2f(-0.1f,-1.5f);		
    glColor3f(1.0f,1.0f,0.0f);
    glVertex2f(0.0f,-1.9f);//rabeta caca
    glEnd();

    glColor3f(0.5f,0.6f,0.86f);
    glBegin(GL_QUADS);
    glVertex2f(0.2f,-0.2f);//meio
    glVertex2f(0.2f,0.5f);
    glVertex2f(-0.2f,0.5f);
    glVertex2f(-0.2f,-0.2f);
    glVertex2f(0.1f,-1.4f);//saida
    glVertex2f(0.1f,-1.3f);
    glVertex2f(-0.1f,-1.4f);
    glVertex2f(-0.1f,-1.3f);

    glEnd();
}
//desenha inimigo mais acima
void DesenhaCapacete(struct enemy inv){
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(inv.x,inv.y+1.2);//lado esquerdo cabeca
    glVertex2f(inv.x,inv.y+2);
    glVertex2f(inv.x+1,inv.y+2);
    glVertex2f(inv.x+2,inv.y+1.2);//lado direito cabeca
    glVertex2f(inv.x+2,inv.y+2);
    glVertex2f(inv.x+1,inv.y+2);
    glVertex2f(inv.x,inv.y+1);//mandibula esquerda
    glVertex2f(inv.x,inv.y);
    glVertex2f(inv.x+0.8,inv.y+1);
    glVertex2f(inv.x+2,inv.y+1);//mandibula direita
    glVertex2f(inv.x+2,inv.y);
    glVertex2f(inv.x+1.2,inv.y+1);
    glVertex2f(inv.x,inv.y);//buraco boca
    glVertex2f(inv.x+2,inv.y);
    glVertex2f(inv.x+1,inv.y+0.9);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex2f(inv.x+0.5,inv.y+0.5);// boca
    glVertex2f(inv.x+1.5,inv.y+0.5);
    glVertex2f(inv.x+1,inv.y+0.1);
    glEnd();
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);//olhos
    glVertex2f(inv.x+0.5,inv.y+1.3);//esquerdo
    glVertex2f(inv.x+0.5,inv.y+1.5);
    glVertex2f(inv.x+0.7,inv.y+1.5);
    glVertex2f(inv.x+0.7,inv.y+1.3);
    glVertex2f(inv.x+1.3,inv.y+1.3);//direito
    glVertex2f(inv.x+1.3,inv.y+1.5);
    glVertex2f(inv.x+1.5,inv.y+1.5);
    glVertex2f(inv.x+1.5,inv.y+1.3);
    glEnd();
}
//desenha inimigo do meio
void DesenhaAndroid(struct enemy inv){
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(inv.x-0.2,inv.y+2);//buraco testa
    glVertex2f(inv.x+2.2,inv.y+2);
    glVertex2f(inv.x+1,inv.y+1.4);
    glVertex2f(inv.x,inv.y+0.9);//lado esquerdo cabeca
    glVertex2f(inv.x,inv.y+1.7);
    glVertex2f(inv.x+0.6,inv.y+1.4);
    glVertex2f(inv.x+2,inv.y+0.9);//lado direito cabeca
    glVertex2f(inv.x+2,inv.y+1.7);
    glVertex2f(inv.x+1.4,inv.y+1.4);
    glVertex2f(inv.x+0.2,inv.y);//mandibula esquerda
    glVertex2f(inv.x+0.2,inv.y+0.7);
    glVertex2f(inv.x+0.6,inv.y);
    glVertex2f(inv.x+1.8,inv.y);//mandibula direita
    glVertex2f(inv.x+1.8,inv.y+0.7);
    glVertex2f(inv.x+1.4,inv.y);
    glVertex2f(inv.x+0.5,inv.y+0.5);//buraco boca
    glVertex2f(inv.x+1.4,inv.y+0.5);
    glVertex2f(inv.x+1,inv.y-0.3);
    glEnd();
    glBegin(GL_QUADS);//olhos
    glVertex2f(inv.x+0.5,inv.y+0.7);//esquerdo
    glVertex2f(inv.x+0.5,inv.y+0.9);
    glVertex2f(inv.x+0.7,inv.y+0.9);
    glVertex2f(inv.x+0.7,inv.y+0.7);
    glVertex2f(inv.x+1.3,inv.y+0.7);//direito
    glVertex2f(inv.x+1.3,inv.y+0.9);
    glVertex2f(inv.x+1.5,inv.y+0.9);
    glVertex2f(inv.x+1.5,inv.y+0.7);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex2f(inv.x+0.9,inv.y+0.3);//bigode
    glVertex2f(inv.x+0.9,inv.y+0.5);
    glVertex2f(inv.x+1.1,inv.y+0.5);
    glVertex2f(inv.x+1.1,inv.y+0.3);
    glEnd();
}

//desenha o inimigo mais abaixo
void DesenhaCaveira(struct enemy inv){
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(inv.x,inv.y);//boca
    glVertex2f(inv.x+2,inv.y);
    glVertex2f(inv.x+1,inv.y+0.9);
    glVertex2f(inv.x,inv.y+1.5);//lado esquerdo cabeca
    glVertex2f(inv.x,inv.y+2);
    glVertex2f(inv.x+0.6,inv.y+2);
    glVertex2f(inv.x+2,inv.y+1.5);//lado direito cabeca
    glVertex2f(inv.x+2,inv.y+2);
    glVertex2f(inv.x+1.4,inv.y+2);
    glVertex2f(inv.x,inv.y+1);//mandibula esquerda
    glVertex2f(inv.x,inv.y+0.2);
    glVertex2f(inv.x+0.8,inv.y+1);
    glVertex2f(inv.x+2,inv.y+1);//mandibula direita
    glVertex2f(inv.x+2,inv.y+0.2);
    glVertex2f(inv.x+1.2,inv.y+1);
    glEnd();
    glBegin(GL_QUADS);//olhos
    glVertex2f(inv.x+0.5,inv.y+1.3);//esquerdo
    glVertex2f(inv.x+0.5,inv.y+1.5);
    glVertex2f(inv.x+0.8,inv.y+1.5);
    glVertex2f(inv.x+0.8,inv.y+1.3);
    glVertex2f(inv.x+1.2,inv.y+1.3);//direito
    glVertex2f(inv.x+1.2,inv.y+1.5);
    glVertex2f(inv.x+1.5,inv.y+1.5);
    glVertex2f(inv.x+1.5,inv.y+1.3);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex2f(inv.x+0.8,inv.y+0.4);//bigode
    glVertex2f(inv.x+0.8,inv.y+0.6);
    glVertex2f(inv.x+1.2,inv.y+0.6);
    glVertex2f(inv.x+1.2,inv.y+0.4);
    glEnd();
}

//desenha o inimigo recebido, chamando a funcao correta para cada tipo
void DesenhaInvader(struct enemy inv){
    if(inv.type >=4){
        glColor3f(0.6f,0.6f,0.6f);
        glBegin(GL_QUADS);
        glVertex2f(inv.x,inv.y);
        glVertex2f(inv.x,inv.y+2);
        glVertex2f(inv.x+2,inv.y+2);
        glVertex2f(inv.x+2,inv.y);
        glEnd();
        DesenhaCaveira( inv);
    }else if(inv.type>=2){
        glColor3f(0.0f,0.6f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(inv.x,inv.y);
        glVertex2f(inv.x,inv.y+2);
        glVertex2f(inv.x+2,inv.y+2);
        glVertex2f(inv.x+2,inv.y);
        glEnd();
        DesenhaAndroid( inv);
    }else{
        glColor3f(0.6f,0.1f,0.1f);
        glBegin(GL_QUADS);
        glVertex2f(inv.x,inv.y);
        glVertex2f(inv.x,inv.y+2);
        glVertex2f(inv.x+2,inv.y+2);
        glVertex2f(inv.x+2,inv.y);
        glEnd();
        DesenhaCapacete(inv);
    }
    
}
//desenha a defesa
void DesenhaDef(struct def defesa){
    glColor3f(0.0f,defesa.g,0.0f);
    glBegin(GL_QUADS);
    glVertex2f(defesa.x,defesa.y);
    glVertex2f(defesa.x,defesa.y+1);
    glVertex2f(defesa.x+1,defesa.y+1);
    glVertex2f(defesa.x+1,defesa.y);
    glEnd();

}
//faz a explosao
void desenha_explo(GLfloat x, GLfloat y){
    int tri_number = 20;
    GLfloat doispi = 2.0f*PI;
    glLineWidth(1);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(x,y);
    glColor3f(1.0f,1.0f,0.0f);
    for(int i=0;i<=tri_number;i++){
        glVertex2f(x+(cos(i*doispi/tri_number)),y+sin(i*doispi/tri_number));
    }
    glEnd();
}
//desenha o tiro do inimigo
void desenha_tiro(GLfloat x, GLfloat y){
    glColor3f(1.0f,0.0f,0.1f);
    glLineWidth(2);
    glBegin(GL_QUADS);
    glVertex2f(x-0.1f,y);
    glVertex2f(x+0.1f,y);
    glVertex2f(x+0.1f,y+0.4f);
    glVertex2f(x-0.1f,y+0.4f);
    glEnd();
}
//desenha os misseis do player
void DesenhaMisseis(){

    glColor3f(1.0f,1.0f,1.0f);
    glLineWidth(2);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f,-1.0f);
    glVertex2f(-1.0f,-0.7f);
    glVertex2f(-0.9f,-0.6f);
    glVertex2f(-0.8f,-0.7f);
    glVertex2f(-0.8f,-1.0f);
    glEnd();
}
//reseta o missel m para poder atirar novamente
void resetMissel(int m){
    if(m == 1){    
        missel1_y =0;
        missel1_moving = false;
        missel1_tx=0;
    }else{
        missel2_y =0;
        missel2_moving = false;
        missel2_tx=0;
    }

}
//dado um inimigo(alien) faz ele atirar
void atira(int alien){
    tiros.push_back(invasores[alien]);
}
//escolhe um inimigo randomicamente a cada 4 sec
//vai chamando mais rapido com o tempo ate chegar em 1 por segundo
void escolhe_Alien(int passo){
    if(invasores.size() == 0)return;
    int alien = rand()%invasores.size();
    atira(alien);

    glutTimerFunc((4000-passo>1000?4000-passo:1000),escolhe_Alien,passo+30);
}

// Função callback de redesenho da janela de visualização
void Desenha(void)
{	
    // Muda para o sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa a matriz de transformação corrente
    glLoadIdentity();

    // Limpa a janela de visualização com a cor  
    // de fundo definida previamente
    glClear(GL_COLOR_BUFFER_BIT);
    
    //escreve na tela a pontuacao
    std::string pts = ("score "); 
    pts+= std::to_string(score);
    drawText(pts.c_str(),pts.length(),0,570,0.2,1,1,1);
    //escreve na tela quanta vidas
    pts = ("lives "); 
    pts+= std::to_string(lives);
    drawText(pts.c_str(),pts.length(),0,550,0.2,1,1,1);

    //manda desenhar as defesas
    for(int i=0;i<defesa.size();i++){
        DesenhaDef(defesa[i]);
    }
    //manda desenhar os invasores
    for(int i=0;i<invasores.size();i++){
        DesenhaInvader(invasores[i]);
        if(invasores[i].y<2 )   //se invasores muito perto(passou defesas) game over
            paused=true;
    }
    //ve se os tiros dados pelos invasores acertaram algo
    for(int i=0;i<tiros.size();i++){
        if(tiros[i].y<=4){//distancia em que estao as defesas, portanto checa elas
            for(int j=0;j<defesa.size();j++){
                if((tiros[i].x-defesa[j].x >= 0 && tiros[i].x - defesa[j].x <=1 ) && tiros[i].y -defesa[j].y <=1){
                    tiros.erase(tiros.begin()+i);
                    defesa[j].g -=0.25f;
                    if(defesa[i].g <=0){
                        defesa.erase(defesa.begin()+i);
                    }
                    break;
                }
            }
        }if(tiros[i].y <=0 && tiros[i].y>-2 && (tiros[i].x-aviao_x >-1 && tiros[i].x-aviao_x<1)){//distancia em que esta o player, checa ele
            lives--;
            desenha_explo(tiros[i].x,tiros[i].y);
            paused = (lives > 0) ? false: true;
            tiros.erase(tiros.begin()+i);
        }
        if(tiros[i].y<=-2){
            tiros.erase(tiros.begin()+i);
        }
        desenha_tiro(tiros[i].x,tiros[i].y);
    }
    
    //se "pausado" escreve game over na tela
    //se nao ha mais inimigos escreve you win
    if(paused)
        drawText("Game Over",9,200,250,0.5,1,0,0);
    if(invasores.size()==0){
        drawText("You Win",7,230,250,0.5,1,1,1);
    }

    //printf("def %f",defesa.x);
    glTranslatef(aviao_x,0.0f,0.0f);
    glTranslatef(0.0f,-0.7f,0.0f);	
    //glScalef(0.25f,0.25f,0.0f);
    glPushMatrix();
    //ve se o tiro saiu do cenario
    if(missel1_y > 30){
        resetMissel(1);
    }
    if(missel2_y > 30){
        resetMissel(2);
    }
    //ve se o tiro acertou a propria defesa
    for(int i=0;i<defesa.size();i++){ 
        if((missel1_tx - defesa[i].x >= 1 && missel1_tx - defesa[i].x <=2) &&  (missel1_y > defesa[i].y +1.5)){
            defesa[i].g -=0.25f;
            resetMissel(1);
            //printf("HIT");
            glutPostRedisplay();
            if(defesa[i].g <=0){
                defesa.erase(defesa.begin()+i);
            }
        }else if((missel2_tx - defesa[i].x >= -1 && missel2_tx - defesa[i].x <=0) &&  (missel2_y > defesa[i].y +1.5)){
            defesa[i].g -=0.25f;
            resetMissel(2);
            //printf("HIT");
            glutPostRedisplay();
            if(defesa[i].g <=0){
                defesa.erase(defesa.begin()+i);
            }
        }
    }
    //ve se o tiro acertou um invasor
    for(int i=0;i<invasores.size();i++){ 
        if((missel1_tx - invasores[i].x >= 1 && missel1_tx - invasores[i].x <=3) &&  (missel1_y > invasores[i].y +1.5)){
            resetMissel(1);
            glutPostRedisplay();
            score +=  invasores[i].type > 3 ? 10 : (invasores[i].type > 1? 20 : 40);
            invasores.erase(invasores.begin()+i);
        }else if((missel2_tx - invasores[i].x >= -1 && missel2_tx - invasores[i].x <=1) &&  (missel2_y > invasores[i].y +1.5)){
            resetMissel(2);
            glutPostRedisplay();
            score +=  invasores[i].type > 3 ? 10 : (invasores[i].type > 1? 20 : 40);
            invasores.erase(invasores.begin()+i);
        }
    }

    if(missel2_moving){	
        glTranslatef(-aviao_x,0.0f,0.0f);	
        glTranslatef(missel2_tx,0.0f,0.0f);
    }
    //Míssel 2;
    glTranslatef(1.8f,0.0f,0.0f);
    glTranslatef(0.0f,missel2_y,0.0f);	
    DesenhaMisseis();

    glPopMatrix(); // Carrega a identidade = Limpa a matrix de transformações.
    
    glPushMatrix();

    if(missel1_moving){
        glTranslatef(-aviao_x,0.0f,0.0f);	
        glTranslatef(missel1_tx,0.0f,0.0f);
    }
   // printf("missel1 %f\n",missel1_tx);
    //Míssel 1.
    glTranslatef(0.0f,missel1_y,0.0f);
    DesenhaMisseis();

    glPopMatrix(); //Pro jatinho nao sair junto com o missel 1.
    // Desenha o jatinho.
    DesenhaAviao();

    // Executa os comandos OpenGL 
    glFlush();
}

// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    GLsizei largura, altura;

    // Evita a divisao por zero
    if(h == 0) h = 1;

    // Atualiza as variáveis
    largura = w;
    altura = h;

    // Especifica as dimensões da Viewport
    glViewport(0, 0, largura, altura);

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Estabelece a janela de seleção (esquerda, direita, inferior, 
    // superior) mantendo a proporção com a janela de visualização
    if (largura <= altura) 
    { 
        gluOrtho2D (-1.0f, 1.0f, -1.0f*altura/largura, 1.0f*altura/largura);
        //win = 1.0f;
    }              
    else 
    { 
        gluOrtho2D (-1.0f*largura/altura, 1.0f*largura/altura, -1.0f, 1.0f);
        //	win = 1.0f*largura/altura;           
    }             
}

// Função callback chamada para gerenciar eventos de teclas especiais(F1,PgDn,...)
void TeclasEspeciais(int key, int x, int y)
{
   // printf("%f\n",aviao_x);
    // Move a base
    if(key == GLUT_KEY_LEFT && !paused)
    {
        aviao_x-=0.5f;
        if ( aviao_x < -19 )
            aviao_x = -19; 
    }
    if(key == GLUT_KEY_RIGHT && !paused)
    {
        aviao_x+=0.5f;
        if ( aviao_x > 19 )
            aviao_x = 19; 
    }
    if(key == GLUT_KEY_UP   && missel1_moving!=true){
        missel1_moving = true;
        missel1_tx = aviao_x;
        glutTimerFunc(10, move_missel1, 20);
    }     
    if(key == GLUT_KEY_DOWN     && missel2_moving !=true){
        missel2_moving = true;
        missel2_tx = aviao_x;
        glutTimerFunc(10, move_missel2, 20);
    }


    glutPostRedisplay();
}

// Função callback chamada para gerenciar eventos de teclas
void Teclado(unsigned char key, int x, int y)
{
    switch (key){
        case 27:
            exit(0);
        case 'p':       //
            paused = !paused;
            break;
    }
}

// Função responsável por inicializar parâmetros e variáveis
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como branca
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D (-20, 20, -3, 31);
    glViewport(0, 0, 800, 600);   
}



// Programa Principal 
int main(int argc, char* argv[])
{
    glutInit(&argc, argv); // Inicia uma instância da glut
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
    glutInitWindowPosition(5,5);     
    glutInitWindowSize(800,600);  
    glutCreateWindow("Space Invaders"); 

    if(!inicializado){
        inicializa();
        inicializado = true;
    }

    glutTimerFunc(0, move_invader, 30);//faz a chamada pros inimigos se moverem 
    glutTimerFunc(0, escolhe_Alien, 100);//faz a chamada pra escolher qual atirara
    move_tiro(10);//faz o tiro se mover

    // Registra a função callback de redesenho da janela de visualização
    glutDisplayFunc(Desenha);  
    glMatrixMode(GL_PROJECTION);
    // Registra a função callback de redimensionamento da janela de visualização
   // glutReshapeFunc(AlteraTamanhoJanela);    
    // Registra a função callback para tratamento das teclas especiais
    glutSpecialFunc(TeclasEspeciais);

    // Registra a função callback para tratamento das teclas ASCII
    glutKeyboardFunc (Teclado);

    // Chama a função responsável por fazer as inicializações 
    Inicializa(); 

    glutTimerFunc(0, move_missel1, 0); // Timer para mover o missel 1
    glutTimerFunc(0, move_missel2, 0); // ..........................2
    // Inicia o processamento e aguarda interações do usuário
    glutMainLoop();

    return 0;
}
