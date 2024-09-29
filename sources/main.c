#include "main.h"

#define SCREEN_WIDTH (1920)
#define SCREEN_HEIGHT (1080)

#define WINDOW_TITLE "SONK TEST"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    
	SetTargetFPS(60);

    playerwk pl = InitializePlayer();
    AnimFrame sonk_frame = { 0 };

    Model sonk_model = LoadModel(ASSETS_PATH"/models/sonk/sonk3fixed.gltf");
    sonk_frame.animsCount = 28;
    sonk_frame.animIndex = 0;
    sonk_frame.animCurrentFrame = 0;
    ModelAnimation *sonk_anim = LoadModelAnimations(ASSETS_PATH"/models/sonk/sonk3fixed.gltf", &sonk_frame.animsCount);

    Vector3 axisQuat = { 0 };
    float outAng = 0.0f;

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&pl.cam, CAMERA_THIRD_PERSON);

        // Select current animation
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) sonk_frame.animIndex = (sonk_frame.animIndex + 1)% sonk_frame.animsCount;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) sonk_frame.animIndex = (sonk_frame.animIndex + sonk_frame.animsCount - 1)% sonk_frame.animsCount;
         
        if (IsGamepadAvailable(pl.input.gamepad)) 
        {
            UpdateSonicInput(&pl);
        }

        SonicTheHedgehog(&pl);

        // Update model animation
        ModelAnimation anim = sonk_anim[sonk_frame.animIndex];
        sonk_frame.animCurrentFrame = (sonk_frame.animCurrentFrame + 1)%anim.frameCount;
        UpdateModelAnimation(sonk_model, anim, sonk_frame.animCurrentFrame);
        
        QuaternionToAxisAngle(pl.t.rotation, &axisQuat, &outAng);
         
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(pl.cam);
                DrawModelEx(sonk_model, pl.t.translation, axisQuat, outAng, pl.t.scale, WHITE);
                DrawCapsuleWires(pl.collider.base, pl.collider.tip, pl.collider.radius, 5, 5, GREEN);
                DrawSphereWires(pl.collider.base, pl.collider.radius, 5, 5, BLUE);
                DrawSphereWires(pl.collider.tip, pl.collider.radius, 5, 5, BLUE);
                DrawGrid(10, 1.0f);
            EndMode3D();

            DrawText("Test", 10, 10, 20, GRAY);
            DrawText(TextFormat("X: %f Y: %f Z: %f", pl.t.translation.x, pl.t.translation.y, pl.t.translation.z), 10, GetScreenHeight() - 20, 20, BLACK);
            //DrawText("Use the LEFT/RIGHT mouse buttons to switch animation", 10, 10, 20, GRAY);
            //DrawText(TextFormat("Animation: %s", anim.name), 10, GetScreenHeight() - 20, 10, DARKGRAY);

        EndDrawing();
        //----------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(sonk_model);         // Unload model and meshes/material

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

playerwk InitializePlayer() 
{
    playerwk player = { 0 };
    
    player.p = (player_parameter) { 
        60,
        2.0f,
        16.0f,
        16.0f,
        3.0f,
        0.6f,
        1.66f,
        3.0f,
        0.23f,
        0.46f,
        1.39f,
        2.3f,
        3.7f,
        5.09f,
        0.076f,
        0.05f,
        0.031f,
        -0.06f,
        -0.18f,
        -0.17f,
        -0.028f,
        -0.008f,
        -0.001f,
        -0.4f,
        -0.1f,
        -0.6f,
        -0.2825f,
        0.3f,
        4.0f,
        10.0f,
        0.08f,
        7.0f,
        5.4f
    };

    //Set transform
    player.t.translation = (Vector3){ 0.0f, 5.0f, 0.0f }; // Set model position
    player.t.rotation = (Quaternion){ 0.0f, 0.0f, 0.0f, 0.0f };
    player.t.scale = (Vector3){ 1.0f, 1.0f, 1.0f };

    //Initialize player state
    player.hpos = 0.0f;
    player.eff = Vector3Zero();
    player.acc = Vector3Zero();
    player.spd = Vector3Zero();
    player.last_up = Vector3Zero();

    //Physics state
    player.gravity = (Vector3) { 0.0f, -1.0f, 0.0f };

    //Collision state
    player.floor_normal = (Vector3) { 0.0f, 1.0f, 0.0f };
    player.wall_normal = Vector3Zero();
    player.dotp = 1.0f;

    //Movement state
    player.frict_mult = 1.0f;
    
    player.timer.jump_timer = 0;
    player.timer.spring_timer = 0.0f;
    player.timer.dashpanel_timer = 0.0f;
    player.timer.dashring_timer = 0.0f;
    player.timer.rail_debounce = 0.0f;
    
    player.timer.rail_trick = 0.0f;
    
    player.timer.spindash_speed = 0.0f;
    
    player.attr = ColFlags_Grounded;
    player.last_attr = player.attr;
    player.flag = Status_Ground;
    player.pl_state = IDLE;
    player.grounded = true;

    player.last_turn = 0.0f;

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };    // Camera position
    camera.target = player.t.translation;               // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    player.cam = camera;

    // Load gltf model
    //player.mS.model = LoadModel(ASSETS_PATH"/models/sonk/sonk3.gltf");

    // Load gltf model animations
    //player.mS.animsCount = 28;
    //player.mS.animIndex = 0;
    //player.mS.animCurrentFrame = 0;
    //player.mS.anims = LoadModelAnimations(ASSETS_PATH"/models/sonk/sonk3.gltf", &player.mS.animsCount);

    //Input setup
    player.input.gamepad = 0;
    player.input.stick_x = 0.0f;
    player.input.stick_y = 0.0f;
    player.input.stick_mag = 0.0f;
    player.input.stick_angle = 0.0f;

    player.input.jump = (InputState) { false, false, false, GAMEPAD_BUTTON_RIGHT_FACE_DOWN };
    player.input.roll = (InputState) { false, false, false, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT };

    player.collider = (Capsule){ (Vector3) { 0.0f, 1.5f, 0.0f }, (Vector3){ 0.0f, 8.5f, 0.0f }, 1.5f };

    return player;
}

void SonicTheHedgehog(playerwk* pwp) {
    pwp->last_turn = 0.0f;
    //CheckMoves(pwp);
    /*
    switch(pwp->pl_state) 
    {
        case IDLE:
            PGetRotation(pwp);
            PRotatedByGravity(pwp);
            PGetAcceleration(pwp);
            HardcodedCollision(pwp);
            if () {}
            break;
        case WALK:
            break;
        case SKID:
            break;
        case SPINDASH:
            break;
        case ROLL:
            break;
        case AIRBORNE:
            break;
        case HOMING:
            break;
        case BOUNCE:
            break;
        case RAIL:
            break;
        case LIGHTDASH:
            break;
        case AIRKICK:
            break;
        case HURT:
            break;
    }
    */
    
    //pwp->t.translation = Vector3Add(pwp->t.translation, SpdToPos(pwp->spd));
}
