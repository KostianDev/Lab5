#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define n1 3
#define n2 1
#define n3 0
#define n4 3
#define N (10 + n3)
#define max_rand 199
#define min_rand 0
#define k (1.0 - n3 * 0.01 - n4 * 0.005 - 0.15)
#define WIDTH 800
#define HEIGHT 600
#define seed (1000 * n1 + 100 * n2 + 10 * n3 + n4)
#define arrowAngle  (M_PI / 5.0)
#define sizeMult 3
#define shiftAngle (M_PI / 18.0)
#define END_CODE (-100)

typedef struct passMatrix {
    double matrix[N][N];
} matrix;

typedef struct q {
    int queue[N];
    int rear;
    int front;
} queue;

typedef struct stack {
    int stack[N * N];
    int top;
    int prevVertex;
} stack;

typedef struct newOrder {
    int array[N];
    int place;
} array;

typedef struct node {
    int key;
    int x;
    int y;
    int pos;
    int state;
    struct node *next_node;
} l_list;

bool KEYS[322];

int startKey_init(matrix graphMatrix);

l_list *l_list_init(int key, int x, int y, int pos, int state);

l_list *addto_list(l_list *l_pointer, int key, int x, int y, int pos, int state);

l_list *delfrom_start(l_list *l_pointer);

l_list *find_num(l_list *l_pointer, int key);

matrix matrix_init();

matrix generateDirectedMatrix();

matrix matrixMult(matrix matrixA, matrix matrixB);

matrix powMatrix(matrix passMatrix, int degree);

void printMatrix(matrix passMatrix, FILE *fptr, const char[]);

void clearScreen(SDL_Renderer *Renderer);

void drawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius);

void drawBezierCurve(SDL_Renderer *renderer, int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4);

void drawArrowHead(SDL_Renderer *renderer, int endX, int endY, int gap, double angle, SDL_Color color);

void drawVertexNumber(SDL_Renderer *renderer, int number, int x, int y, int gap, SDL_Color color);

l_list *drawGraph(SDL_Renderer *renderer, SDL_Window *window, matrix matrix, l_list *list_ptr, int size, int *r);

void drawDirConnections(SDL_Renderer *renderer, l_list *node1, l_list *node2,
                        int r, int dir, int width, int height, int size, SDL_Color color);

l_list *reDraw(SDL_Renderer *renderer, SDL_Window *window, matrix graphMatrix, l_list *list_ptr, int *r);

queue q_init();

queue enqueue(int vertex, queue queue);

queue dequeue(queue queue, int *vertex);

queue BFS(matrix graphMatrix, l_list *list_ptr, SDL_Renderer *renderer, SDL_Window *window,
          int r, queue queue, int *flag, int startKey, FILE *fptr, int *isCurrent, matrix *treeMatrix);

stack stack_init();

stack DFS(matrix graphMatrix, l_list *list_ptr, SDL_Renderer *renderer, SDL_Window *window,
          int r, int startKey, int *place, FILE *fptr, matrix *treeMatrix, array *array, stack stack);

int main(int argc, char *argv[]) {
    srand(seed);

    FILE *fptr, *fptr2;
    int r1, r2, flag = 0, BFSflag = 1, BFSstartKey, DFSstartKey, isCurrent = -1, finished, place = -1;
    char ch1[] = "|", ch2[] = "V";
    queue queue1 = q_init();
    stack stack1 = stack_init();
    array array1 = {
            .array = {0},
            .place = 0,
    };

    TTF_Init();

    matrix directedMatrix = generateDirectedMatrix();
    matrix treeMatrixBFS = matrix_init();
    matrix treeMatrixDFS = matrix_init();

    fptr = fopen("Output.txt", "a");
    printMatrix(directedMatrix, fptr, "directed graph ");
    fclose(fptr);
    fptr2 = fopen("Output2.txt", "a");
    printMatrix(directedMatrix, fptr2, "directed graph ");
    fclose(fptr2);

    BFSstartKey = startKey_init(directedMatrix);
    DFSstartKey = BFSstartKey;

    l_list *list1_ptr, *list2_ptr, *test_node1, *test_node2;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *directedWindowBFS = SDL_CreateWindow("Directed Graph (BFS)", SDL_WINDOWPOS_UNDEFINED,
                                                     SDL_WINDOWPOS_UNDEFINED, WIDTH,
                                                     HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *directedRendererBFS = SDL_CreateRenderer(directedWindowBFS, -1,
                                                           SDL_RENDERER_ACCELERATED);

    SDL_Window *directedWindowDFS = SDL_CreateWindow("Directed Graph (DFS)", SDL_WINDOWPOS_UNDEFINED,
                                                     SDL_WINDOWPOS_UNDEFINED, WIDTH,
                                                     HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *directedRendererDFS = SDL_CreateRenderer(directedWindowDFS, -1,
                                                           SDL_RENDERER_ACCELERATED);

    SDL_SetWindowResizable(directedWindowBFS, SDL_TRUE);
    SDL_SetWindowResizable(directedWindowDFS, SDL_TRUE);

    SDL_Event event;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        list1_ptr = reDraw(directedRendererBFS, directedWindowBFS,
                                           directedMatrix, list1_ptr, &r1);
                        list2_ptr = reDraw(directedRendererDFS, directedWindowDFS,
                                           directedMatrix, list2_ptr, &r2);

                        for (int i = 0; i < N; ++i) {
                            queue1.queue[i] = -1;
                            stack1.stack[i] = -1;
                            array1.array[i] = 0;
                        }
                        queue1.rear = -1;
                        queue1.front = 0;
                        stack1.top = 0;
                        BFSflag = 1;
                        array1.place = 0;
                        break;
                    } else if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                        quit = 1;
                        break;
                    }
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym < 322)
                        KEYS[event.key.keysym.sym] = true;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym < 322)
                        KEYS[event.key.keysym.sym] = false;
                    break;
            }
        }

        if (!flag) {
            list1_ptr = reDraw(directedRendererBFS, directedWindowBFS,
                               directedMatrix, list1_ptr, &r1);
            list2_ptr = reDraw(directedRendererDFS, directedWindowDFS,
                               directedMatrix, list2_ptr, &r2);

            flag = 1;
        }

        if (KEYS[SDLK_n]) {
            queue1 = BFS(directedMatrix, list1_ptr, directedRendererBFS,
                         directedWindowBFS, r1, queue1, &BFSflag,
                         BFSstartKey, fptr, &isCurrent, &treeMatrixBFS);
            KEYS[SDLK_n] = false;

            if (queue1.front == END_CODE) {
                BFSstartKey = N;
                while (BFSstartKey >= 1) {
                    test_node1 = find_num(list1_ptr, BFSstartKey);
                    if (test_node1->state == 0) {
                        BFSstartKey = test_node1->key;

                        for (int i = 0; i < N; ++i)
                            queue1.queue[i] = -1;
                        queue1.rear = -1;
                        queue1.front = 0;
                        BFSflag = 1;
                        break;
                    } else if (BFSstartKey == 1) {
                        if (BFSflag) {
                            fptr = fopen("Output.txt", "a");
                            fprintf(fptr, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr, "%4s", ch1);
                            fprintf(fptr, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr, "%4s", ch2);
                            fprintf(fptr, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr, "%4d", i + 1);
                            fprintf(fptr, "\n\nThe BFS algorithm was successfully finished!\n\n");
                            fprintf(fptr, "Here is your tree's matrix:\n\n");
                            for (int i = 0; i < N; ++i) {
                                for (int j = 0; j < N; ++j) {
                                    fprintf(fptr, "%3.0lf", treeMatrixBFS.matrix[i][j]);
                                }
                                fprintf(fptr, "\n");
                            }
                            fprintf(fptr, "\n");
                            fclose(fptr);
                            BFSflag = 0;
                        }
                        break;
                    }
                    BFSstartKey--;
                }
            }

        }

        if (KEYS[SDLK_m]) {
            stack1 = DFS(directedMatrix, list2_ptr, directedRendererDFS,
                         directedWindowDFS, r2, DFSstartKey, &place,
                         fptr, &treeMatrixDFS, &array1, stack1);
            KEYS[SDLK_m] = false;

            if (stack1.top == END_CODE) {
                DFSstartKey = N;
                while (DFSstartKey >= 1) {
                    test_node2 = find_num(list2_ptr, DFSstartKey);
                    if (test_node2->state == 0) {
                        DFSstartKey = test_node2->key;

                        for (int i = 0; i < N; ++i)
                            stack1.stack[i] = -1;
                        stack1.top = 0;
                        break;
                    } else if (DFSstartKey == 1) {
                        if (array1.place == N) {
                            fptr2 = fopen("Output2.txt", "a");
                            fprintf(fptr2, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr2, "%4s", ch1);
                            fprintf(fptr2, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr2, "%4s", ch2);
                            fprintf(fptr2, "\n");
                            for (int i = 0; i < N; ++i)
                                fprintf(fptr2, "%4d", i + 1);
                            fprintf(fptr2, "\n\nThe DFS algorithm was successfully finished!\n\n");
                            fprintf(fptr2, "Here is your tree's matrix:\n\n");
                            for (int i = 0; i < N; ++i) {
                                for (int j = 0; j < N; ++j) {
                                    fprintf(fptr2, "%3.0lf", treeMatrixDFS.matrix[i][j]);
                                }
                                fprintf(fptr2, "\n");
                            }
                            fprintf(fptr2, "\n");
                            fclose(fptr2);
                            array1.place = 0;
                        }
                    }
                    DFSstartKey--;
                }
            }
        }

        if (KEYS[SDLK_r]) {
            list1_ptr = reDraw(directedRendererBFS, directedWindowBFS,
                               directedMatrix, list1_ptr, &r1);
            list2_ptr = reDraw(directedRendererDFS, directedWindowDFS,
                               directedMatrix, list2_ptr, &r2);

            for (int i = 0; i < N; ++i) {
                queue1.queue[i] = -1;
                stack1.stack[i] = -1;
                array1.array[i] = 0;
            }
            queue1.rear = -1;
            queue1.front = 0;
            stack1.top = 0;
            BFSflag = 1;
            array1.place = 0;
        }

    }

    while (list1_ptr != NULL)
        list1_ptr = delfrom_start(list1_ptr);
    while (list2_ptr != NULL)
        list2_ptr = delfrom_start(list2_ptr);

    SDL_DestroyWindow(directedWindowBFS);
    SDL_DestroyRenderer(directedRendererBFS);
    SDL_DestroyWindow(directedWindowDFS);
    SDL_DestroyRenderer(directedRendererDFS);

    SDL_Quit();

    unlink("Output.txt");
    unlink("Output2.txt");
    return 0;
}

int startKey_init(matrix graphMatrix) {
    int startKey;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (graphMatrix.matrix[i][j]) {
                startKey = i + 1;
                i = N;
                break;
            }
}

l_list *l_list_init(int key, int x, int y, int pos, int state) {
    l_list *l_pointer;
    l_pointer = malloc(sizeof(struct node));
    *l_pointer = (l_list) {
            .key = key,
            .x = x,
            .y = y,
            .pos = pos,
            .state = state,
            .next_node = NULL
    };
    return l_pointer;
}

l_list *addto_list(l_list *l_pointer, int key, int x, int y, int pos, int state) {
    l_list *new_node;
    new_node = malloc(sizeof(struct node));
    new_node->key = key;
    new_node->x = x;
    new_node->y = y;
    new_node->pos = pos;
    new_node->state = state;
    new_node->next_node = l_pointer;
    return new_node;
}

l_list *delfrom_start(l_list *l_pointer) {
    l_list *node_ptr;
    node_ptr = l_pointer->next_node;
    free(l_pointer);
    return node_ptr;
}

l_list *find_num(l_list *l_pointer, int key) {
    l_list *this_node = l_pointer;

    while (this_node != NULL) {
        if (this_node->key == key) return this_node;
        else this_node = this_node->next_node;
    }
    return NULL;
}

matrix matrix_init() {
    matrix matrix;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            matrix.matrix[i][j] = 0;
    return matrix;
}

matrix generateDirectedMatrix() {
    matrix passMatrix;
    double a;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            a = (rand() % (max_rand + 1 - min_rand) + min_rand) / 100.0;
            a *= k;
            passMatrix.matrix[i][j] = a < 1.0 ? 0 : 1;
        }
    return passMatrix;
}

matrix matrixMult(matrix matrixA, matrix matrixB) {
    matrix midMatrix = {{0}};

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int l = 0; l < N; ++l)
                midMatrix.matrix[i][j] += (int) (matrixA.matrix[i][l] * matrixB.matrix[l][j]);
    return midMatrix;
}

matrix powMatrix(matrix passMatrix, int degree) {
    if (degree > 1) {
        if (degree % 2 == 0)
            passMatrix = powMatrix(matrixMult(passMatrix, passMatrix), degree / 2);
        else
            passMatrix = matrixMult(passMatrix,
                                    powMatrix(matrixMult(passMatrix, passMatrix),
                                              (degree - 1) / 2));
    }
    return passMatrix;
}

void printMatrix(matrix passMatrix, FILE *fptr, const char graphName[]) {
    fprintf(fptr, "\nHere is your %s matrix:\n\n", graphName);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            fprintf(fptr, " %2.0lf", passMatrix.matrix[i][j]);
        }
        fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");
}

void clearScreen(SDL_Renderer *Renderer) {
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);
}

void drawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void drawBezierCurve(SDL_Renderer *renderer, int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4) {
    double xu, yu, u;
    for (int i = 0; i <= 10000; i += 5) {
        u = (double) i / 10000;
        xu = pow(1 - u, 3) * x1 + 3 * u * pow(1 - u, 2) * x2 + 3 * pow(u, 2) * (1 - u) * x3
             + pow(u, 3) * x4;
        yu = pow(1 - u, 3) * y1 + 3 * u * pow(1 - u, 2) * y2 + 3 * pow(u, 2) * (1 - u) * y3
             + pow(u, 3) * y4;
        SDL_RenderDrawPoint(renderer, (int) xu, (int) yu);
    }
}

void drawArrowHead(SDL_Renderer *renderer, int endX, int endY, int gap, double angle, SDL_Color color) {
    double arrowSize = (double) gap / 3 / sizeMult;

    double x1 = endX - arrowSize * cos(angle + arrowAngle);
    double y1 = endY - arrowSize * sin(angle + arrowAngle);
    double x2 = endX - arrowSize * cos(angle - arrowAngle);
    double y2 = endY - arrowSize * sin(angle - arrowAngle);

    SDL_Vertex vertex_1 = {{(float) endX, (float) endY},
                           color,
                           {1, 1}};
    SDL_Vertex vertex_2 = {{(float) x1, (float) y1},
                           color,
                           {1, 1}};
    SDL_Vertex vertex_3 = {{(float) x2, (float) y2},
                           color,
                           {1, 1}};

    SDL_Vertex vertices[] = {
            vertex_1,
            vertex_2,
            vertex_3
    };

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void drawVertexNumber(SDL_Renderer *renderer, int number, int x, int y, int gap, SDL_Color color) {
    TTF_Font *font = TTF_OpenFont("arial.ttf", gap / 3);
    char numberString[4];

    snprintf(numberString, sizeof(numberString), "%d", number);

    SDL_Surface *surface = TTF_RenderText_Solid(font, numberString, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    SDL_Rect rect = {x - texW / 2, y - texH / 2, texW, texH};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

l_list *drawGraph(SDL_Renderer *renderer, SDL_Window *window, matrix matrix, l_list *list_ptr, int size, int *r) {
    int width, height, xTopCircles, xLowCircles, yCircles, turn = 1, mid, dir;
    int gap, gap1, gap2, gap3, key = 1, pos = 1, flag = 1;

    if (size == 0) return NULL;

    SDL_GetWindowSize(window, &width, &height);

    SDL_Color color = {255, 255, 255, 0};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

    if (size < 4) {
        if (size == 1) {
            *r = (height > width) ? width / 8 : height / 8;
            gap = *r * sizeMult;

            drawCircle(renderer, width / 2, height / 2, *r);
            drawVertexNumber(renderer, key, width / 2, height / 2, gap, color);
            list_ptr = l_list_init(key, width / 2, height / 2, pos, 0);
        } else {
            *r = (height > width) ? width / 12 : height / 12;

            gap = *r * sizeMult;
            gap2 = (height - 2 * *r) / 2;
            gap3 = (width - 2 * gap - size * 2 * *r) / (size - 1);

            drawCircle(renderer, gap + *r, height / 2, *r);
            drawVertexNumber(renderer, key, gap + *r, height / 2, gap, color);
            list_ptr = l_list_init(key, gap + *r, height / 2, pos, 0);
            key++;

            for (int i = 1; i < size; ++i) {
                drawCircle(renderer, gap + *r + i * (2 * *r + gap3),
                           height / 2, *r);
                drawVertexNumber(renderer, key, gap + *r + i * (2 * *r + gap3),
                                 height / 2, gap, color);
                list_ptr = addto_list(list_ptr, key,
                                      gap + *r + i * (2 * *r + gap3), height / 2, pos, 0);
                key++;
            }
        }
    } else {

        mid = (abs(size - 5)) / 4;
        yCircles = 2 + mid * 2;
        xLowCircles = (size - yCircles) / 2;
        xTopCircles = size - xLowCircles - yCircles;

        *r = (height > width || (size >= 7 && size < 9)) ?
             width / (int) ((sizeMult + 1) * (xTopCircles + 1) + sizeMult) :
             height / ((sizeMult + 1) * (yCircles / 2 + 1) + sizeMult);

        gap = *r * sizeMult;
        gap1 = (width - 2 * gap - (xTopCircles + 1) * 2 * *r) / xTopCircles;
        gap2 = (height - (yCircles / 2 + 1) * 2 * *r) / (yCircles / 2 + 2);
        gap3 = (width - 2 * gap - (xLowCircles + 1) * 2 * *r) / xLowCircles;

        drawCircle(renderer, gap + *r, gap2 + *r, *r);
        drawVertexNumber(renderer, key, gap + *r, gap2 + *r, gap, color);
        list_ptr = l_list_init(key, gap + *r, gap2 + *r, pos, 0);
        key++;

        while (key <= size) {
            switch (turn % 4) {
                case 1:
                    for (int j = 1; j < xTopCircles; ++j) {
                        drawCircle(renderer, gap + *r + j * (2 * *r + gap1),
                                   gap2 + *r, *r);
                        drawVertexNumber(renderer, key, gap + *r + j * (2 * *r + gap1),
                                         gap2 + *r, gap, color);
                        list_ptr = addto_list(list_ptr, key,
                                              gap + *r + j * (2 * *r + gap1), gap2 + *r,
                                              pos, 0);
                        key++;
                    }
                    break;
                case 2:
                    for (int j = 0; j < yCircles / 2; ++j) {
                        drawCircle(renderer, width - gap - *r,
                                   (gap2 + *r) + j * (gap2 + 2 * *r), *r);
                        drawVertexNumber(renderer, key, width - gap - *r,
                                         (gap2 + *r) + j * (gap2 + 2 * *r), gap, color);
                        list_ptr = addto_list(list_ptr, key,
                                              width - gap - *r, (gap2 + *r) + j * (gap2 + 2 * *r),
                                              pos, 0);
                        key++;
                    }
                    break;
                case 3:
                    for (int j = 0; j < xLowCircles; ++j) {
                        drawCircle(renderer, width - gap - *r - j * (2 * *r + gap3),
                                   height - gap2 - *r, *r);
                        drawVertexNumber(renderer, key, width - gap - *r - j * (2 * *r + gap3),
                                         height - gap2 - *r, gap, color);
                        list_ptr = addto_list(list_ptr, key,
                                              width - gap - *r - j * (2 * *r + gap3), height - gap2 - *r,
                                              pos, 0);
                        key++;
                    }
                    break;
                case 0:
                    for (int j = 0; j < yCircles / 2; ++j) {
                        drawCircle(renderer, gap + *r,
                                   height - (*r + gap2) - j * (2 * *r + gap2), *r);
                        drawVertexNumber(renderer, key, gap + *r,
                                         height - (*r + gap2) - j * (2 * *r + gap2), gap, color);
                        list_ptr = addto_list(list_ptr, key,
                                              gap + *r, height - (*r + gap2) - j * (2 * *r + gap2),
                                              pos, 0);
                        key++;
                    }
                    break;
            }
            turn++;
            pos++;
        }
    }

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (matrix.matrix[i][j] == 1) {
                l_list *node1 = find_num(list_ptr, i + 1);
                l_list *node2 = find_num(list_ptr, j + 1);
                if (i == j) {
                    if (node1->pos >= 3) flag = -1;
                    drawCircle(renderer,
                               (int) (node1->x - flag * (*r + (double) *r / 2 - flag)
                                                 * cos(M_PI / 4)),
                               (int) (node1->y - flag * (*r + (double) *r / 2 - flag)
                                                 * sin(M_PI / 4)),
                               *r / 2);
                    drawArrowHead(renderer, (int) (node1->x - flag * *r * cos(M_PI / 4)),
                                  (int) (node1->y - flag * *r * sin(M_PI / 4)), gap,
                                  3 * M_PI / 4 - flag * M_PI / 15, color);
                } else {
                    dir = 1;
                    drawDirConnections(renderer, node1, node2, *r, dir, width, height, size, color);
                    if (matrix.matrix[j][i] == 1) {
                        matrix.matrix[j][i] = 0;
                        dir = -1;
                        drawDirConnections(renderer, node2, node1, *r, dir, width, height, size, color);
                    }
                }
            }
    return list_ptr;
}

void drawDirConnections(SDL_Renderer *renderer, l_list *node1, l_list *node2,
                        int r, int dir, int width, int height, int size, SDL_Color color) {
    int startX = node1->x, startY = node1->y, endX = node2->x, endY = node2->y;
    int mid1X, mid1Y, mid2X, mid2Y, mid, yCircles;
    int midX = (node1->x + node2->x) / 2;
    int midY = (node1->y + node2->y) / 2;
    int dirX = (node1->x - width / 2) <= 0 ? -1 : 1;
    int dirY = (node1->y - height / 2) <= 0 ? -1 : 1;

    if (size >= 4) {
        mid = (abs(size - 5)) / 4;
        yCircles = 2 + mid * 2;
    }

    int gap = sizeMult * r;
    int gap2 = (size < 4) ? (height - 2 * r) / 2 :
               (height - (yCircles / 2 + 1) * 2 * r) / (yCircles / 2 + 2);
    double angle1, angle2;
    if ((abs(node1->key - node2->key) == 1) ||
        (((node1->key == 1 || node2->key == 1) && (node1->key == size || node2->key == size)) && size >= 4)) {
        angle1 = atan2(endY - startY, endX - startX);
        SDL_RenderDrawLine(renderer, startX + (int) ((double) r * cos(angle1 + shiftAngle)),
                           startY + (int) ((double) r * sin(angle1 + shiftAngle)),
                           endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                           endY - (int) ((double) r * sin(angle1 - shiftAngle)));
        drawArrowHead(renderer, endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                      endY - (int) ((double) r * sin(angle1 - shiftAngle)), gap, angle1, color);
    } else {
        if (startX == endX && (startX == gap + r || startX == width - gap - r)) {
            mid1X = (int) ((double) (startX + midX) / 2 + gap * dir * dirX);
            mid1Y = (startY + midY) / 2;
            mid2X = (int) ((double) (midX + endX) / 2 + gap * dir * dirX);
            mid2Y = (midY + endY) / 2;
            angle1 = atan2(mid1Y - startY, mid1X - startX);
            angle2 = atan2(endY - mid2Y, endX - mid2X);
            drawBezierCurve(renderer, startX + (int) ((double) r * cos(angle1)), mid1X,
                            mid2X, endX - (int) ((double) r * cos(angle2)),
                            startY + (int) ((double) r * sin(angle1)), mid1Y,
                            mid2Y, endY - (int) ((double) r * sin(angle2)));
            drawArrowHead(renderer, endX - (int) ((double) r * cos(angle2)),
                          endY - (int) ((double) r * sin(angle2)), gap, angle2, color);
        } else if (startY == endY && (startY == gap2 + r || startY == height - gap2 - r)) {
            mid1X = (startX + midX) / 2;
            mid1Y = (int) ((double) (startY + midY) / 2 + gap * dir * dirY);
            mid2X = (midX + endX) / 2;
            mid2Y = (int) ((double) (midY + endY) / 2 + gap * dir * dirY);
            angle1 = atan2(mid1Y - startY, mid1X - startX);
            angle2 = atan2(endY - mid2Y, endX - mid2X);
            drawBezierCurve(renderer, startX + (int) ((double) r * cos(angle1)), mid1X,
                            mid2X, endX - (int) ((double) r * cos(angle2)),
                            startY + (int) ((double) r * sin(angle1)), mid1Y,
                            mid2Y, endY - (int) ((double) r * sin(angle2)));
            drawArrowHead(renderer, endX - (int) ((double) r * cos(angle2)),
                          endY - (int) ((double) r * sin(angle2)), gap, angle2, color);
        } else {
            angle1 = atan2(endY - startY, endX - startX);
            SDL_RenderDrawLine(renderer, startX + (int) ((double) r * cos(angle1 + shiftAngle)),
                               startY + (int) ((double) r * sin(angle1 + shiftAngle)),
                               endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                               endY - (int) ((double) r * sin(angle1 - shiftAngle)));
            drawArrowHead(renderer, endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                          endY - (int) ((double) r * sin(angle1 - shiftAngle)), gap, angle1, color);
        }
    }
}

l_list *reDraw(SDL_Renderer *renderer, SDL_Window *window, matrix graphMatrix, l_list *list_ptr, int *r) {
    clearScreen(renderer);
    list_ptr = drawGraph(renderer, window,
                         graphMatrix, list_ptr, N, r);
    SDL_RenderPresent(renderer);
    return list_ptr;
}

queue q_init() {
    queue q = {
            .queue = {-1},
            .rear = -1,
            .front = 0,
    };
    for (int i = 1; i < N; ++i)
        q.queue[i] = -1;
    return q;
}

queue enqueue(int vertex, queue queue) {
    queue.queue[++queue.rear] = vertex;
    return queue;
}

queue dequeue(queue queue, int *vertex) {
    *vertex = queue.queue[queue.front];
    ++queue.front;
    return queue;
}

queue BFS(matrix graphMatrix, l_list *list_ptr, SDL_Renderer *renderer, SDL_Window *window,
          int r, queue queue, int *flag, int startKey, FILE *fptr, int *isCurrent, matrix *treeMatrix) {
    if (queue.front == END_CODE) {
        fclose(fptr);
        return queue;
    }

    int currentVertex, width, height;
    int gap = r * sizeMult;

    SDL_GetWindowSize(window, &width, &height);

    l_list *new_node = find_num(list_ptr, startKey);
    l_list *this_node;

    fptr = fopen("Output.txt", "a");

    SDL_Color color = {0, 255, 0, 255};
    SDL_Color color1 = {255, 0, 0, 255};
    SDL_Color color2 = {0, 0, 255, 255};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    if (queue.rear == -1) {
        queue = enqueue(new_node->key - 1, queue);
        new_node->state = 1;

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        drawCircle(renderer, new_node->x, new_node->y, r);
        drawVertexNumber(renderer, new_node->key, new_node->x, new_node->y, gap, color1);
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        fprintf(fptr, "%4d", new_node->key);
        fclose(fptr);
        return queue;
    }

    while (queue.front <= queue.rear) {
        if (*flag == 1) {
            if (*isCurrent == 1) {
                queue = dequeue(queue, &currentVertex);
                --queue.front;

                if (currentVertex + 1 != startKey) {
                    drawCircle(renderer, this_node->x, this_node->y, r);
                    drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color);
                    SDL_RenderPresent(renderer);
                }

                this_node = find_num(list_ptr, currentVertex + 1);

                if (this_node->key == currentVertex + 1) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    drawCircle(renderer, this_node->x, this_node->y, r);
                    drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color1);
                    SDL_RenderPresent(renderer);
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                }

                *isCurrent *= -1;
                fclose(fptr);
                return queue;
            } else {
                queue = dequeue(queue, &currentVertex);

                if (currentVertex + 1 > startKey) {
                    drawCircle(renderer, this_node->x, this_node->y, r);
                    drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color);
                    SDL_RenderPresent(renderer);
                }

                this_node = find_num(list_ptr, currentVertex + 1);

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                drawCircle(renderer, this_node->x, this_node->y, r);
                drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color1);
                SDL_RenderPresent(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

                *isCurrent *= -1;
            }
        }
        *flag = 0;

        for (int i = 0; i < N; ++i) {
            new_node = find_num(list_ptr, i + 1);

            if (i == N - 1)
                *flag = 1;

            if (graphMatrix.matrix[currentVertex][i] && new_node->state == 0) {
                new_node->state = 1;
                drawCircle(renderer, new_node->x, new_node->y, r);
                drawVertexNumber(renderer, new_node->key, new_node->x, new_node->y, gap, color);
                drawDirConnections(renderer, this_node, new_node, r,1,
                                   width, height, N, color);
                treeMatrix->matrix[this_node->key - 1][new_node->key - 1] = 1;
                SDL_RenderPresent(renderer);

                fprintf(fptr, "%4d", new_node->key);
                queue = enqueue(i, queue);
                fclose(fptr);
                return queue;
            }
        }
        drawCircle(renderer, this_node->x, this_node->y, r);
        drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color);
        SDL_RenderPresent(renderer);
    }
    queue.front = END_CODE;
    fclose(fptr);
    return queue;
}

stack stack_init() {
    stack stack = {
            .stack = {-1},
            .top = 0,
            .prevVertex = -1,
    };

    for (int i = 1; i < N * N; ++i)
        stack.stack[i] = -1;
    return stack;
}

stack DFS(matrix graphMatrix, l_list *list_ptr, SDL_Renderer *renderer, SDL_Window *window,
          int r, int startKey, int *place, FILE *fptr, matrix *treeMatrix, array *array, stack stack) {

    if (stack.top == END_CODE) {
        fclose(fptr);
        return stack;
    }

    int width, height;
    l_list *this_node = find_num(list_ptr, startKey), *new_node, *prev_node;
    int gap = r * sizeMult;

    SDL_GetWindowSize(window, &width, &height);

    fptr = fopen("Output2.txt", "a");
    SDL_Color color = {0, 255, 0, 255};
    SDL_Color color1 = {255, 0, 0, 255};
    SDL_Color color2 = {0, 0, 255, 255};

    if (stack.top == 0) {
        stack.stack[stack.top++] = startKey;
        stack.prevVertex = startKey;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        drawCircle(renderer, this_node->x, this_node->y, r);
        drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color1);
        SDL_RenderPresent(renderer);
        fclose(fptr);
        return stack;
    }

    while (stack.top > 0) {
        this_node = find_num(list_ptr, stack.stack[--stack.top]);
        prev_node = find_num(list_ptr, stack.prevVertex);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        drawCircle(renderer, this_node->x, this_node->y, r);
        drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color1);
        SDL_RenderPresent(renderer);

        if (this_node->state == 0) {
            if (*place == 1) {
                *place *= -1;
                stack.top++;
                fclose(fptr);
                return stack;
            }

            this_node->state = 1;

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            drawCircle(renderer, this_node->x, this_node->y, r);
            drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color);
            SDL_RenderPresent(renderer);

            if (this_node->key != startKey) {
                drawDirConnections(renderer, prev_node, this_node, r,
                                   (this_node->key > prev_node->key) ? 1 : -1,
                                   width, height, N, color);
                treeMatrix->matrix[prev_node->key - 1][this_node->key - 1] = 1;
                SDL_RenderPresent(renderer);
            }

            for (int i = 0; i < N; i++) {
                new_node = find_num(list_ptr, i + 1);
                if (graphMatrix.matrix[this_node->key - 1][i] && new_node->state == 0) {
                    stack.stack[stack.top++] = i + 1;
                    stack.prevVertex = stack.stack[stack.top - 1];
                }
            }

            fprintf(fptr, "%4d", this_node->key);
            array->array[array->place++] = this_node->key;
            fclose(fptr);
            stack.prevVertex = this_node->key;
            *place *= -1;
            return stack;
        } else {
            if (array->place == N && stack.top == END_CODE)
                this_node = find_num(list_ptr, array->array[array->place - 1]);

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            drawCircle(renderer, this_node->x, this_node->y, r);
            drawVertexNumber(renderer, this_node->key, this_node->x, this_node->y, gap, color);
            SDL_RenderPresent(renderer);
        }
    }

    fclose(fptr);
    stack.top = END_CODE;
    return stack;

}