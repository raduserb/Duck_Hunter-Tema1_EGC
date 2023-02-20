#pragma once

#include "components/simple_scene.h"

#define MAX_DUCKS 1000

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

    protected:
        float cx, cy;
        glm::mat3 modelMatrix, head_matrix, body_matrix, wing1_matrix, wing2_matrix, beak_matrix;
        float angularStep, startAngle[MAX_DUCKS],actualAngle[MAX_DUCKS];
        float centerX, centerY,translateX[MAX_DUCKS], translateY[MAX_DUCKS], totalTime[MAX_DUCKS], localResX, localResY,scoreScale;
        int wingDir,duckNr,maxTime, bullets, lives, scoreCount;
        int headCenterX[MAX_DUCKS], headCenterY[MAX_DUCKS], resX, resY, hitboxCornerX[MAX_DUCKS], hitboxCornerY[MAX_DUCKS], hitboxCenterX, hitboxCenterY;
        int tries[MAX_DUCKS], speeds[MAX_DUCKS], directionX[MAX_DUCKS], directionY[MAX_DUCKS];
        bool dead[MAX_DUCKS], escaped[MAX_DUCKS], active[MAX_DUCKS],timePassed;
        // TODO(student): If you need any other class variables, define them here.

    };
}   // namespace m1
