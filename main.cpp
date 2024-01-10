#include "GL/glew.h"
#include "GL/glut.h"

bool mouseLeftDown, mouseRightDown;
float mouseX = {0}, mouseY = {0};

float cameraAngleX = {0};
float cameraAngleY = {0};

float scale = {0.2f};

const float cameraZoomK = 0.01f;
const float cameraAngleK = 0.1f;

struct Vec3 {
    float x = {0};
    float y = {0};
    float z = {0};
};

struct Vec4 {
    float x = {0};
    float y = {0};
    float z = {0};
    float alpha = {0};
};

/**
 * Функция для рисования куба
 *
 * @param pos - координаты в пространстве XYZ
 * @param size - размер стороны куба
 * @param color - цвет для окрашивания сторон куба
 */
void DrawCube(Vec3 pos, float size, Vec4 color) {
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    // Сторона - ПЕРЕДНЯЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x, color.y, color.z, color.alpha);
    glVertex3f(x + size, y - size, z - size);
    glVertex3f(x + size, y + size, z - size);
    glVertex3f(x - size, y + size, z - size);
    glVertex3f(x - size, y - size, z - size);
    glEnd();

    // Сторона - ЗАДНЯЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x * 0.95f, color.y * 0.95f, color.z * 0.95f, color.alpha);
    glVertex3f(x + size, y - size, z + size);
    glVertex3f(x + size, y + size, z + size);
    glVertex3f(x - size, y + size, z + size);
    glVertex3f(x - size, y - size, z + size);
    glEnd();

    // Сторона — ПРАВАЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x * 0.9f, color.y * 0.9f, color.z * 0.9f, color.alpha);
    glVertex3f(x + size, y - size, z - size);
    glVertex3f(x + size, y + size, z - size);
    glVertex3f(x + size, y + size, z + size);
    glVertex3f(x + size, y - size, z + size);
    glEnd();

    // Сторона — ЛЕВАЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x * 0.85f, color.y * 0.85f, color.z * 0.85f, color.alpha);
    glVertex3f(x - size, y - size, z + size);
    glVertex3f(x - size, y + size, z + size);
    glVertex3f(x - size, y + size, z - size);
    glVertex3f(x - size, y - size, z - size);
    glEnd();

    // Сторона — ВЕРХНЯЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, color.alpha);
    glVertex3f(x + size, y + size, z + size);
    glVertex3f(x + size, y + size, z - size);
    glVertex3f(x - size, y + size, z - size);
    glVertex3f(x - size, y + size, z + size);
    glEnd();

    // Сторона — НИЖНЯЯ
    glBegin(GL_POLYGON);
    glColor4f(color.x * 0.75f, color.y * 0.75f, color.z * 0.75f, color.alpha);
    glVertex3f(x + size, y - size, z - size);
    glVertex3f(x + size, y - size, z + size);
    glVertex3f(x - size, y - size, z + size);
    glVertex3f(x - size, y - size, z - size);
    glEnd();
}

/**
 * Функция для рендеринга окна в буфере цвета RGBA (8 бит на пиксель) и буфер глубины (8 бит на пиксель).
 * По умолчанию буфер глубины отключен. Буфер цвета включен.
 */
void display() {
    // Очищает буфер цвета и буфер глубины
    // GL_COLOR_BUFFER_BIT -Указывает, какие буферы в данный момент включены для записи цвета.
    // GL_DEPTH_BUFFER_BIT - Указывает буфер глубины.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Сбрасывает/заново загружает матрицу проекций/вида
    glLoadIdentity();
    glScalef(scale, scale, scale);
    glRotatef(cameraAngleX, 1, 0, 0);
    glRotatef(cameraAngleY, 0, 1, 0);

    DrawCube({-1.5, -1.5, -1.5}, 0.5, {1, 0, 0, 1});
    DrawCube({-1.5, 1.5, -1.5}, 0.5, {1, 0, 0, 1});
    DrawCube({1.5, 1.5, 1.5}, 0.5, {0, 1, 0, 1});
    DrawCube({1.5, -1.5, 1.5}, 0.5, {0, 1, 0, 1});
    DrawCube({0, 0, 0}, 0.5, {0, 0, 1, 1});

    // Очищает буфер цвета
    glFlush();
    // Смена буферов
    glutSwapBuffers();
}

/**
 * Обработчик нажатия кнопки мыши
 * @param button - id нажатой кнопки
 * @param state - состояние нажатой кнопки
 * @param x - x координата нажатой кнопки
 * @param y - y координата нажатой кнопки
 */
void mousePressed(int button, int state, int x, int y) {
    mouseX = (float) x;
    mouseY = (float) y;

    switch (button) {
        case GLUT_MIDDLE_BUTTON:
            break;
        case GLUT_LEFT_BUTTON:
            mouseLeftDown = state == GLUT_DOWN;
            break;
        case GLUT_RIGHT_BUTTON:
            mouseRightDown = state == GLUT_DOWN;
            break;
        case 3:
            // Приблизить
            scale += cameraZoomK;
            break;
        case 4:
            // Отдалить
            scale -= cameraZoomK;
            break;
        default:
            break;
    }
    // Перерисовать окно
    glutPostRedisplay();
}

/**
 * Обработчик движения мыши
 * @param x - x координата нажатой кнопки
 * @param y - y координата нажатой кнопки
 */
void mouseMovePressed(int x, int y) {
    // Поворот камеры
    if (mouseLeftDown) {
        cameraAngleY += (x - mouseX) * cameraAngleK;
        cameraAngleX += (y - mouseY) * cameraAngleK;
    }
    // Переместить камеру
    mouseY = (float) y;
    mouseX = (float) x;
    // Перерисовать окно
    glutPostRedisplay();
}


int main(int argc, char* argv[]) {
    // Инициализация GLUT
    glutInit(&argc, argv);

    // GLUT_RGB - Битовая маска для выбора окна режима RGBA
    // GLUT_DOUBLE - Режим двойной буферизации
    // GLUT_DEPTH - Буфер глубины
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("VolRender");
    glutReshapeWindow(900, 900);

    // Инициализация GLEW
    // GL_TRUE - Включает поддержку расширений OpenGL
    glewExperimental = GL_TRUE;
    glewInit();

    // Включает буфер глубины
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutMotionFunc(mouseMovePressed);
    glutMouseFunc(mousePressed);

    // Запуск цикла GLUT
    glutMainLoop();
    return 0;
}