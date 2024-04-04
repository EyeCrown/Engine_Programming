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
    GLint ShaderPostProcess;
    GLint ShaderBirds;

    YFbo* Fbo;

    YTexFile * MainTexture = NULL;
    YTexManager * TexManager = NULL;

    YVbo* SunCube;
    YColor SunColor, SkyColor;
    YVec3f SunDirection, SunPosition;
    float boostTime;

    MWorld* World;
    
    MAvatar* Avatar;

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
        //Chargement du shader (dans loadShaders() pour etre li� � F5)
        ShaderCube = Renderer->createProgram("shaders/cube");
        ShaderSun = Renderer->createProgram("shaders/sun");
        ShaderWorld = Renderer->createProgram("shaders/world");
        ShaderPostProcess = Renderer->createProgram("shaders/postprocess");
        ShaderBirds = Renderer->createProgram("shaders/birds");
    }

    void init()
    {
        YLog::log(YLog::ENGINE_INFO, "Minicraft Started : initialisation");

        
        Fbo = new YFbo(1);
        Fbo->init(Renderer->ScreenWidth, Renderer->ScreenHeight);
        
        Renderer->setBackgroundColor(YColor(0.0f,0.5f,1.0f,1.0f));
        Renderer->Camera->setPosition(YVec3f(10, 10, 10));
        Renderer->Camera->setLookAt(YVec3f());

        Avatar = new MAvatar(Renderer->Camera, World);        

        //Creation du VBO
        SunCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);
        //D�finition du contenu du VBO
        SunCube->setElementDescription(0, YVbo::Element(3)); //Sommet
        SunCube->setElementDescription(1, YVbo::Element(3)); //Normale
        SunCube->setElementDescription(2, YVbo::Element(2)); //UV
        //On demande d'allouer la m�moire cot� CPU
        SunCube->createVboCpu();
        fillVBOCube(SunCube);
        //On envoie le contenu au GPU
        SunCube->createVboGpu();
        //On relache la m�moire CPU
        SunCube->deleteVboCpu();

        // World
        World = new MWorld();
        World->init_world(0);
        
        MainTexture = TexManager->getInstance()->loadTexture("atlas.png");
        TexManager->getInstance()->loadTextureToOgl(*MainTexture);
    }
    
    void update(float elapsed)
    {
        Renderer->Camera->update(elapsed);
        Avatar->update(elapsed);
    }

    void renderObjects()
    {
        Fbo->setAsOutFBO(true);
        
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


        // Sun
        updateLights(boostTime);
        glPushMatrix();
        glUseProgram(ShaderSun);
        GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
        glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);
        glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
        glScalef(10, 10, 10);
        Renderer->updateMatricesFromOgl();
        Renderer->sendMatricesToShader(ShaderSun);
        SunCube->render();
        glPopMatrix();


        // World
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPushMatrix();
        glUseProgram(ShaderWorld);
        Renderer->sendTimeToShader(FpsElapsed, ShaderWorld);
        
        MainTexture->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "myTexture");
        
        World->render_world_vbo(false, true);
        glPopMatrix();



        //Post Process
        Fbo->setAsOutFBO(false);

        glUseProgram(ShaderPostProcess);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
        Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");
        Fbo->setAsOutFBO(false);

        glUseProgram(ShaderPostProcess);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
        Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");

        GLuint var3 = glGetUniformLocation(ShaderPostProcess, "sky_color");
        glUniform3f(var3, SkyColor.R, SkyColor.V, SkyColor.B);

        Renderer->sendScreenSizeToShader(ShaderPostProcess);
        Renderer->sendNearFarToShader(ShaderPostProcess);
        Renderer->drawFullScreenQuad();
    }

    void resize(int width, int height)
    {
        Fbo->resize(width, height);
    }

    /*INPUTS*/

    void keyPressed(int key, bool special, bool down, int p1, int p2)
    {
        YLog::log(YLog::ENGINE_INFO, ("Key Pressed > Key " + to_string(key) + " | Special " + to_string(special) + " | Down " + to_string(down) + " | P1 " + to_string(p1) + " | P2 " + to_string(p2)).c_str());

        if (key == 'z' || key == 'Z')
            Avatar->avance = down;
        if (key == 's' || key == 'S')
            Avatar->recule = down;
        if (key == 'q' || key == 'Q')
            Avatar->gauche = down;
        if (key == 'd' || key == 'D')
            Avatar->droite = down;

        if (key == 'g' || key == 'G')
            boostTime += 10.0f;
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
                    Renderer->Camera->rotateAround((float)-dx / 300.0f);
                    Renderer->Camera->rotateUpAround((float)-dy / 300.0f);
                    glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
                    lastx = Renderer->ScreenWidth / 2;
                    lasty = Renderer->ScreenHeight / 2;
                } else {
                    showMouse(false);
                    Renderer->Camera->rotate((float)-dx / 300.0f);
                    Renderer->Camera->rotateUp((float)-dy / 300.0f);
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
                    YVec3f strafe = Renderer->Camera->RightVec;
                    strafe.Z = 0;
                    strafe.normalize();
                    strafe *= (float)-dx / 2.0f;

                    YVec3f avance = Renderer->Camera->Direction;
                    avance.Z = 0;
                    avance.normalize();
                    avance *= (float)dy / 2.0f;

                    Renderer->Camera->move(avance + strafe);
                } else {
                    YVec3f strafe = Renderer->Camera->RightVec;
                    strafe.Z = 0;
                    strafe.normalize();
                    strafe *= (float)-dx / 5.0f;

                    Renderer->Camera->move(Renderer->Camera->UpRef * (float)dy / 5.0f);
                    Renderer->Camera->move(strafe);
                    glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
                    lastx = Renderer->ScreenWidth / 2;
                    lasty = Renderer->ScreenHeight / 2;
                }
            }
        }
    }


    /* Sun calculs */
    bool getSunDirFromDayTime(YVec3f & sunDir, float mnLever, float mnCoucher, float boostTime)
    {
        bool nuit = false;

        SYSTEMTIME t;
        GetLocalTime(&t);

        //On borne le tweak time � une journ�e (cyclique)
        while (boostTime > 24 * 60)
            boostTime -= 24 * 60;

        //Temps �coul� depuis le d�but de la journ�e
        float fTime = (float)(t.wHour * 60 + t.wMinute);
        fTime += boostTime;
        while (fTime > 24 * 60)
            fTime -= 24 * 60;

        //Si c'est la nuit
        if (fTime < mnLever || fTime > mnCoucher)
        {
            nuit = true;
            if (fTime < mnLever)
                fTime += 24 * 60;
            fTime -= mnCoucher;
            fTime /= (mnLever + 24 * 60 - mnCoucher);
            fTime *= (float)M_PI;
        }
        else
        {
            //c'est le jour
            nuit = false;
            fTime -= mnLever;
            fTime /= (mnCoucher - mnLever);
            fTime *= (float)M_PI;
        }

        //Direction du soleil en fonction de l'heure
        sunDir.X = cos(fTime);
        sunDir.Y = 0.2f;
        sunDir.Z = sin(fTime);
        sunDir.normalize();

        return nuit;
    }
    void updateLights(float boostTime = 0) 
    {		
        //On recup la direciton du soleil
        bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, boostTime);
        SunPosition = Renderer->Camera->Position + SunDirection * 500.0f;

        //Pendant la journ�e
        if (!nuit)
        {
            //On definit la couleur
            SunColor =  YColor(1.0f, 1.0f, 0.8f, 1.0f);
            SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
            YColor downColor(0.9f, 0.5f, 0.1f, 1);
		
            SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
            SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
        }
        else
        {
            //La nuit : lune blanche et ciel noir
            SunColor =  YColor(1, 1, 1, 1);
            SkyColor = YColor(0, 0, 0, 1);
        }

        Renderer->setBackgroundColor(SkyColor);
    }

    int addQuadToVbo(YVbo * vbo, int iVertice, YVec3f & a, YVec3f & b, YVec3f & c, YVec3f & d) 
    {
        YVec3f normal = (b - a).cross(d - a);
        normal.normalize();

        vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 0, 0);

        iVertice++;

        vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 1, 0);

        iVertice++;

        vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 1, 1);

        iVertice++;

        vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 0, 0);

        iVertice++;

        vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 1, 1);

        iVertice++;

        vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
        vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
        vbo->setElementValue(2, iVertice, 0, 1);

        iVertice++;

        return 6;
    }
    void fillVBOCube(YVbo * vbo, float size = 5.0f) 
    {
        int iVertice = 0;

        YVec3f a( size / 2.0f, -size / 2.0f, -size / 2.0f);
        YVec3f b( size / 2.0f,  size / 2.0f, -size / 2.0f);
        YVec3f c( size / 2.0f,  size / 2.0f,  size / 2.0f);
        YVec3f d( size / 2.0f, -size / 2.0f,  size / 2.0f);
        YVec3f e(-size / 2.0f, -size / 2.0f, -size / 2.0f);
        YVec3f f(-size / 2.0f,  size / 2.0f, -size / 2.0f);
        YVec3f g(-size / 2.0f,  size / 2.0f,  size / 2.0f);
        YVec3f h(-size / 2.0f, -size / 2.0f,  size / 2.0f);

        iVertice += addQuadToVbo(vbo, iVertice, a, b, c, d); //x+
        iVertice += addQuadToVbo(vbo, iVertice, f, e, h, g); //x-
        iVertice += addQuadToVbo(vbo, iVertice, b, f, g, c); //y+
        iVertice += addQuadToVbo(vbo, iVertice, e, a, d, h); //y-
        iVertice += addQuadToVbo(vbo, iVertice, c, g, h, d); //z+
        iVertice += addQuadToVbo(vbo, iVertice, e, f, b, a); //z-
    }
};


#endif
