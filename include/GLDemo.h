#ifndef GLDEMO_H_
#define GLDEMO_H_
#include <QOpenGLWindow>

class GLDemo
{
    public:
        static void initialize();
        static void render();
        static void destroy();
        ~GLDemo() noexcept;
    private:
        static unsigned int m_shaderProgram, m_VBO, m_VAO;
    protected:
        GLDemo();
};


#endif