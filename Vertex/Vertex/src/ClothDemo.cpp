#include <windows.h>

#include "Cloth.h"
#include "ClothDemo.h"
#include "CoreServices.h"
#include "FreeCamera.h"
#include "VertexEngineCore.h"
#include "DirectionalLight.h"

#include <glm\gtc\constants.hpp>
#include <glm\gtc\random.hpp>

void gotoxy(int x, int y)
{
    COORD p = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

ClothDemo::ClothDemo()
{
    cam = new FreeCamera(60.0f, 1024, 768, 0.1f, 1000.0f);
    cam->setPosition(6.84f, 8.12f, 18.93f);
    cam->setYaw(-109.2f);
    cam->setPitch(-24.8f);

    scene = new Scene(cam);
    CoreServices::getCore()->setScene(scene);

    Model * sphere = CoreAssetManager::createModel();
    sphere->genSphere(1.0f, 4);
    sphere->setTexture("res/texture/white_4x4.jpg");
    sphere->setDiffuseColor(0, glm::vec3(1, 0, 0));
    sphere->setPosition(glm::vec3(10, 0, 0));

    DirectionalLight * dirLight = new DirectionalLight();
    dirLight->setDirection(glm::vec3(0, -1, 1));

    cloth = new Cloth(40, 40, 4.0f, 4.0f);
    cloth->simulate = false;

    scene->addChild(sphere);
    scene->addChild(dirLight);
    scene->addChild(cloth);

    /* Skybox */
    CoreServices::getRenderer()->createSkybox("res/skyboxes/stormydays/stormydays_rt.tga",
                                              "res/skyboxes/stormydays/stormydays_lf.tga",
                                              "res/skyboxes/stormydays/stormydays_dn.tga",
                                              "res/skyboxes/stormydays/stormydays_up.tga",
                                              "res/skyboxes/stormydays/stormydays_ft.tga",
                                              "res/skyboxes/stormydays/stormydays_bk.tga");
}

ClothDemo::~ClothDemo()
{
    delete scene;
    scene = nullptr;
}

bool vsync = true;
bool isWire = false;
void ClothDemo::processInput()
{
    if (Input::getKeyDown(Input::Escape))
    {
        CoreServices::getCore()->quitApp();
    }

    if (Input::getKeyUp(Input::V))
    {
        vsync = !vsync;
        CoreServices::getCore()->setVSync(vsync);

        gotoxy(0, 7);
        if (vsync)
        {
            printf("Vsync ON ");
        }
        else
        {
            printf("Vsync OFF");
        }
    }

    if (Input::getKeyUp(Input::G))
    {
        isWire = !isWire;

        if(isWire)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (Input::getKeyUp(Input::P))
    {
        gotoxy(0, 9);
        if(cloth->simulate = !cloth->simulate)
        {
            printf("Start simulation [P]");
        }
        else
        {
            printf("Pause simulation [P]");
        }
    }

    if (Input::getKeyUp(Input::R))
    {
        gotoxy(0, 11);
        printf("Reset simulation");
        cloth->reset();
    }
}

void ClothDemo::update(float delta)
{
    gotoxy(0, 0);
    printf("FPS = %d || MS =  %.3f     \r", CoreServices::getCore()->getFPS(), 1000.0f / CoreServices::getCore()->getFPS());
    
    gotoxy(0, 2);
    printf("========= Camera info =========\n"
           "| Position = %.2f, %.2f, %.2f\n"
           "| Yaw      = %.2f\n"
           "| Pitch    = %.2f",
           cam->getPosition().x, cam->getPosition().y, cam->getPosition().z,
           cam->getYaw(), 
           cam->getPitch());
}

void ClothDemo::onGUI()
{
}
