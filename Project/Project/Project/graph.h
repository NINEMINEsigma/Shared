#pragma once

#ifndef __GRAPH

#define __GRAPH

#include"mastgl.h"
#include"file.h"

#define __H__ 600
#define __W__ 900

class tick_clock
{
public:
    tick_clock() : start_(), end_() {  }
    void Start()
    {
        QueryPerformanceCounter(&start_);
    }
    void CountEnd()
    {
        QueryPerformanceCounter(&end_);
    }
    double GetElapsedMilliseconds()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (end_.QuadPart - start_.QuadPart) * 1000.0 / (double)freq.QuadPart;
    }
    double GetElapsedSeconds()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (end_.QuadPart - start_.QuadPart) * 1.0 / (double)freq.QuadPart;
    }

    double GetElapsedXXXSeconds(double xxx)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (end_.QuadPart - start_.QuadPart) * xxx / (double)freq.QuadPart;
    }

private:
    LARGE_INTEGER start_;
    LARGE_INTEGER end_;
};

namespace graph
{
    void OneFrameStart()
    {
        //glClearColor(0.3f, 0.6f, 0.9f,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_STENCIL_BUFFER_BIT);
    }

    void OneFrameEnd(void(*display)(int), int value)
    {
        glutSwapBuffers();
        glutTimerFunc(1, display, value + 1);
    }

    struct window_vec3
    {
        window_vec3(GLfloat _x = 0, GLfloat _y = 0, GLfloat _z = 0)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        GLfloat x = 0, y = 0, z = 0;
    };

    class window_builder
    {
    private:
        int find_index = -1;
    public:
        window_builder()
        {
            __Timer.Start();
            if (instance_main() == nullptr)set_main();
        }
        window_builder(int argc_, char** argv_)
        {
            __Timer.Start();
            if (instance_main() == nullptr)set_main();
            argc = argc_;
            argv = argv_;
        }
        ~window_builder() {}
        void set_main()
        {
            instance_main() = this;
        }

        int get_window_index()
        {
            return find_index;
        }

        void operator()()
        {
            glutInit(&argc, argv);
            glutInitDisplayMode(DisplayMode);
            glutInitWindowPosition((int)Pos.x, (int)Pos.y);
            glutInitWindowSize((int)Size.x, (int)Size.y);
            find_index = glutCreateWindow(Winname.c_str());

            if (__ReshapeFunc != nullptr) glutReshapeFunc(__ReshapeFunc);
            if (__KeyboardFunc != nullptr) glutKeyboardFunc(__KeyboardFunc);
            if (__MouseFunc != nullptr) glutMouseFunc(__MouseFunc);
            if (__SpecialFunc != nullptr) glutSpecialFunc(__SpecialFunc);
            if (__MotionFunc != nullptr) glutMotionFunc(__MotionFunc);

            glShadeModel(GL_SMOOTH);
            if (__Display != nullptr)glutDisplayFunc(__Display);
            if (__TimerFunc != nullptr)glutTimerFunc(time, __TimerFunc, value);
            glutMainLoop();//进行消息循环
            /*
            1.glutCreateWindow 创建顶层窗口
            2.glutCreateSubWindow 创建子窗口
            3.glutSetWindowTitle 设置当前顶层窗口的窗口标题
            4.glutPositionWindow 申请改变当前窗口的位置
            5.glutPostRedisplay 标记当前窗口需要重新绘制
            6.glutSwapbuffers 交换当前窗口的缓存
            */
        }

        void AddListener_ReshapeFunc(void(*callback)(int, int))
        {
            __ReshapeFunc = callback;
        }
        void AddListener_KeyboardFunc(void(*callback)(unsigned char, int, int))
        {
            __KeyboardFunc = callback;
        }
        void AddListener_SpecialFunc(void(*callback)(int, int, int))
        {
            __SpecialFunc = callback;
        }
        void AddListener_TimerFunc(void(*callback)(int))
        {
            __TimerFunc = callback;
        }
        void AddListener_Display(void(*callback)())
        {
            __Display = callback;
        }
        void AddListener_MouseFunc(void(*callback)(int button, int state, int x, int y))
        {
            __MouseFunc = callback;
        }
        void AddListener_MotionFunc(void(*callback)(int x, int y))
        {
            __MotionFunc = callback;
        }
        //true设置逆时针为正面
        void SetCounterClockWiseFrontFace(bool b)
        {
            if (b)glFrontFace(GL_CCW);
            else glFrontFace(GL_CW);
        }

        double GetCountEnd(double xxx = 1.0)
        {
            __Timer.CountEnd();
            return __Timer.GetElapsedXXXSeconds(xxx);
        }
        HMODULE GetWin()
        {
            return GetModuleHandle(NULL);
        }

        int argc = 0;
        char** argv = nullptr;
        window_vec3 Pos = window_vec3(600, 350), Size = window_vec3(__W__, __H__), CurrentSize = window_vec3(__W__, __H__);
        std::string Winname = "new opengl";
        unsigned int DisplayMode = GLUT_RGBA | GLUT_DOUBLE;
        unsigned int time = 1;
        int value = 0;

        static window_builder*& instance_main();

        tick_clock __Timer = tick_clock();
    private:
        void(*__ReshapeFunc)(int, int) = nullptr;
        void(*__KeyboardFunc)(unsigned char, int, int) = nullptr;
        void(*__MouseFunc)(int, int, int, int) = nullptr;
        void(*__SpecialFunc)(int, int, int) = nullptr;
        void(*__MotionFunc)(int, int) = nullptr;
        void(*__TimerFunc)(int) = nullptr;
        void(*__Display)() = nullptr;
    };

    window_builder*& window_builder::instance_main()
    {
        static window_builder* instance = nullptr;
        return instance;
    }

    void ReRectWindow(window_builder* instance)
    {
        HWND gameh = ::FindWindow(NULL, string2wstring(instance->Winname).c_str());
        RECT r1;
        ::GetClientRect(gameh, &r1);
        instance->CurrentSize.x = (float)r1.right;
        instance->CurrentSize.y = (float)r1.bottom;
        glViewport(0, 0, (GLsizei)instance->CurrentSize.x, (GLsizei)instance->CurrentSize.y);
    }
}

#endif // !__GRAPH
