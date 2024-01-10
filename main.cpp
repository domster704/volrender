#include "GL/glew.h"
#include "GL/glut.h"
#include <string>
#include <vector>

// https://en.wikipedia.org/wiki/BMP_file_format
// pragma pack(push, 1) - устанавливает выравнивание структур в 1 байт
#pragma pack(push, 1)
// struct BMPHeader - структура BMP-файла с заголовком
struct BMPHeader {
    char signature[2]; // сигнатура BMP-файла
    uint32_t fileSize; // размер BMP-файла в байтах
    uint16_t reserved1; // зарезервированное поле
    uint16_t reserved2; // зарезервированное поле
    uint32_t dataOffset; // положение данных в BMP-файле
    uint32_t headerSize; // размер заголовка в байтах
    int32_t width; // ширина изображения в пикселях
    int32_t height; // высота изображения в пикселях
    uint16_t planes; // число плоскостей
    uint16_t bitsPerPixel; // число бит на пиксель
    uint32_t compression; // тип сжатия
    uint32_t imageSize; // размер изображения в байтах
    int32_t xPixelsPerMeter; // число пикселей в метре
    int32_t yPixelsPerMeter; // число пикселей в метре
    uint32_t colorsUsed; // число используемых цветов
    uint32_t colorsImportant; // число важных цветов
};
// pragma pack(pop) - сбрасывает выравнивание структур
#pragma pack(pop)

const int SLICES = 16;
const int TEX_SIZE = 512;
const int MAX_SLICES = 9;

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

GLuint texID; // ID текстуры для рисования срезов

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
 * Функция загрузки текстуры из BMP файла
 * @param filename - имя файла
 * @param index - индекс текстуры
 */
void loadTexture(const char* filename, int index) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: could not open file %s\n", filename);
        return;
    }

    // Чтение заголовка BMP
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file);
    // Получение ширины и высоты
    int width = *(int*) &header[18];
    int height = *(int*) &header[22];
    // Получение размера изображения
    int size = width * height * 3;
    // Выделение памяти для изображения
    auto* data = new unsigned char[size];
    // Чтение изображения
    fread(data, sizeof(unsigned char), size, file);
    fclose(file);
    // Выделение памяти для текстуры
    auto* texData = new unsigned char[size / 3 * 4];
    // Преобразование RGB в RGBA
    for (int i = 0; i < height * width; i++) {
        unsigned char r = data[i * 3 + 2];
        unsigned char g = data[i * 3 + 1];
        unsigned char b = data[i * 3 + 0];

        texData[i * 4 + 0] = r;
        texData[i * 4 + 1] = g;
        texData[i * 4 + 2] = b;
        texData[i * 4 + 3] = 255;
        if (r + g + b < 40) {
            texData[i * 4 + 3] = 0;
        }
    }

    // Загрузка подготовленного текстурированного изображения в текстуру
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    // Удаление буфферов для освобождения памяти
    delete[] data;
    delete[] texData;
}

/**
 * Функция инициализации отображения срезов
 */
void initGL() {
    // Включает буфер глубины
    glEnable(GL_DEPTH_TEST);
    // Включает буфер с прозрачностью
    glEnable(GL_BLEND);
    // Устанавливает режим смешивания
    // GL_SRC_ALPHA - прозрачность sfa
    // GL_ONE_MINUS_SRC_ALPHA - прозрачность
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Генерация текстуры для срезов и загрузка изображения в текстуру
    // GL_TEXTURE_WRAP_S - повторение текстуры по горизонтали
    // GL_TEXTURE_WRAP_T - повторение текстуры по вертикали
    // GL_TEXTURE_WRAP_R - повторение текстуры по глубине
    // GL_TEXTURE_MAG_FILTER - фильтр магнитуд
    // GL_TEXTURE_MIN_FILTER - фильтр минимального значения
    // GL_LINEAR - линейный фильтр
    // GL_NEAREST - ближайшее сопоставление
    // GL_TEXTURE_3D - 3D текстура
    // GL_CLAMP_TO_EDGE - повторение текстуры по границам
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_3D, texID);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Загрузка изображения в текстуру
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, TEX_SIZE, TEX_SIZE, SLICES, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Загрузка срезов
    char filename[256];
    int index = 1;
    for (int i = 1; i < SLICES; i++) {
        if (index > MAX_SLICES) {
            index = 1;
        }
        sprintf(filename, "C:/Users/isupo/YandexDisk/Programming/C++/OpenGL/Sem3/assets/0%d.bmp", index);
        loadTexture(filename, index);
        index++;
    }
}

/**
 * Функция рисования срезов с использованием текстуры
 */
void drawSlices() {
    // GL_TEXTURE_3D - 3D текстура
    // Включаем 3D текстуру
    glEnable(GL_TEXTURE_3D);

    // Биндим текстуру
    glBindTexture(GL_TEXTURE_3D, texID);

    // Устанавливаем режим текстуры
    // GL_TEXTURE_ENV - режим текстуры
    // GL_TEXTURE_ENV_MODE - режим текстуры
    // GL_REPLACE - заменить текстуру
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Устанавливаем цвет текстуры
    glColor4f(1.0f, 1.0f, 1.0f, 0.1f); // Use a low alpha value for transparency

    for (int i = 0; i < SLICES; i++) {
        // Срез текстуры
        float z = (float) i / (float) SLICES;
        // Рисуем квадрат с текстурой в данном срезе
        glBegin(GL_QUADS);
        glTexCoord3f(0.0f, 0.0f, z);
        glVertex3f(-1.0f, -1.0f, z * 2.0f - 1.0f);
        glTexCoord3f(1.0f, 0.0f, z);
        glVertex3f(1.0f, -1.0f, z * 2.0f - 1.0f);
        glTexCoord3f(1.0f, 1.0f, z);
        glVertex3f(1.0f, 1.0f, z * 2.0f - 1.0f);
        glTexCoord3f(0.0f, 1.0f, z);
        glVertex3f(-1.0f, 1.0f, z * 2.0f - 1.0f);
        glEnd();
    }
    // Отключаем 3D текстуру
    glDisable(GL_TEXTURE_3D);
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
//    DrawCube({-1.5, 1.5, -1.5}, 0.5, {1, 0, 0, 1});
//    DrawCube({1.5, 1.5, 1.5}, 0.5, {0, 1, 0, 1});
//    DrawCube({1.5, -1.5, 1.5}, 0.5, {0, 1, 0, 1});
//    DrawCube({0, 0, 0}, 0.5, {0, 0, 1, 1});


    drawSlices();

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

    initGL();

    glutDisplayFunc(display);
    glutMotionFunc(mouseMovePressed);
    glutMouseFunc(mousePressed);

    // Запуск цикла GLUT
    glutMainLoop();
    return 0;
}