#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <random>
#include <algorithm>

struct Door {
    std::string name;
    bool isGood;
    float x;
    float width;
    GLuint textureID;
};
GLuint loadTexture(const char* filePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}

// Game variables
int currentLevel = 0;
int lives = 3;
const int totalLevels = 4;
//std::vector<Door> GoodDoors = {
//    {"Good_Door_1", true, -0.8f, 0.5f, 0},
//    {"Good_Door_2", true, 0.0f, 0.5f, 0},
//    {"Good_Door_3", true, 0.8f, 0.5f, 0},
//    {"Good_Door_4", true, -0.8f, 0.5f, 0},
//    {"Good_Door_5", true, 0.0f, 0.5f, 0},
//    {"Good_Door_6", true, 0.8f, 0.5f, 0}
//};
//std::vector<Door> EasyBadDoors = {
//    {"Bad_Door_2.1", false, -0.8f, 0.5f, 0},
//    {"Bad_Door_3", false, 0.0f, 0.5f, 0},
//    {"Bad_Door_4", false, 0.8f, 0.5f, 0},
//    {"Bad_Door_6", false, 0.8f, 0.5f, 0}
//};
//std::vector<Door> HardBadDoors = {
//    {"Bad_Door_2", false, -0.8f, 0.5f, 0},
//    {"Bad_Door_5", false, 0.0f, 0.5f, 0}
//};
std::vector<std::vector<Door>> levels = {
    {{"Good_Door_1", true, -1.0f, 0.5f, 0}, {"Bad_Door_6", false, 0.0f, 0.5f, 0}, {"Bad_Door_2", false, 1.0f, 0.5f, 0}}, // Level 1
    {{"Bad_Door_3", false, -1.0f, 0.5f, 0}, {"Bad_Door_2.1", false, 0.0f, 0.5f, 0}, {"Good_Door_4", true, 1.0f, 0.5f, 0}}, // Level 2
    {{"Good_Door_5", true, -1.0f, 0.5f, 0}, {"Bad_Door_1", false, 0.0f, 0.5f, 0}, {"Bad_Door_4", false, 1.0f, 0.5f, 0}},// Level 3
    {{"Bad_Door_3", false, -1.0f, 0.5f, 0}, {"Good_Door_2", true, 0.0f, 0.5f, 0}, {"Bad_Door_5", false, 1.0f, 0.5f, 0}}  // Level 4
};

// Load textures for all doors
void loadTextures() {
    for (auto& level : levels) {
        for (auto& door : level) {
            std::string filePath = "img/" + door.name + ".png"; // Use img folder
            door.textureID = loadTexture(filePath.c_str());
        }
    }
}

// Render a door with a texture
void renderDoor(const Door& door) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, door.textureID);

    float x = door.x;
    float width = door.width;
    float doorHeight = 1.5f;  // Adjust door height to fit

    // Adjust coordinates for rendering to fix upside down and scaling
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x - width / 1.5f, -doorHeight / 2.5f); // Bottom-left
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width / 1.5f, -doorHeight / 2.5f); // Bottom-right
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width / 1.5f, doorHeight / 2.5f);  // Top-right
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x - width / 1.5f, doorHeight / 2.5f);  // Top-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Display doors for the current level
    for (const Door& door : levels[currentLevel]) {
        renderDoor(door);
    }

    glutSwapBuffers(); // Swap buffers to display the rendered image
    glFlush(); // Ensure rendering is completed
}

// Mouse input callback function
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Get window dimensions
        float windowWidth = (float)glutGet(GLUT_WINDOW_WIDTH);
        float windowHeight = (float)glutGet(GLUT_WINDOW_HEIGHT);

        // Calculate the normalized coordinates, considering aspect ratio
        float aspectRatio = windowWidth / windowHeight;

        // Normalize the X and Y positions from window space to normalized device coordinates (-1 to 1)
        float normX = (2.0f * x / windowWidth - 1.0f) * aspectRatio;
        float normY = 1.0f - 2.0f * y / windowHeight;
      /*  std::cout << "Mouse Click - NormX: " << normX << ", NormY: " << normY << std::endl;*/

        // Iterate through the doors in the current level
        for (size_t i = 0; i < levels[currentLevel].size(); ++i) {
            Door& door = levels[currentLevel][i];

            // Calculate the door's bounds in NDC
            float doorLeft = door.x - door.width / 1.5f;
            float doorRight = door.x + door.width / 1.5f;
            float doorBottom = -1.5f / 2.5f; // Adjust based on doorHeight in rendering
            float doorTop = 1.5f / 2.5f;     // doorHeight is 1.5f in rendering
          /*  std::cout << "Door Bounds - Left: " << doorLeft << ", Right: " << doorRight
                << ", Bottom: " << doorBottom << ", Top: " << doorTop << std::endl;*/

            // Check if the mouse click is within the door's bounds
            if (normX >= doorLeft && normX <= doorRight && normY >= doorBottom && normY <= doorTop) {
                std::cout << "Clicked on door " << i + 1 << std::endl;
                if (door.isGood) {
                    std::cout << "Correct choice! Proceeding to the next level.\n";
                    currentLevel++;
                    if (currentLevel >= totalLevels) {
                        std::cout << "Congratulations! You completed all levels.\n";
                        currentLevel = 0;
                    }
                }
                else {
                    lives--;
                    std::cout << "Wrong choice! You have " << lives << " lives left.\n";
                    if (lives <= 0) {
                        std::cout << "Game Over. Better luck next time!\n";
                        lives = 3;
                        currentLevel = 0;
                    }
                }
                glutPostRedisplay(); // Redraw the scene after an action
                break;
            }
        }
    }
}

// Initialize OpenGL settings
void initialize() {
    GLenum err = glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glEnable(GL_TEXTURE_2D); // Enable texturing
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending mode for transparency
}

// Set up orthogonal projection for 2D rendering
void setupOrthogonalProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); // Switch back to modelview
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    // Protect against a divide-by-zero when the window is too short
    if (height == 0) height = 1;

    float aspectRatio = (float)width / (float)height;

    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Maintain aspect ratio
    if (width >= height) {
        gluOrtho2D(-1.0 * aspectRatio, 1.0 * aspectRatio, -1.0, 1.0);
    }
    else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspectRatio, 1.0 / aspectRatio);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffering for smoother animations
    glutInitWindowSize(1100, 600);
    glutCreateWindow("OpenGL Door Game with Textures");
    /*glutFullScreen();*/

    initialize();
    setupOrthogonalProjection();

    loadTextures(); // Load door textures from img folder
    glutReshapeFunc(reshape);
    glutDisplayFunc(display); // Register the display callback
    glutMouseFunc(mouse); // Register the mouse callback
    glutMainLoop(); // Enter the GLUT main loop

    return 0;
}
