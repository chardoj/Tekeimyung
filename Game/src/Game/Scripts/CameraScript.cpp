/*
** @Author : Simon AMBROISE
*/

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <Engine/EntityFactory.hpp>
#include <Engine/Core/Components/Components.hh>
#include <Engine/Window/GameWindow.hpp>
#include <Engine/Debug/Logger.hpp>
#include <Game/Scripts/CameraScript.hpp>

REGISTER_SCRIPT(CameraScript);

void CameraScript::start()
{
    auto    em = EntityFactory::getBindedEntityManager();

    _cameraComp = entity->getComponent<sCameraComponent>();
    _scrollSpeed = 1500.0f;

    auto &mouse = GameWindow::getInstance()->getMouse();
    auto &&scroll = mouse.getScroll();
    _lastScrollOffset = scroll.yOffset;
    this->_initialPosition = this->getComponent< sTransformComponent >()->getPos();
}

void CameraScript::update(float dt)
{
    const auto &gameWindow = GameWindow::getInstance();
    auto &mouse = gameWindow->getMouse();
    auto &keyboard = gameWindow->getKeyboard();
    auto& camera = _cameraComp->camera;

#if defined (ENGINE_DEBUG)
    // update Projection type
    {
        if (keyboard.isPressed(Keyboard::eKey::O))
            camera.setProjType(Camera::eProj::ORTHOGRAPHIC_3D);
        else if (keyboard.isPressed(Keyboard::eKey::P))
            camera.setProjType(Camera::eProj::PERSPECTIVE);
    }
#endif

    if (keyboard.getStateMap()[Keyboard::eKey::Y] == Keyboard::eKeyState::KEY_RELEASED)
    {
        sTransformComponent*    entityTransform = this->getComponent<sTransformComponent>();

        if (entityTransform != nullptr)
            entityTransform->setPos(this->_initialPosition);
    }

    // update zoom
    {
        auto &&scroll = mouse.getScroll();

        double offset = scroll.yOffset - _lastScrollOffset;

        if (offset && keyboard.isPressed(Keyboard::eKey::LEFT_CONTROL))
        {
            camera.setProjSize(camera.getProjSize() + (float)offset * dt * _scrollSpeed);

            // Limit max projection size to 700
            camera.setProjSize(std::min(camera.getProjSize(), 700.0f));
            // Limit min projection size to 250
            camera.setProjSize(std::max(camera.getProjSize(), 250.0f));
            //camera.zoom((float)(-offset * dt));
        }
        _lastScrollOffset = scroll.yOffset;
    }


    if (this->_followEntity == true)
    {
        static float moveSpeed = 300.0f;

        if (this->_entityToFollow != nullptr)
        {
            sTransformComponent*    entityTransform = this->_entityToFollow->getComponent<sTransformComponent>();

            if (entityTransform != nullptr)
            {
                camera.translate({ moveSpeed * dt, 0.0f, 0.0f }, Camera::eTransform::LOCAL);
            }
        }
    }
    //  Update camera position when reaching edge
    else
    {
        static float edgeDist = 80.0f;
        static float moveSpeed = 300.0f;
        auto& cursor = mouse.getCursor();
        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse)
        {
            if (cursor.getX() > gameWindow->getBufferWidth() - edgeDist)
            {
                camera.translate({ moveSpeed * dt, 0.0f, 0.0f }, Camera::eTransform::LOCAL);
            }
            if (cursor.getX() < edgeDist)
            {
                camera.translate({ -moveSpeed * dt, 0.0f, 0.0f }, Camera::eTransform::LOCAL);
            }
            if (cursor.getY() > gameWindow->getBufferHeight() - edgeDist)
            {
                // Double moveSpeed for Y because it seems to be slower then X due to isometric view
                camera.translate({ 0.0f, -moveSpeed * dt, 0.0f }, Camera::eTransform::LOCAL);
            }
            if (cursor.getY() < edgeDist)
            {
                // Double moveSpeed for Y because it seems to be slower then X due to isometric view
                camera.translate({ 0.0f, moveSpeed * dt, 0.0f }, Camera::eTransform::LOCAL);
            }
        }

    }
}
