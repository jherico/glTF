#include "common.hpp"
#include "easings.hpp"
#include "camera.hpp"
#include "json.hpp"

namespace glfw {
    class Window {
    private:
        GLFWwindow* _window{ nullptr };
        glm::uvec2 _size{ 1280, 720 };
        uint32_t _frameCounter{ 0 };
        static std::once_flag init;

    public:
        Window() {
            std::call_once(init, [&] {
                glfwInit();
            });
        }

        virtual ~Window() {
            if (nullptr != _window) {
                glfwDestroyWindow(_window);
            }
            // FIXME make static destroyer predicated on initialization
            glfwTerminate();
        }

        float aspect() const {
            return (float)_size.x / (float)_size.y;
        }

        uint32_t frameCount() {
            return _frameCounter;
        }

        const glm::uvec2& size() const {
            return _size;
        }

        virtual void createHints() const {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_DEPTH_BITS, 16);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        }

        virtual void createCallbacks() const {
            glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
            glfwSetMouseButtonCallback(_window, mouseButtonCallback);
            glfwSetCursorPosCallback(_window, cursorPosCallback);
        }

        virtual void createWindow(const glm::uvec2& size = glm::uvec2()) {
            if (size != glm::uvec2()) {
                _size = size;
            }
            if (_window) {
                throw std::runtime_error("Window already exists");
            }
            createHints();
            _window = glfwCreateWindow(_size.x, _size.y, "glfw", nullptr, nullptr);
            if (!_window) {
                throw std::runtime_error("Unable to create rendering window");
            }
            glfwSetWindowUserPointer(_window, this);
            createCallbacks();
        }

        void makeCurrent() const {
            glfwMakeContextCurrent(_window);
        }

        void swapBuffers() {
            glfwSwapBuffers(_window);
            ++_frameCounter;
        }

        bool shouldClose() {
            return 0 != glfwWindowShouldClose(_window);
        }

        static void pollEvents() {
            glfwPollEvents();
        }

        virtual void onFramebufferSize(const glm::uvec2& newSize) {
        }

        virtual void onMouseButton(const glm::uvec3& buttonInfo) {
        }

        virtual void onMouseMove(const glm::vec2& cursorPos) {
        }

        static void framebufferSizeCallback(GLFWwindow* glfwWindow,int width,int height) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
            window->onFramebufferSize({ width, height });
        }

        static void mouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
            window->onMouseButton({ button, action, mods });
        }

        static void cursorPosCallback(GLFWwindow* glfwWindow, double x, double y) {
            Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
            window->onMouseMove({ x, y });
        }
    }; // class Window


} // namespace glfw

std::once_flag glfw::Window::init;

class SimpleExample : public glfw::Window {
public:
    Camera camera;
    const float duration = 4.0f;
    const float interval = 6.0f;
    bool paused{ false };
    glm::quat orientation;

    SimpleExample() {
        createWindow();
        makeCurrent();
        //glfwSwapInterval(0);
        glewExperimental = true;
        glewInit();
        glGetError();
        camera.setPerspective(60, aspect());
    }

    void render() {
        glFlush();
        glClearColor(0, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        swapBuffers();
    }

    void prepare() {
    }

    void update(float deltaTime) {
    }

    void runWindow() {
        prepare();
        auto tStart = std::chrono::high_resolution_clock::now();
        while (!shouldClose()) {
            pollEvents();

            auto tEnd = std::chrono::high_resolution_clock::now();
            auto tDiff = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
            tStart = tEnd;
            update(tDiff / 1000.0f);

            auto projection = camera.matrices.perspective;
            auto view = camera.matrices.view;
            render();
        }
    }

    void runParser() {
        std::string binFile { PROJECT_ROOT "/data/gltf/2.0/Triangle/glTF/simpleTriangle.bin"};
        std::string gltfFile { PROJECT_ROOT "/data/gltf/2.0/Triangle/glTF/Triangle.gltf"};
        using json = nlohmann::json;

        json gltfData; 
        {
            std::ifstream infile{ gltfFile };
            infile >> gltfData;
        }

    }

    void run() {
        runParser();
    }

    std::string getWindowTitle() {
        //return "OpenGL Interop - " + std::to_string(frameCounter) + " fps";
    }
};

RUN_EXAMPLE(SimpleExample)
