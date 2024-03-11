#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {

public :
	GLint ShaderCubeDebug;
	GLint ShaderSun;

	YVbo* VboCube;

	YColor SunColor;

	SYSTEMTIME local;

	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		//Chargement du shader (dans loadShaders() pour etre lié à F5)
		ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");

		ShaderSun = Renderer->createProgram("shaders/sun");

	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.5f,1.0f,1.0f));
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f());


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
	
		//VboCube->setElementValue(0, iVertice, 0, 0, 0); //Sommet (lié au layout(0) du shader)
		//VboCube->setElementValue(1, iVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		//VboCube->setElementValue(2, iVertice, 0, 0);      //UV (lié au layout(2) du shader)
		//iVertice++;

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
		MySetElement(*VboCube, element, id+0, vec1);
		MySetElement(*VboCube, element, id+1, vec2);
		MySetElement(*VboCube, element, id+2, vec3);
		id += 3;
	}

	void update(float elapsed) 
	{
		
		GetLocalTime(&local);

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

		//glRotatef(this->DeltaTimeCumul / 10.0f * 360, -1, -1, 1);

		//Exemple d'utilisation d'un shader
		glUseProgram(ShaderSun); //Demande au GPU de charger ces shaders

		// Color of the Sun
		SunColor = YColor(1.0f, 0.5f, 0.0f, 1.0f);
		GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);


		//glPushMatrix();

		glRotatef(this->DeltaTimeCumul / 10.0f * 360, 0, 0, 0);
		Renderer->Camera->setPosition(YVec3f(5.0, 25.0, 10.0));


		YVec3f sunOffset = YVec3f(-10.0, -10.0, -10.0);
		
		YVec3f sunPosition = Renderer->Camera->Position + sunOffset;
		
		glTranslatef(sunPosition.X, sunPosition.Y, sunPosition.Z);

		//glPopMatrix();


		// Color of the sky
		
		float val = cosf(local.wSecond);
		YColor skyColor = YColor(0.0, 0.5, 1.0, 1.0);
		skyColor = YColor().interpolate(skyColor, val);

		Renderer->setBackgroundColor(skyColor);

		
		Renderer->updateMatricesFromOgl(); //Calcule toute les matrices à partir des deux matrices OGL
		Renderer->sendMatricesToShader(ShaderSun); //Envoie les matrices au shader
		VboCube->render(); //Demande le rendu du VBO

	}

	void resize(int width, int height) {
	
	}

	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	

	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		
	}
	
};


#endif