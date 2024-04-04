#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine
{
public :
    GLint ShaderCube;
    GLint ShaderSun;
    GLint ShaderWorld;
    GLint ShaderBirds;

    MWorld* World;

    YVbo* VboCube;
    YColor SunColor, SkyColor;
    YVec3f SunDirection, SunPosition;


    YCamera Camera;

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
        ShaderCube = Renderer->createProgram("shaders/cube");
        ShaderSun = Renderer->createProgram("shaders/sun");
    }

    void init()
    {
        YLog::log(YLog::ENGINE_INFO, "Minicraft Started : initialisation");

        Renderer->setBackgroundColor(YColor(0.0f,0.5f,1.0f,1.0f));
        Renderer->Camera->setPosition(YVec3f(10, 10, 10));
        Renderer->Camera->setLookAt(YVec3f());

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

        

        //On envoie le contenu au GPU
        VboCube->createVboGpu();

        //On relache la mémoire CPU
        VboCube->deleteVboCpu();


        
        
    }
    
    void update(float elapsed)
    {
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


        /*computeSun();

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

        glPopMatrix();*/

        // glPushMatrix();
        // glUseProgram(ShaderCube);
        // World->render_world_basic(ShaderCube, VboCube);
        // glPopMatrix();
    }

    void resize(int width, int height)
    {
    }

    /*INPUTS*/

    void keyPressed(int key, bool special, bool down, int p1, int p2)
    {
        //YLog::log(YLog::ENGINE_INFO, ("Key Pressed > Key " + to_string(key) + " | Special " + to_string(special) + " | Down " + to_string(down) + " | P1 " + to_string(p1) + " | P2 " + to_string(p2)).c_str());
        
    }

    void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
    {
        //YLog::log(YLog::ENGINE_INFO,("Mouse Wheel > Wheel " + to_string(wheel) + " | Dir " + to_string(dir) + " | [" + to_string(x) + ", " + to_string(y) + "]" + " | In Ui " + to_string(inUi)).c_str());

    }

    void mouseClick(int button, int state, int x, int y, bool inUi)
    {
        //YLog::log(YLog::ENGINE_INFO,("Mouse Click > Button " + to_string(button) + " | State " + to_string(state) + " | [" + to_string(x) + ", " + to_string(y) + "]" + " | In Ui " + to_string(inUi)).c_str());
        
    }

    void mouseMove(int x, int y, bool pressed, bool inUi)
    {
        //YLog::log(YLog::ENGINE_INFO,("Mouse Move > [" + to_string(x) + ", " + to_string(y) + "]" + " | Pressed " + to_string(pressed) + " | inUi " + to_string(inUi)).c_str());
        static int lastx = -1;
        static int lasty = -1;

        if (!pressed)
        {
            lastx = x;
            lasty = y;
            showMouse(true);
        }
        else
        {
            if (lastx == -1 && lasty == -1)
            {
                lastx = x;
                lasty = y;
            }

            int dx = x - lastx;
            int dy = y - lasty;

            if (dx == 0 && dy == 0)
                return;

            lastx = x;
            lasty = y;
					
            if (MouseBtnState & GUI_MRBUTTON)
            {
                showMouse(false);
                if (GetKeyState(VK_LCONTROL) & 0x80)
                {
                    Camera.rotateAround((float)-dx / 300.0f);
                    Camera.rotateUpAround((float)-dy / 300.0f);
                    glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
                    lastx = Renderer->ScreenWidth / 2;
                    lasty = Renderer->ScreenHeight / 2;
                } else {
                    showMouse(false);
                    Camera.rotate((float)-dx / 300.0f);
                    Camera.rotateUp((float)-dy / 300.0f);
                    glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
                    lastx = Renderer->ScreenWidth / 2;
                    lasty = Renderer->ScreenHeight / 2;
                }	
            }
			
            if (MouseBtnState & GUI_MMBUTTON)
            {
                showMouse(false);
                if (GetKeyState(VK_LCONTROL) & 0x80)
                {
                    YVec3f strafe = Camera.RightVec;
                    strafe.Z = 0;
                    strafe.normalize();
                    strafe *= (float)-dx / 2.0f;

                    YVec3f avance = Camera.Direction;
                    avance.Z = 0;
                    avance.normalize();
                    avance *= (float)dy / 2.0f;

                    Camera.move(avance + strafe);
                } else {
                    YVec3f strafe = Camera.RightVec;
                    strafe.Z = 0;
                    strafe.normalize();
                    strafe *= (float)-dx / 5.0f;

                    Camera.move(Camera.UpRef * (float)dy / 5.0f);
                    Camera.move(strafe);
                    glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
                    lastx = Renderer->ScreenWidth / 2;
                    lasty = Renderer->ScreenHeight / 2;
                }
            }
        }
    }


    /* Sun calculs */

    
};


#endif
