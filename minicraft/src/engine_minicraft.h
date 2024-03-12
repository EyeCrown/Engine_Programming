#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine
{
public :
    GLint ShaderCubeDebug;
    GLint ShaderSun;

    YVbo* VboCube;

    YColor SunColor, SkyColor;
    YVec3f SunDirection, SunPosition;


    YCamera Camera;


    SYSTEMTIME local;

    //Gestion singleton
    static YEngine* getInstance()
    {
        if (Instance == NULL)
            Instance = new MEngineMinicraft();
        return Instance;
    }

    /*HANDLERS GENERAUX*/
    void loadShaders()
    {
        //Chargement du shader (dans loadShaders() pour etre lié à F5)
        ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");

        ShaderSun = Renderer->createProgram("shaders/sun");
    }

    void init()
    {
        YLog::log(YLog::ENGINE_INFO, "Minicraft Started : initialisation");

        //Renderer->setBackgroundColor(YColor(0.0f,0.5f,1.0f,1.0f));
        //Renderer->Camera->setPosition(YVec3f(10, 10, 10));
        //Renderer->Camera->setLookAt(YVec3f());

        // Setup camera
        Camera = YCamera();
        Camera.setPosition(YVec3f(10, 10, 10));
        Camera.setLookAt(YVec3f());

        Renderer->Camera = &Camera;

        //Creation du VBO
        VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

        //Définition du contenu du VBO
        VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
        VboCube->setElementDescription(1, YVbo::Element(3)); //Normale
        VboCube->setElementDescription(2, YVbo::Element(2)); //UV

        //On demande d'allouer la mémoire coté CPU
        VboCube->createVboCpu();

        //On ajoute les sommets
        int iVertice = 0;

        YVec3f A = YVec3f(0, 0, 0);
        YVec3f B = YVec3f(1, 0, 0);
        YVec3f C = YVec3f(1, 1, 0);
        YVec3f D = YVec3f(0, 1, 0);
        YVec3f E = YVec3f(0, 0, 1);
        YVec3f F = YVec3f(1, 0, 1);
        YVec3f G = YVec3f(1, 1, 1);
        YVec3f H = YVec3f(0, 1, 1);


        MySetTriangle(*VboCube, 0, iVertice, A, D, C);
        MySetTriangle(*VboCube, 0, iVertice, A, C, B);
        MySetTriangle(*VboCube, 0, iVertice, E, G, H);
        MySetTriangle(*VboCube, 0, iVertice, E, F, G);

        MySetTriangle(*VboCube, 0, iVertice, B, G, F);
        MySetTriangle(*VboCube, 0, iVertice, B, C, G);
        MySetTriangle(*VboCube, 0, iVertice, C, H, G);
        MySetTriangle(*VboCube, 0, iVertice, C, D, H);

        MySetTriangle(*VboCube, 0, iVertice, D, E, H);
        MySetTriangle(*VboCube, 0, iVertice, D, A, E);
        MySetTriangle(*VboCube, 0, iVertice, A, F, E);
        MySetTriangle(*VboCube, 0, iVertice, A, B, F);


        //On envoie le contenu au GPU
        VboCube->createVboGpu();

        //On relache la mémoire CPU
        VboCube->deleteVboCpu();
    }

    void MySetElement(YVbo& vbo, int element, int id, YVec3f vec)
    {
        vbo.setElementValue(element, id, vec.X, vec.Y, vec.Z);
    }

    void MySetTriangle(YVbo& vbo, int element, int& id, YVec3f vec1, YVec3f vec2, YVec3f vec3)
    {
        MySetElement(*VboCube, element, id + 0, vec1);
        MySetElement(*VboCube, element, id + 1, vec2);
        MySetElement(*VboCube, element, id + 2, vec3);
        id += 3;
    }

    void update(float elapsed)
    {
        GetLocalTime(&local);


        Camera.update(elapsed);
    }

    void renderObjects()
    {
        glUseProgram(0);
        //Rendu des axes
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3d(1, 0, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(10000, 0, 0);
        glColor3d(0, 1, 0);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 10000, 0);
        glColor3d(0, 0, 1);
        glVertex3d(0, 0, 0);
        glVertex3d(0, 0, 10000);
        glEnd();


        computeSun();

        glPushMatrix();
        glUseProgram(ShaderSun);

        GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
        glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);

        glRotatef(this->DeltaTimeCumul / 10.0f * 360, -1, -1, 1);

        glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);

        Renderer->setBackgroundColor(SkyColor);

        Renderer->updateMatricesFromOgl(); //Calcule toute les matrices à partir des deux matrices OGL
        Renderer->sendMatricesToShader(ShaderSun); //Envoie les matrices au shader
        VboCube->render(); //Demande le rendu du VBO

        glPopMatrix();
    }

    void resize(int width, int height)
    {
    }

    /*INPUTS*/

    void keyPressed(int key, bool special, bool down, int p1, int p2)
    {
        YLog::log(YLog::ENGINE_INFO,
                  ("Key Pressed > Key " + to_string(key) + " | Special " + to_string(special) + " | Down " +
                      to_string(down) + " | P1 " + to_string(p1) + " | P2 " + to_string(p2)).c_str());
        // < : 100 | ^ : 101 | > : 102 | v : 103 || special = 1
        int const LEFT = 113, UP = 122, RIGHT = 100, DOWN = 115;
        float step = 1.0f;
        
        switch (special)
        {
        case 0:
            switch (key)
            {
            case LEFT:
                Camera.move(Camera.RightVec * step);
                break;
            case UP:
                Camera.move(Camera.UpVec * step);
                break;
            case RIGHT:
                Camera.move(-Camera.RightVec * step);
                break;
            case DOWN:
                Camera.move(-Camera.UpVec * step);
                break;
            }
            break;

        case 1:
            switch (key)
            {
            case 0:
                break;
            case 1:
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
    {
        YLog::log(YLog::ENGINE_INFO,
                  ("Mouse Wheel > Wheel " + to_string(wheel) + " | Dir " + to_string(dir) + " | [" + to_string(x) + ", "
                      + to_string(y) + "]" + " | In Ui " + to_string(inUi)).c_str());

        float step = 1.0f;

        if (dir > 0)
            YLog::log(YLog::ENGINE_INFO, ("Mouse Wheel > Zoom"));
        else
        {
            YLog::log(YLog::ENGINE_INFO, ("Mouse Wheel > Dezoom"));
            step *= -1;
        }


        Camera.move(Camera.Direction * step);
    }

    void mouseClick(int button, int state, int x, int y, bool inUi)
    {
        //YLog::log(YLog::ENGINE_INFO,("Mouse Click > Button " + to_string(button) + " | State " + to_string(state) + " | [" + to_string(x) + ", " + to_string(y) + "]" + " | In Ui " + to_string(inUi)).c_str());
        int RIGHT_CLICK = 2;
        int IS_PRESSED = 0;

        if (button == RIGHT_CLICK && state == IS_PRESSED)
        {
            //sCamera.rotate(5);
        }
    }

    void mouseMove(int x, int y, bool pressed, bool inUi)
    {
        //YLog::log(YLog::ENGINE_INFO,("Mouse Move > [" + to_string(x) + ", " + to_string(y) + "]" + " | Pressed " + to_string(pressed) + " | inUi " + to_string(inUi)).c_str());
    }


    /* Calculation */
    void computeSun()
    {
        // Calculate values
        // Colors
        SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
        SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
        YColor downColor(0.9f, 0.5f, 0.1f, 1);

        SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
        SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));

        // // Position
        // SunDirection.X = cos(fTime);
        // SunDirection.Y = 0.2f;
        // SunDirection.Z = sin(fTime);
        // SunDirection.normalize();

        SunPosition = Renderer->Camera->Position + SunDirection * 500.0f;


        SunPosition = YVec3f(1, 1, 0);
    }
};


#endif
