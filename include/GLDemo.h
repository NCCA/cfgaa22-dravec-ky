#ifndef GLDEMO_H_
#define GLDEMO_H_
#include <QOpenGLWindow>
#include <QElapsedTimer>


class GLDemo
{
//Class for testing out pure OpenGL purely for learning purposes
    public:
        GLDemo() = default;
        void initialize();
        void render(int elapsed_time);
        void destroy();
        void update();
        ~GLDemo() noexcept;
    private:
        //shaderProgram, VertexBufferObject, VertexArrayObject and ElementBufferObject
        unsigned int m_shaderProgram, m_VBO, m_VAO, m_EBO = 0;
        void loadShaders();
        char * readShaderFile(const std::string &_name);
        
};


#endif