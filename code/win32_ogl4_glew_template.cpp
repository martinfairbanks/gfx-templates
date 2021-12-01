// Creates an OpenGL core profile graphics context and using glew to load OpenGL library functions

// linking
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "glew32s.lib")

#define WIN32_LEAN_AND_MEAN
#define GLEW_STATIC
#pragma warning(push, 3)
    #include <windows.h>	
    #include <gl/glew.h>		
    #include <gl/wglew.h>
#pragma warning(pop)

static int windowWidth = 960;
static int windowHeight = 540;
static bool running = true;

void quitError(char *caption, char *msg)
{
	char buffer[256];
	wsprintf(buffer, "%s", msg);
	MessageBoxA(0, buffer, caption, MB_ICONERROR);
	ExitProcess(1);
}

// shaders
char *vertexShaderSource = "#version 460 core\n"
	"layout (location = 0) in vec3 position;\n"
	"void main()\n"
	"{\n"
	"gl_Position = vec4(position, 1.0);\n"
	"}\0";

char *fragmentShaderSource = "#version 460 core\n"
	"layout(location = 0) out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"	outputColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int vCode = (int)wParam;

	switch (msg)
	{	
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		} break;

		case WM_KEYDOWN:
		{
			switch (vCode)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
				} break;
			}
		} break;

		case WM_SIZE:
		{
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
			glViewport(0, 0, windowWidth, windowHeight);
		} break;

		default:
		{
			// let windows take care of any unhandled messages
			return DefWindowProc(window, msg, wParam, lParam);
		} break;
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	// create temporary window to load extensions
	// we have to do this cause you could only set the pixel format once
	// and it is needed to be set again with wglChoosePixelFormatARB to get a GL core profile context

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_OWNDC, WindowProc, 0, 0,
		GetModuleHandle(0), NULL, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, "OpenGL4", NULL };

	if (RegisterClassEx(&wc) == 0)
	{
		quitError("Fatal error", "Failed to initialize window class!");
	}

	HWND tempWindow = CreateWindowExA(0, "OpenGL4", "TempWindow",
		0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, GetModuleHandle(0), NULL);

	if (!tempWindow)
	{
		quitError("Fatal error", "Failed to create window!");
	}

	HDC dc = GetDC(tempWindow);

	PIXELFORMATDESCRIPTOR tempPFD = {0};
	tempPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	tempPFD.nVersion = 1;
	tempPFD.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	tempPFD.iPixelType = PFD_TYPE_RGBA;
	tempPFD.cColorBits = 32;
	tempPFD.cAlphaBits = 8;
	tempPFD.cDepthBits = 24;
	tempPFD.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(dc, &tempPFD);

	SetPixelFormat(dc, pixelFormat, &tempPFD);

	// create a temprary gl context
	HGLRC tempRC = wglCreateContext(dc);
	wglMakeCurrent(dc, tempRC);

	// activate the specified OpenGL rendering context to make it the current rendering context
	if (wglMakeCurrent(dc, tempRC) == 0)
	{
		// NOTE: the program won't crash if the other OpenGL calls would fail so we can
		// safely catch the error here
		quitError("OpenGL Error", "Failed to create OpenGL rendering context!");
	}
	
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		quitError("GLEW Error", "Failed to initialize GLEW!");
	}

	// delete temporary window and OpenGL context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	ReleaseDC(tempWindow, dc);
	DestroyWindow(tempWindow);

	// create a new window and context
	HWND window = CreateWindowExA(0, "OpenGL4",
		"OpenGL4", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, 0, 0, 0, 0);

	dc = GetDC(window);

	const int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
	};

	int pixelFormatID;
	UINT numFormats;
	
	if(!wglChoosePixelFormatARB(dc, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats) || numFormats == 0)
	{
		quitError("OpenGL Error", "wglChoosePixelFormatARB failed. OpenGL pixel format not supoorted!");
	}

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(dc, pixelFormatID, sizeof(pfd), &pfd);
	if (!SetPixelFormat(dc, pixelFormatID, &pfd))
	{
		quitError("OpenGL Error", "Failed to set pixel format!");
	}

	int contextAttribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
		0
	};

	HGLRC rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
	if (rc == NULL)
	{
		quitError("OpenGL Error", "Failed to create OpenGL context!");		
	}

	if (!wglMakeCurrent(dc, rc))
	{
		quitError("OpenGL Error", "Failed to make current OpenGL context!");
	}
    
    bool vsync = true;
    wglSwapIntervalEXT(vsync ? 1 : 0);
    
	SetWindowText(window, (LPCSTR)glGetString(GL_VERSION));
	ShowWindow(window, cmdShow);

	// build and compile shader program

	// vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	// check for compile time errors
	GLint success;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		quitError("Vertex shader error", errorLog);
	}

	// fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		quitError("Fragment shader error", errorLog);
	}

	// link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, errorLog);
		quitError("Shader link error", errorLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data
	GLfloat vertices[] =
	{
		-0.5f, -0.5f, 0.0f, // Left  
		 0.5f, -0.5f, 0.0f, // Right 
		 0.0f, 0.5f, 0.0f	// Top   
	};

	GLuint vbo, vao;
	// generate a Vertex Array Object
	glGenVertexArrays(1, &vao);

	// generate a Vertex Buffer Object
	glGenBuffers(1, &vbo);
	
	// bind the vao
	glBindVertexArray(vao);

	// bind the vbo to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// upload the vertex data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute pointer, tells opengl how the data buffer is structured
	// start at 0 using 3 floats
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	 // unbind vao
	glBindVertexArray(0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	MSG msg = {};
	while (running)
	{
		while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				running = false;

			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		SwapBuffers(dc);
	}

	return 0;
}
