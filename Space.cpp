#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include <time.h>
#include <string>

#define DEF_TAM 20
#define PI 3.14159

// Declara��o de vari�veis globais
GLfloat aviao_x = 0;

typedef struct Point
{
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
} Point;

//defesa
struct def
{
    GLfloat g, x, y;
    def() {}
    def(GLfloat _x, GLfloat _y, GLfloat _g) : x(_x), y(_y), g(_g) {}
};
//inimigo
struct enemy
{
    GLfloat x, y;
    int type;
    enemy() {}
    enemy(GLfloat _x, GLfloat _y, int _type) : x(_x), y(_y), type(_type) {}
};

struct missel
{
    float x, y;
    bool moving;
};

bool inicializado = false, gameOver; //inicializado flag pra iniciar as variaveis, pause marca end game
bool flash = false;
bool forward = true;
int score = 0, lives = 3;

std::vector<struct def> defesa;      //contem as defesas
std::vector<struct enemy> invasores; //contem os invasores
std::vector<struct missel> misseis;
std::vector<struct enemy> tiros;
std::vector<Point> explosoes;
//inicializa as posicoes das defesas e inimigos
void inicializa()
{
    struct def temp;
    //struct enemy temp1;
    struct missel temp2;
    srand(time(NULL));
    for (int i = 0, k = -13.5; i < DEF_TAM / 5; i++, k += 8)
    {
        temp = def(k, 2, 1.0f);
        // temp.g = 1.0f;
        // temp.x = k;
        // temp.y = 2;
        defesa.push_back(temp);
        temp.y = 3;
        defesa.push_back(temp);
        temp.x = k + 1;
        defesa.push_back(temp);
        temp.x = k + 2;
        defesa.push_back(temp);
        temp.y = 2;
        defesa.push_back(temp);
    }
    for (int i = 0; i < 5; i++)
    {
        for (float j = 0, k = -13; j < 10; j++, k += 3)
        {
            // temp1.type = i + 1;
            // temp1.x = k;
            // temp1.y = 25.5 - i * 2.5;
            invasores.push_back(enemy(k, 25.5 - i * 2.5, i + 1));
        }
    }
    for (int i = 0; i < 2; i++)
    {

        temp2.moving = false;
        temp2.y = -1;
        temp2.x = aviao_x - 1 + 2 * i;
        misseis.push_back(temp2);
    }
}
//move os inimigos
void move_invader(bool *forward)
{
    bool overflow = false; //flag pra caso encostar nos cantos
    float step = 0.01;

    for (int i = 0; i < invasores.size(); i++)
    {
        if (invasores[i].y < 2)
        {
            //se invasores muito perto(passou defesas) game over
            gameOver = true;
            return;
        }

        if (invasores[i].x + step > 18 && *forward == true)
        {
            overflow = true;
            break;
        }
        if (invasores[i].x - step < -20 && *forward == false)
        {
            overflow = true;
            break;
        }
    }
    if (overflow)
    { //se nao encostou faz normal
        *forward = !*forward;
        for (int i = 0; i < invasores.size(); i++)
        {
            invasores[i].y -= 0.5;
            // invasores[i].x += step * (passo > 0) ? -1 : 1;
        }
    }
    step = (*forward == true) ? step : -step;

    for (int i = 0; i < invasores.size(); i++)
    {
        invasores[i].x += step;
    }
}
//usado para escrever texto, argumentos sao a string, tamanho string, posicao x, posicao y,
//tamanho da fonte(usado para dividir o tam original) e cor em rgb
void drawText(const char *text, int length, float x, float y, float size, float r, float g, float b)
{
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, 0.0f);
    glScalef(size, size, 1.0f);
    //glRasterPos2i(x, y);
    for (int i = 0; i < length; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)text[i]);
    }
    glPopMatrix();
}

//faz cada tiro do inimigo se mover
void move_tiro()
{
    for (int i = 0; i < tiros.size(); i++)
    {
        tiros[i].y -= (1.0) / 10;
    }
}
//move missel 1
void move_misseis()
{
    for (int i = 0; i < misseis.size(); i++)
    {
        if (misseis[i].moving == true)
        {
            misseis[i].y += (1.5) / 10;
        }
    }
}

// Fun��o para desenhar a base do objeto
void DesenhaAviao()
{
    glPushMatrix();
    glTranslatef(aviao_x, 0, 0);
    glColor3f(0.5f, 0.6f, 0.86f);
    glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0f, -1.0f); //asa
    glVertex2f(1.0f, -1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f); //asa frente pequena
    glVertex2f(0.5f, -0.1f);
    glVertex2f(-0.5f, -0.1f);
    glVertex2f(0.0f, 1.0f); //ponta caca
    glVertex2f(0.2f, 0.5f);
    glVertex2f(-0.2f, 0.5f);
    glVertex2f(0.0f, -1.4f); //rabeta caca
    glVertex2f(0.2f, -1.0f);
    glVertex2f(-0.2f, -1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.1f, -1.5f);
    glVertex2f(-0.1f, -1.5f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2f(0.0f, -1.9f); //rabeta caca
    glEnd();

    glColor3f(0.5f, 0.6f, 0.86f);
    glBegin(GL_QUADS);
    glVertex2f(0.2f, -0.2f); //meio
    glVertex2f(0.2f, 0.5f);
    glVertex2f(-0.2f, 0.5f);
    glVertex2f(-0.2f, -0.2f);
    glVertex2f(0.1f, -1.4f); //saida
    glVertex2f(0.1f, -1.3f);
    glVertex2f(-0.1f, -1.4f);
    glVertex2f(-0.1f, -1.3f);

    glEnd();
    glPopMatrix();
}
//desenha inimigo mais acima
void DesenhaCapacete()
{
    glColor3f(0.6f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 1.2); //lado esquerdo cabeca
    glVertex2f(0, 2);
    glVertex2f(1, 2);
    glVertex2f(2, 1.2); //lado direito cabeca
    glVertex2f(2, 2);
    glVertex2f(1, 2);
    glVertex2f(0, 1); //mandibula esquerda
    glVertex2f(0, 0);
    glVertex2f(0.8, 1);
    glVertex2f(2, 1); //mandibula direita
    glVertex2f(2, 0);
    glVertex2f(1.2, 1);
    glVertex2f(0, 0); //buraco boca
    glVertex2f(2, 0);
    glVertex2f(1, 0.9);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.5, 0.5); // boca
    glVertex2f(1.5, 0.5);
    glVertex2f(1, 0.1);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);    //olhos
    glVertex2f(0.5, 1.3); //esquerdo
    glVertex2f(0.5, 1.5);
    glVertex2f(0.7, 1.5);
    glVertex2f(0.7, 1.3);
    glVertex2f(1.3, 1.3); //direito
    glVertex2f(1.3, 1.5);
    glVertex2f(1.5, 1.5);
    glVertex2f(1.5, 1.3);
    glEnd();
}
//desenha inimigo do meio
void DesenhaAndroid()
{
    glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0 - 0.2, 2); //buraco testa
    glVertex2f(2.2, 2);
    glVertex2f(1, 1.4);
    glVertex2f(0, 0.9); //lado esquerdo cabeca
    glVertex2f(0, 1.7);
    glVertex2f(0.6, 1.4);
    glVertex2f(2, 0.9); //lado direito cabeca
    glVertex2f(2, 1.7);
    glVertex2f(1.4, 1.4);
    glVertex2f(0.2, 0); //mandibula esquerda
    glVertex2f(0.2, 0.7);
    glVertex2f(0.6, 0);
    glVertex2f(1.8, 0); //mandibula direita
    glVertex2f(1.8, 0.7);
    glVertex2f(1.4, 0);
    glVertex2f(0.5, 0.5); //buraco boca
    glVertex2f(1.4, 0.5);
    glVertex2f(1, 0 - 0.3);
    glEnd();
    glBegin(GL_QUADS);    //olhos
    glVertex2f(0.5, 0.7); //esquerdo
    glVertex2f(0.5, 0.9);
    glVertex2f(0.7, 0.9);
    glVertex2f(0.7, 0.7);
    glVertex2f(1.3, 0.7); //direito
    glVertex2f(1.3, 0.9);
    glVertex2f(1.5, 0.9);
    glVertex2f(1.5, 0.7);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.9, 0.3); //bigode
    glVertex2f(0.9, 0.5);
    glVertex2f(1.1, 0.5);
    glVertex2f(1.1, 0.3);
    glEnd();
}

//desenha o inimigo mais abaixo
void DesenhaCaveira()
{
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 2);
    glVertex2f(2, 2);
    glVertex2f(2, 0);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0); //boca
    glVertex2f(2, 0);
    glVertex2f(1, 0.9);
    glVertex2f(0, 1.5); //lado esquerdo cabeca
    glVertex2f(0, 2);
    glVertex2f(0.6, 2);
    glVertex2f(2, 1.5); //lado direito cabeca
    glVertex2f(2, 2);
    glVertex2f(1.4, 2);
    glVertex2f(0, 1); //mandibula esquerda
    glVertex2f(0, 0.2);
    glVertex2f(0.8, 1);
    glVertex2f(2, 1); //mandibula direita
    glVertex2f(2, 0.2);
    glVertex2f(1.2, 1);
    glEnd();
    glBegin(GL_QUADS);    //olhos
    glVertex2f(0.5, 1.3); //esquerdo
    glVertex2f(0.5, 1.5);
    glVertex2f(0.8, 1.5);
    glVertex2f(0.8, 1.3);
    glVertex2f(1.2, 1.3); //direito
    glVertex2f(1.2, 1.5);
    glVertex2f(1.5, 1.5);
    glVertex2f(1.5, 1.3);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.8, 0.4); //bigode
    glVertex2f(0.8, 0.6);
    glVertex2f(1.2, 0.6);
    glVertex2f(1.2, 0.4);
    glEnd();
}

//desenha o inimigo recebido, chamando a funcao correta para cada tipo
void DesenhaInvader(struct enemy inv)
{
    glPushMatrix();
    glTranslatef(inv.x, inv.y, 0);
    if (inv.type >= 4)
    {
        DesenhaCaveira();
    }
    else if (inv.type >= 2)
    {
        DesenhaAndroid();
    }
    else
    {
        DesenhaCapacete();
    }
    glPopMatrix();
}
//desenha a defesa
void DesenhaDef(struct def defesa)
{
    glPushMatrix();
    glTranslatef(defesa.x, defesa.y, 0);
    glColor3f(0.0f, defesa.g, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();
    glPopMatrix();
}
//faz a explosao
void desenha_explo(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    int tri_number = 20;
    GLfloat doispi = 2.0f * PI;
    glLineWidth(1);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    glColor3f(1.0f, 1.0f, 0.0f);
    for (int i = 0; i <= tri_number; i++)
    {
        glVertex2f((cos(i * doispi / tri_number)), sin(i * doispi / tri_number));
    }
    glEnd();
    glPopMatrix();
}
void flashScreen()
{
    glClearColor(0.6, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
}

//desenha o tiro do inimigo
void desenha_tiro(GLfloat x, GLfloat y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(1.0f, 0.0f, 0.1f);
    glLineWidth(2);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(0.1f, -0.2f);
    glVertex2f(0.1f, 0.2f);
    glVertex2f(-0.1f, 0.2f);
    glEnd();
    glPopMatrix();
}
//desenha os misseis do player
void DesenhaMissel(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2);
    glBegin(GL_POLYGON);
    glVertex2f(-0.1f, -0.2f);
    glVertex2f(-0.1f, 0.1f);
    glVertex2f(0.0f, 0.2f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(0.1f, -0.2f);
    glEnd();
    glPopMatrix();
}
//reseta o missel m para poder atirar novamente
void resetMissel(int m)
{
    misseis[m].y = -1;
    misseis[m].moving = false;
    misseis[m].x = aviao_x - 1 + 2 * m;
}
//dado um inimigo(alien) faz ele atirar
void atira(int alien)
{
    tiros.push_back(invasores[alien]);
}
//escolhe um inimigo randomicamente a cada 4 sec
//vai chamando mais rapido com o tempo ate chegar em 1 por segundo
void escolhe_Alien(int passo)
{
    if (invasores.size() < tiros.size())
        return;
    int alien = rand() % invasores.size();
    atira(alien);

    glutTimerFunc((4000 - passo > 1000 ? 4000 - passo : 1000), escolhe_Alien, passo * 1.3);
}

bool checkBoundingBox(Point p0, Point p1, float size)
{
    return !(p0.x < p1.x || p0.x > (p1.x + size) || p0.y < p1.y || p0.y > (p1.y + size));
}

void Collider(int passo)
{
    //Point p;
    //ve se os tiros dados pelos invasores acertaram algo
    for (int i = 0; i < tiros.size(); i++)
    {
        if (tiros[i].y <= 4)
        { //distancia em que estao as defesas, portanto checa elas
            for (int j = 0; j < defesa.size(); j++)
            {
                // if ((tiros[i].x - defesa[j].x >= 0 && tiros[i].x - defesa[j].x <= 1) && tiros[i].y - defesa[j].y <= 1)
                if (checkBoundingBox(Point(tiros[i].x, tiros[i].y), Point(defesa[j].x, defesa[j].y), 1))
                {
                    // p.x = tiros[i].x;
                    // p.y = tiros[i].y;
                    explosoes.push_back(Point(tiros[i].x, tiros[i].y));
                    //desenha_explo(tiros[i].x, tiros[i].y);
                    tiros.erase(tiros.begin() + i);
                    defesa[j].g -= 0.25f;
                    if (defesa[i].g <= 0)
                    {
                        defesa.erase(defesa.begin() + i);
                    }
                    break;
                }
            }
        }
        //if (tiros[i].y <= 0 && tiros[i].y > -2 && (tiros[i].x - aviao_x > -1 && tiros[i].x - aviao_x < 1))
        if (checkBoundingBox(Point(tiros[i].x, tiros[i].y), Point(aviao_x - 1, -1), 2))
        { //distancia em que esta o player, checa ele
            lives--;
            flash = true;
            // p.x = tiros[i].x;
            // p.y = tiros[i].y;
            explosoes.push_back(Point(tiros[i].x, tiros[i].y));
            //desenha_explo(tiros[i].x, tiros[i].y);

            gameOver = (lives > 0) ? false : true;
            tiros.erase(tiros.begin() + i);
        }
        if (tiros[i].y <= -2)
        {
            tiros.erase(tiros.begin() + i);
        }
    }
    for (int j = 0; j < misseis.size(); j++)
    {
        if (misseis[j].y > 30)
        {
            resetMissel(j);
        }
        for (int i = 0; i < defesa.size(); i++)
        {
            //   if (abs(misseis[j].x - (defesa[i].x + 0.5)) < 0.5 && (defesa[i].y - misseis[j].y) < 0.4)
            if (checkBoundingBox(Point(misseis[j].x, misseis[j].y), Point(defesa[i].x, defesa[i].y), 1))
            {
                defesa[i].g -= 0.25f;
                // p.x = misseis[j].x;
                // p.y = misseis[j].y;
                explosoes.push_back(Point(misseis[j].x, misseis[j].y));
                //  desenha_explo(misseis[j].x, misseis[j].y);
                resetMissel(j);
                //printf("HIT");

                if (defesa[i].g <= 0)
                {
                    defesa.erase(defesa.begin() + i);
                }
            }
        }
        //ve se o tiro acertou um invasor
        for (int i = 0; i < invasores.size(); i++)
        {
            //if (abs(misseis[j].x - (invasores[i].x + 1)) < 1 && (invasores[i].y - misseis[j].y) < 0.4)
            if (checkBoundingBox(Point(misseis[j].x, misseis[j].y), Point(invasores[i].x, invasores[i].y), 2))
            {
                // p.x = misseis[j].x;
                // p.y = misseis[j].y;
                explosoes.push_back(Point(misseis[j].x, misseis[j].y));
                //desenha_explo(misseis[j].x, misseis[j].y);

                resetMissel(j);

                score += invasores[i].type > 3 ? 10 : (invasores[i].type > 1 ? 20 : 40);
                invasores.erase(invasores.begin() + i);
            }
        }
    }
    //glutPostRedisplay();
    glutTimerFunc(passo, Collider, passo);
}

void MoveAll(int passo)
{
    move_invader(&forward);
    move_tiro();
    move_misseis();
    glutPostRedisplay();
    Collider(0);
    glutTimerFunc(passo, MoveAll, passo);
}

// Fun��o callback de redesenho da janela de visualiza��o
void Desenha(void)
{
    // Muda para o sistema de coordenadas do modelo
    glMatrixMode(GL_MODELVIEW);
    // Inicializa a matriz de transforma��o corrente
    glLoadIdentity();

    // Limpa a janela de visualiza��o com a cor
    // de fundo definida previamente
    glClear(GL_COLOR_BUFFER_BIT);

    //escreve na tela a pontuacao
    std::string pts = ("Score ");
    pts += std::to_string(score);

    drawText(pts.c_str(), pts.length(), -19.5, 29.5, 0.008, 1, 1, 1);

    //escreve na tela quanta vidas
    pts = ("Lives ");
    pts += std::to_string(lives);

    drawText(pts.c_str(), pts.length(), -19.5, 28, 0.008, 0.7, 0.7, 1);

    //manda desenhar as defesas
    for (int i = 0; i < defesa.size(); i++)
    {
        DesenhaDef(defesa[i]);
    }
    //manda desenhar os invasores
    for (int i = 0; i < invasores.size(); i++)
    {
        DesenhaInvader(invasores[i]);
    }

    for (int i = 0; i < tiros.size(); i++)
    {
        desenha_tiro(tiros[i].x, tiros[i].y);
    }
    for (int i = 0; i < misseis.size(); i++)
    {
        DesenhaMissel(misseis[i].x, misseis[i].y);
    }
    if (flash)
    {
        flashScreen();
        flash = false;
    }
    for (int i = 0; i < explosoes.size(); i++)
    {
        desenha_explo(explosoes[i].x, explosoes[i].y);
    }
    explosoes.clear();
    //se "pausado" escreve game over na tela
    //se nao ha mais inimigos escreve you win

    if (gameOver)
        drawText("Game Over", 9, -10, 15, 0.025, 1, 0, 0);
    if (invasores.size() == 0)
    {
        drawText("You Win", 7, -10, 15, 0.025, 1, 1, 1);
    }

    //ve se o tiro saiu do cenario
    DesenhaAviao();

    // Executa os comandos OpenGL
    glutSwapBuffers();
}

// Fun��o callback chamada quando o tamanho da janela � alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    GLsizei largura, altura;

    // Evita a divisao por zero
    if (h == 0)
        h = 1;

    // Atualiza as vari�veis
    largura = w;
    altura = h;

    // Especifica as dimens�es da Viewport
    glViewport(0, 0, largura, altura);

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Estabelece a janela de sele��o (esquerda, direita, inferior,
    // superior) mantendo a propor��o com a janela de visualiza��o
    if (largura <= altura)
    {
        gluOrtho2D(-20.0f, 20.0f, -3.0f * altura / largura, 31.0f * altura / largura);
        //win = 1.0f;
    }
    else
    {
        gluOrtho2D(-20.0f * largura / altura, 20.0f * largura / altura, -3.0f, 31.0f);
        //	win = 1.0f*largura/altura;
    }
}

// Fun��o callback chamada para gerenciar eventos de teclas especiais(F1,PgDn,...)
void TeclasEspeciais(int key, int x, int y)
{
    // printf("%f\n",aviao_x);
    // Move a base
    if (key == GLUT_KEY_LEFT && !gameOver)
    {
        aviao_x -= 0.5f;
        if (aviao_x < -19)
            aviao_x = -19;
    }
    if (key == GLUT_KEY_RIGHT && !gameOver)
    {
        aviao_x += 0.5f;
        if (aviao_x > 19)
            aviao_x = 19;
    }
    if (key == GLUT_KEY_UP && misseis[0].moving != true)
    {
        misseis[0].moving = true;
    }
    if (key == GLUT_KEY_DOWN && misseis[1].moving != true)
    {
        misseis[1].moving = true;
    }

    //glutTimerFunc(10, move_misseis, 20);
    for (int i = 0; i < misseis.size(); i++)
    {
        if (misseis[i].moving == false)
        {
            misseis[i].x = aviao_x - 1 + 2 * i;
        }
    }

    glutPostRedisplay();
}

// Fun��o callback chamada para gerenciar eventos de teclas
void Teclado(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
    case 'p': //
        gameOver = !gameOver;
        break;
    }
}

// Fun��o respons�vel por inicializar par�metros e vari�veis
void Inicializa(void)
{
    // Define a cor de fundo da janela de visualiza��o como branca
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(-20, 20, -3, 31);
}

// Programa Principal
int main(int argc, char *argv[])
{
    glutInit(&argc, argv); // Inicia uma inst�ncia da glut
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    //glGetString(GL_VERSION)
    // glDepthMask(false);
    glutInitWindowPosition(512, 192);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Space Invaders");

    if (!inicializado)
    {
        inicializa();
        inicializado = true;
    }

    glutTimerFunc(0, MoveAll, 5);
    //    glutTimerFunc(0, Collider, 5);
    //glutTimerFunc(0, move_invader, 30);   //faz a chamada pros inimigos se moverem
    glutTimerFunc(0, escolhe_Alien, 100); //faz a chamada pra escolher qual atirara
    //move_tiro(10);                        //faz o tiro se mover
    //move_misseis(10);
    // Registra a fun��o callback de redesenho da janela de visualiza��o
    glutDisplayFunc(Desenha);
    glMatrixMode(GL_PROJECTION);
    // Registra a fun��o callback de redimensionamento da janela de visualiza��o
    // Registra a fun��o callback para tratamento das teclas especiais
    glutSpecialFunc(TeclasEspeciais);

    // Registra a fun��o callback para tratamento das teclas ASCII
    glutKeyboardFunc(Teclado);

    // Chama a fun��o respons�vel por fazer as inicializa��es

    //glutTimerFunc(0, move_misseis, 0); // Timer para mover o missel 1
    Inicializa();
    glutReshapeFunc(AlteraTamanhoJanela);

    // Inicia o processamento e aguarda intera��es do usu�rio
    glutMainLoop();

    return 0;
}
