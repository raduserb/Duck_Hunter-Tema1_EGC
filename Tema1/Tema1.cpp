#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <random>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

float RandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;
    float scoreWireframeLength = 4.2f;

    resX = resolution.x;
    resY = resolution.y;
    localResX = 1920;
    localResY = 1080;
    angularStep = 0;
    timePassed = 0;
    duckNr = 4;


    scoreScale = (1.0 * scoreWireframeLength / duckNr);
    cout << scoreScale << "\n";
    scoreCount = 0;

    centerX = 50;
    centerY = 33.33f;
    hitboxCenterX = 105;
    hitboxCenterY = 100;
    wingDir = 0;

    maxTime = 10;

    bullets = 3;

    lives = 3;

    int dir;

    for (int i = 0; i < duckNr; i++)
    {
        startAngle[i] = 0.88f * (rand() % 101) * 0.01 + 0.34f;

        directionX[i] = 1;
        directionY[i] = 1;


        translateX[i] = rand() % resX ;
        translateY[i] = rand() % (resY / 2 );

        headCenterX[i] = translateX[i] + 115;
        headCenterY[i] = translateY[i];

        hitboxCornerX[i] = translateX[i] - 50;
        hitboxCornerY[i] = translateY[i] - 100;

        dead[i] = 0;
        escaped[i] = 0;
        totalTime[i] = 0;
        tries[i] = 3;
        speeds[i] = 300 + 10 * i;
        active[i] = 0;
    }
    active[0] = 1;


    Mesh* grass = object2D::CreateSquare("grass", corner, squareSide, glm::vec3(0, 1, 0), true);
    AddMeshToList(grass);

    Mesh* body = object2D::CreateTriangle("body", corner, squareSide, glm::vec3(0.58f, 0.3f, 0), true);
    AddMeshToList(body);

    Mesh* wing1 = object2D::CreateTriangle("wing1", corner, squareSide, glm::vec3(0.58f, 0.3f, 0), true);
    AddMeshToList(wing1);

    Mesh* wing2 = object2D::CreateTriangle("wing2", corner, squareSide, glm::vec3(0.58f, 0.3f, 0), true);
    AddMeshToList(wing2);

    Mesh* head = object2D::CreateCircle("head", corner, 30, glm::vec3(0, 0.45f, 0));
    AddMeshToList(head);

    Mesh* beak = object2D::CreateTriangle("beak", corner, 20, glm::vec3(1, 1, 0), true);
    AddMeshToList(beak);

    Mesh* hitbox = object2D::CreateSquare("hitbox", corner, squareSide, glm::vec3(1, 0, 0), false);
    AddMeshToList(hitbox);

    Mesh* life = object2D::CreateCircle("life", corner, 30, glm::vec3(1, 0, 0));
    AddMeshToList(life);

    Mesh* bullet = object2D::CreateSquare("bullet", corner, 50, glm::vec3(0, 0, 0), true);
    AddMeshToList(bullet);

    Mesh* score = object2D::CreateSquare("score", corner, 50, glm::vec3(0, 0, 1), true);
    AddMeshToList(score);

    Mesh* wireframe = object2D::CreateSquare("wireframe", corner, 50, glm::vec3(0, 0, 0), false);
    AddMeshToList(wireframe);

}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.67f, 0.847f, 0.902f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{

    angularStep += deltaTimeSeconds * 0.5;
    if (angularStep >= (1.0 * M_PI) / 8)
    {
        angularStep = 0;
        wingDir++;
    }

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Scale(20, 2);
    RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);

    for (int i = 0; i < duckNr; i++)
    {

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(1020, 475);
        modelMatrix *= transform2D::Scale(4.2f, 1);
        RenderMesh2D(meshes["wireframe"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(1020, 475);
        modelMatrix *= transform2D::Scale(scoreCount * scoreScale, 1);
        RenderMesh2D(meshes["score"], shaders["VertexColor"], modelMatrix);

        if (lives >= 1)
        {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(1050, 650);
            RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
        }

        if (lives >= 2)
        {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(1125, 650);
            RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
        }

        if (lives >= 3)
        {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(1200, 650);
            RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
        }

        
        if (active[i])
        {
            totalTime[i] += deltaTimeSeconds;
        }

        if (totalTime[i] >= maxTime)
        {
            active[i] = 0;
            totalTime[i] = 0;
            lives--;
            escaped[i] = 1;
        }
        
        if (active[i])
        {

            if (tries[i] >= 1)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1040, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            if (tries[i] >= 2)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1115, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            if (tries[i] >= 3)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1190, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            if (directionX[i] == 1 && directionY[i] == 1)
            {
                if (hitboxCornerX[i] + 300  < resX)
                {
                    translateX[i] += (deltaTimeSeconds * speeds[i]) * cos(startAngle[i]);
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }
                else
                {
                    directionX[i] = 0;
                    translateX[i] = resX - 116;
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }

                if (hitboxCornerY[i] + 300 < resY)
                {
                    translateY[i] += (deltaTimeSeconds * speeds[i]) * sin(startAngle[i]);
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
                else {
                    directionY[i] = 0;
                    translateY[i] = resY - 1;
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
            }

            if (directionX[i] == 1 && directionY[i] == 0)
            {
                if (hitboxCornerX[i] + 300 < resX)
                {
                    translateX[i] += (deltaTimeSeconds * speeds[i]) * cos(startAngle[i]);
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }
                else
                {
                    directionX[i] = 0;
                    translateX[i] = resX - 116;
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }

                if (hitboxCornerY[i] > 0)
                {
                    translateY[i] += -(deltaTimeSeconds * speeds[i]) * sin(startAngle[i]);
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
                else {
                    directionY[i] = 1;
                    translateY[i] = 1;
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
            }

            if (directionX[i] == 0 && directionY[i] == 0)
            {
                if( hitboxCornerX[i] > 0)
                {
                    translateX[i] += -(deltaTimeSeconds * speeds[i]) * cos(startAngle[i]);
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }
                else
                {
                    directionX[i] = 1;
                    translateX[i] = -114;
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }

                if (hitboxCornerY[i] > 0)
                {
                    translateY[i] += -(deltaTimeSeconds * speeds[i]) * sin(startAngle[i]);
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
                else {
                    directionY[i] = 1;
                    translateY[i] = 1;
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
            }

            if (directionX[i] == 0 && directionY[i] == 1)
            {
                if (hitboxCornerX[i] > 0)
                {
                    translateX[i] += -(deltaTimeSeconds * speeds[i]) * cos(startAngle[i]);
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }
                else
                {
                    directionX[i] = 1;
                    translateX[i] = -114;
                    headCenterX[i] = translateX[i] + 115;
                    hitboxCornerX[i] = translateX[i] - 150;
                }

                if (hitboxCornerY[i] + 300 < resY)
                {
                    translateY[i] += (deltaTimeSeconds * speeds[i]) * sin(startAngle[i]);
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
                else {
                    directionY[i] = 0;
                    translateY[i] = resY - 1;
                    headCenterY[i] = translateY[i];
                    hitboxCornerY[i] = translateY[i] - 160;
                }
            }

            

            head_matrix = glm::mat3(1);
            head_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            //head_matrix *= transform2D::Translate(-hitboxCenterX, -hitboxCenterY);
            if(directionX[i] == 1 && directionY[i] == 1)
                head_matrix *= transform2D::Rotate(startAngle[i]);
            else if(directionX[i] == 1 && directionY[i] == 0)
                head_matrix *= transform2D::Rotate(-startAngle[i]);
            else if(directionX[i] == 0 && directionY[i] == 0)
                head_matrix *= transform2D::Rotate(M_PI + startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 1)
                head_matrix *= transform2D::Rotate(M_PI - startAngle[i]);
            //head_matrix *= transform2D::Translate(hitboxCenterX, hitboxCenterY);
            head_matrix *= transform2D::Translate(115,0);
            RenderMesh2D(meshes["head"], shaders["VertexColor"], head_matrix);

            body_matrix = glm::mat3(1);
            body_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            //body_matrix *= transform2D::Translate(-hitboxCenterX, -hitboxCenterY);
            if (directionX[i] == 1 && directionY[i] == 1)
                body_matrix *= transform2D::Rotate(startAngle[i]);
            else if (directionX[i] == 1 && directionY[i] == 0)
                body_matrix *= transform2D::Rotate(-startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 0)
                body_matrix *= transform2D::Rotate(M_PI + startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 1)
                body_matrix *= transform2D::Rotate(M_PI - startAngle[i]);
            //body_matrix *= transform2D::Translate(hitboxCenterX, hitboxCenterY);
            body_matrix *= transform2D::Translate(10,0);
            body_matrix *= transform2D::Scale(1.5f, 1.2f);
            body_matrix *= transform2D::Rotate(-(0.5f * M_PI));
            body_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["body"], shaders["VertexColor"], body_matrix);

            wing1_matrix = glm::mat3(1);
            wing1_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            //wing1_matrix *= transform2D::Translate(-hitboxCenterX, -hitboxCenterY);
            if (directionX[i] == 1 && directionY[i] == 1)
                wing1_matrix *= transform2D::Rotate(startAngle[i]);
            else if (directionX[i] == 1 && directionY[i] == 0)
                wing1_matrix *= transform2D::Rotate(-startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 0)
                wing1_matrix *= transform2D::Rotate(M_PI + startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 1)
                wing1_matrix *= transform2D::Rotate(M_PI - startAngle[i]);
            //wing1_matrix *= transform2D::Translate(hitboxCenterX, hitboxCenterY);
            wing1_matrix *= transform2D::Scale(0.5f, 1);
            wing1_matrix *= transform2D::Translate(centerX, centerY);

            if (wingDir % 2 == 0)
                wing1_matrix *= transform2D::Rotate(angularStep);
            else
                wing1_matrix *= transform2D::Rotate((1.0 * M_PI) / 8 - angularStep);

            wing1_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing1"], shaders["VertexColor"], wing1_matrix);

            wing2_matrix = glm::mat3(1);
            wing2_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            //wing2_matrix *= transform2D::Translate(-hitboxCenterX, -hitboxCenterY);
            if (directionX[i] == 1 && directionY[i] == 1)
                wing2_matrix *= transform2D::Rotate(startAngle[i]);
            else if (directionX[i] == 1 && directionY[i] == 0)
                wing2_matrix *= transform2D::Rotate(-startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 0)
                wing2_matrix *= transform2D::Rotate(M_PI + startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 1)
                wing2_matrix *= transform2D::Rotate(M_PI - startAngle[i]);
            //wing2_matrix *= transform2D::Translate(hitboxCenterX, hitboxCenterY);
            wing2_matrix *= transform2D::Scale(0.5f, 1);
            wing2_matrix *= transform2D::Translate(centerX, -centerY);
            wing2_matrix *= transform2D::Rotate(M_PI);

            if (wingDir % 2 == 0)
                wing2_matrix *= transform2D::Rotate(-angularStep);
            else
                wing2_matrix *= transform2D::Rotate(-((1.0 * M_PI) / 8 - angularStep));

            wing2_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing2"], shaders["VertexColor"], wing2_matrix);

            beak_matrix = glm::mat3(1);
            beak_matrix *= transform2D::Translate(translateX[i] , translateY[i] );
            //beak_matrix *= transform2D::Translate(-hitboxCenterX, -hitboxCenterY);
            if (directionX[i] == 1 && directionY[i] == 1)
                beak_matrix *= transform2D::Rotate(startAngle[i]);
            else if (directionX[i] == 1 && directionY[i] == 0)
                beak_matrix *= transform2D::Rotate(-startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 0)
                beak_matrix *= transform2D::Rotate(M_PI + startAngle[i]);
            else if (directionX[i] == 0 && directionY[i] == 1)
                beak_matrix *= transform2D::Rotate(M_PI - startAngle[i]);
            //beak_matrix *= transform2D::Translate(hitboxCenterX, hitboxCenterY);
            beak_matrix *= transform2D::Translate(110,  -85);
            beak_matrix *= transform2D::Translate(centerX, centerY);
            beak_matrix *= transform2D::Rotate(-((1.0 * 7) / 12 * M_PI));
            beak_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["beak"], shaders["VertexColor"], beak_matrix);


            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(hitboxCornerX[i], hitboxCornerY[i]);
            //modelMatrix *= transform2D::Translate(hitboxCornerX[i] + 105, hitboxCornerY[i] + 100);
            //modelMatrix *= transform2D::Rotate(startAngle[i]);
            //modelMatrix *= transform2D::Translate(-hitboxCornerX[i] - 105, -hitboxCornerY[i] - 100);
            modelMatrix *= transform2D::Scale(3, 3);
            //RenderMesh2D(meshes["hitbox"], shaders["VertexColor"], modelMatrix);

        }
        else if (dead[i] )
        {
            translateY[i] -= deltaTimeSeconds * speeds[i];

            if (tries[i] >= 1)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1040, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            if (tries[i] >= 2)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1115, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            if (tries[i] >= 3)
            {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(1190, 550);
                modelMatrix *= transform2D::Scale(0.4f, 1);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
            }

            head_matrix = glm::mat3(1);
            head_matrix *= transform2D::Translate(translateX[i] , translateY[i]);
            head_matrix *= transform2D::Rotate(-(M_PI / 2));
            head_matrix *= transform2D::Translate(115, 0);
            RenderMesh2D(meshes["head"], shaders["VertexColor"], head_matrix);

            body_matrix = glm::mat3(1);
            body_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            body_matrix *= transform2D::Rotate(-(M_PI / 2));
            body_matrix *= transform2D::Translate(10, 0);
            body_matrix *= transform2D::Scale(1.5f, 1.2f);
            body_matrix *= transform2D::Rotate(-(0.5f * M_PI));
            body_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["body"], shaders["VertexColor"], body_matrix);

            wing1_matrix = glm::mat3(1);
            wing1_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            wing1_matrix *= transform2D::Rotate(-(M_PI / 2));
            wing1_matrix *= transform2D::Scale(0.5f, 1);
            wing1_matrix *= transform2D::Translate(centerX, centerY);

            if (wingDir % 2 == 0)
                wing1_matrix *= transform2D::Rotate(angularStep);
            else
                wing1_matrix *= transform2D::Rotate((1.0 * M_PI) / 8 - angularStep);

            wing1_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing1"], shaders["VertexColor"], wing1_matrix);

            wing2_matrix = glm::mat3(1);
            wing2_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            wing2_matrix *= transform2D::Rotate(-(M_PI / 2));
            wing2_matrix *= transform2D::Scale(0.5f, 1);
            wing2_matrix *= transform2D::Translate(centerX, -centerY);
            wing2_matrix *= transform2D::Rotate(M_PI);

            if (wingDir % 2 == 0)
                wing2_matrix *= transform2D::Rotate(-angularStep);
            else
                wing2_matrix *= transform2D::Rotate(-((1.0 * M_PI) / 8 - angularStep));

            wing2_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing2"], shaders["VertexColor"], wing2_matrix);

            beak_matrix = glm::mat3(1);
            beak_matrix *= transform2D::Translate(translateX[i], translateY[i] );
            beak_matrix *= transform2D::Rotate(-(M_PI / 2));
            beak_matrix *= transform2D::Translate(110, -85);
            beak_matrix *= transform2D::Translate(centerX, centerY);
            beak_matrix *= transform2D::Rotate(-((1.0 * 7) / 12 * M_PI));
            beak_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["beak"], shaders["VertexColor"], beak_matrix);

            if (translateY[i] < 0)
            {
                active[i + 1] = 1;
                dead[i] = 0;
            }
        }
        else if (escaped[i] )
        {
            

            translateY[i] += deltaTimeSeconds * speeds[i];

            head_matrix = glm::mat3(1);
            head_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            head_matrix *= transform2D::Rotate(M_PI / 2);
            head_matrix *= transform2D::Translate( 115, 0);
            RenderMesh2D(meshes["head"], shaders["VertexColor"], head_matrix);

            body_matrix = glm::mat3(1);
            body_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            body_matrix *= transform2D::Rotate(M_PI / 2);
            body_matrix *= transform2D::Translate(10, 0);
            body_matrix *= transform2D::Scale(1.5f, 1.2f);
            body_matrix *= transform2D::Rotate(-(0.5f * M_PI));
            body_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["body"], shaders["VertexColor"], body_matrix);

            wing1_matrix = glm::mat3(1);
            wing1_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            wing1_matrix *= transform2D::Rotate(M_PI / 2);
            wing1_matrix *= transform2D::Scale(0.5f, 1);
            wing1_matrix *= transform2D::Translate(centerX, centerY);

            if (wingDir % 2 == 0)
                wing1_matrix *= transform2D::Rotate(angularStep);
            else
                wing1_matrix *= transform2D::Rotate((1.0 * M_PI) / 8 - angularStep);

            wing1_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing1"], shaders["VertexColor"], wing1_matrix);

            wing2_matrix = glm::mat3(1);
            wing2_matrix *= transform2D::Translate(translateX[i], translateY[i]);
            wing2_matrix *= transform2D::Rotate(M_PI / 2);
            wing2_matrix *= transform2D::Scale(0.5f, 1);
            wing2_matrix *= transform2D::Translate(centerX, -centerY);
            wing2_matrix *= transform2D::Rotate(M_PI);

            if (wingDir % 2 == 0)
                wing2_matrix *= transform2D::Rotate(-angularStep);
            else
                wing2_matrix *= transform2D::Rotate(-((1.0 * M_PI) / 8 - angularStep));

            wing2_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["wing2"], shaders["VertexColor"], wing2_matrix);

            beak_matrix = glm::mat3(1);
            beak_matrix *= transform2D::Translate(translateX[i], translateY[i] );
            beak_matrix *= transform2D::Rotate(M_PI/2);
            beak_matrix *= transform2D::Translate(110, - 85);
            beak_matrix *= transform2D::Translate(centerX, centerY);
            beak_matrix *= transform2D::Rotate(-((1.0 * 7) / 12 * M_PI));
            beak_matrix *= transform2D::Translate(-centerX, -centerY);
            RenderMesh2D(meshes["beak"], shaders["VertexColor"], beak_matrix);

            if (translateY[i] > resY)
            {
                if (lives > 0)
                    active[i + 1] = 1;
                escaped[i] = 0;
                active[i] = 0;
                totalTime[i ] = 0;
            }
        }
        
    }
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_P) {
        for (int i = 0; i < duckNr; i++)
        {
            cout << totalTime[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < duckNr; i++)
        {
            cout << active[i] << " ";
        }
        cout << endl;
    }

}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    int coordX = mouseX * (1.0 * resX / (window->GetResolution().x));
    int coordY = resY - mouseY * (1.0 * resY / (window->GetResolution().y));
    for (int i = 0; i < duckNr; i++)
    {
        if (active[i])
        {
            int minX = hitboxCornerX[i];
            int minY = hitboxCornerY[i];
            int maxX = hitboxCornerX[i] + 300;
            int maxY = hitboxCornerY[i] + 300;

            if (coordX > minX && coordY > minY && coordX < maxX && coordY < maxY)
            {
                tries[i]--;
                scoreCount++;
                bullets = 3;
                cout << "HIT\n";
                dead[i] = 1;
                active[i] = 0;
                totalTime[i] = 0;
                break;
            }
            else
            {
                tries[i]--;
                if (tries[i] == 0)
                {
                    lives--;
                    cout << "MISS\n";
                    cout << "escaped\n";
                    escaped[i] = 1;
                    active[i] = 0;
                    totalTime[i] = 0;
                    if (!lives)
                        cout << "LOSE\n";
                    break;
                }
                cout << "MISS\n";
            }
        }
    }


}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
