#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <cmath>
#include <cstdlib>
#include <stdio.h>

// Circle properties
float x = 0.0f, y = 0.0f;      // Position of the circle
float vx = 0.01f, vy = 0.015f; // Velocity of the circle
const float radius = 0.1f;     // Radius of the circle

// Set up the shader source code
const char *vertexShaderSource = R"(
    attribute vec2 aPosition;
    varying vec2 vPosition;
    uniform vec2 uTranslation;
    void main() {
        gl_Position = vec4(aPosition + uTranslation, 0.0, 1.0);
        vPosition = aPosition;
    }
)";

const char *fragmentShaderSource = R"(
    precision mediump float;
    varying vec2 vPosition;
    void main() {
        float dist = length(vPosition);
        if (dist < 0.1) {
            gl_FragColor = vec4(0.0, 0.5, 1.0, 1.0);  // Circle color
        } else {
            discard;  // Discard pixels outside the circle radius
        }
    }
)";

// OpenGL variables
GLuint program;
GLuint positionBuffer;
GLint positionLocation;

void initOpenGL()
{
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Link shaders into a program
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    // Define the circle's shape as a triangle fan
    const int numSegments = 100;
    float circleVertices[numSegments * 2];
    for (int i = 0; i < numSegments; ++i)
    {
        float angle = i * 2.0f * M_PI / numSegments;
        circleVertices[2 * i] = cosf(angle) * radius;
        circleVertices[2 * i + 1] = sinf(angle) * radius;
    }

    // Create and bind vertex buffer
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

    // Set up position attribute in shader
    positionLocation = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Update circle position and check for boundary collisions
void update()
{
    x += vx;
    y += vy;

    // Bounce off walls
    if (x + radius > 1.0f || x - radius < -1.0f)
        vx = -vx;
    if (y + radius > 1.0f || y - radius < -1.0f)
        vy = -vy;
}

// Render the circle
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Update position and bind shader program
    glUseProgram(program);

    // Apply translation for the circle's position
    GLint translationLocation = glGetUniformLocation(program, "uTranslation");
    glUniform2f(translationLocation, x, y);

    // Draw the circle as a triangle fan
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 100);
}

// Main loop function
void main_loop()
{
    update();
    render();
}

int main()
{
    // Initialize WebGL context on the canvas
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    attr.depth = 1;
    attr.stencil = 0;
    attr.antialias = 1;
    attr.premultipliedAlpha = 0;
    attr.preserveDrawingBuffer = 0;
    attr.enableExtensionsByDefault = 1;
    attr.majorVersion = 2; // Use WebGL 2 if available

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context("#canvas", &attr);
    if (context <= 0)
    {
        printf("Failed to create WebGL context\n");
        return 1;
    }
    emscripten_webgl_make_context_current(context);
    initOpenGL();
    emscripten_set_main_loop(main_loop, 0, true);
    return 0;
}
