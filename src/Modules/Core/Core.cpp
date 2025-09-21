#include "Core.h"

#include <flecs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/PositionComponent.h"
#include "ECS/Components/TextComponent.h"
#include "ECS/Phases.h"

namespace fc::Core
{

flecs::system gPreDrawSystem;
flecs::system gEndDrawSystem;

static void RegisterComponents(flecs::world &ecs)
{
    ecs.component<CameraComponent>();
    ecs.component<PositionComponent>();
    ecs.component<TextComponent>();
}

static void InitCommonECS(flecs::world &ecs)
{
    fc::InitPhases(ecs);
}

static void InitServerECS(flecs::world &ecs)
{
    ecs.system("Test").kind(fc::Draw2D).each([]() {
        // spdlog::info("Test system tick");
    });
}

static void InitClientECS(flecs::world &ecs)
{
    // Entity data
    Camera3D camera3D = {0};
    camera3D.position = {0.0f, 10.0f, 10.0f};
    camera3D.target = {0.0f, 0.0f, 0.0f};
    camera3D.up = {0.0f, 1.0f, 0.0f};
    camera3D.fovy = 45.0f;
    camera3D.projection = CAMERA_PERSPECTIVE;

    // Singletons and entities
    ecs.set<CameraComponent>({camera3D});

    flecs::entity cube = ecs.entity().set<PositionComponent>({0, 0, 0});

    flecs::entity text = ecs.entity()
                             .set<PositionComponent>({300, 300, 0})
                             .set<TextComponent>({});

    // Systems
    gPreDrawSystem = ecs.system<CameraComponent>()
                         .kind(fc::PreDraw)
                         .term_at(0)
                         .singleton()
                         .each([](CameraComponent &camera) {
                             if (IsCursorHidden())
                             {
                                 UpdateCamera(&camera.mCamera, CAMERA_FREE);
                             }

                             if (IsKeyPressed(KEY_C))
                             {
                                 IsCursorHidden() ? EnableCursor() : DisableCursor();
                             }

                             BeginDrawing();
                             ClearBackground(LIGHTGRAY);
                         });

    ecs.system<const CameraComponent>("BeginDraw3D")
        .kind(fc::Draw3D)
        .each([&camera3D](const CameraComponent &camera) { BeginMode3D(camera.mCamera); });

    ecs.system<const PositionComponent>("DrawCubes")
        .kind(fc::Draw3D)
        .each([](const PositionComponent &position) { DrawCube(position.mPosition, 5.0, 5.0, 5.0, RED); });

    ecs.system("EndDraw3D").kind(fc::Draw3D).each([]() { EndMode3D(); });

    ecs.system<const CameraComponent, const PositionComponent, const TextComponent>("DrawText")
        .kind(fc::Draw2D)
        .term_at(0)
        .singleton()
        .each([](const CameraComponent &camera,
                 const PositionComponent &position,
                 const TextComponent &text) {
            const Vector3 &cameraPos = camera.mCamera.position;
            DrawText(TextFormat("Camera position: %f, %f, %f", cameraPos.x,
                                cameraPos.y, cameraPos.z),
                     static_cast<int>(position.mPosition.x),
                     static_cast<int>(position.mPosition.y), 30.0, WHITE); });

    gEndDrawSystem = ecs.system().kind(fc::PostDraw).each([]() { EndDrawing(); });
}

fc::Module MODULE{&RegisterComponents, &InitCommonECS, &InitServerECS, &InitClientECS};

};  // namespace fc::Core
