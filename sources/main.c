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
        60,       //how long in frames can sonic "hang" in air while jumping
        2.0f,     //amount of grip on floor or smthn
        16.0f,    //hard limit h speed cap
        16.0f,    //hard limit v speed cap
        3.0f,     //"soft" h speed cap
        0.6f,     //max push object speed
        1.66f,    //initial jump speed
        3.0f,     //nocontrol i.e spring speed
        0.23f,    //slide speed (lost world slide?)
        0.46f,    //jog speed - rolling cancel speed as well
        1.39f,    //run speed - rolling end speed as well
        2.3f,     //rush speed (normal run speed methinks)
        3.7f,     //crash speed, min speed to get knocked back when hitting wall, affects pushing speed too?
        5.09f,    //dash speed - normally shouldn't be at this speed, but if you are, sonic gets some top speed anim and steering is hard
        0.076f,   //speed added when holding jump
        0.05f,    //ground accel
        0.031f,   //air accel
        -0.06f,   //ground decel
        -0.18f,   //ground brake speed 
        -0.17f,   //air brake speed 
        -0.028f,  //air decel (lmao called air_resist_air)
        -0.008f,  //rolling decel (in air too i guess) (called air_resist. gosh, ain't that confusing naming?)
        -0.01f,   //called air_resist_y, drag for y_spd
        -0.4f,    //called air_resist_z, drag for z_spd
        -0.1f,    //grd_frict - min speed before stopping, affects spindash charging and braking
        -0.6f,    //grd_frict_z - affects movement on sloped surfaces
        -0.2825f, //lim_frict - affects accel on ground. limit of how much friction affects char???
        0.3f,     //rat_bound - idk lol
        4.0f,     //rad - sonic retro says something about "Ripple Size?" like. stepping in puddle??
        10.0f,    //sonic col height - bro is 1m tall thats a block in minecraft
        0.08f,    //called "weight", technically gravity scales with this value, i.e "heavier" chars fall faster. kinda funny
        7.0f,     //eyes_height - camera y offset
        5.4f      //center_height - y offset of player
    };

    //Set transform
    player.t.translation = (Vector3){ 0.0f, player.p.center_height, 0.0f }; // Set model position
    player.t.rotation = (Quaternion){ 0.0f, 0.0f, 0.0f, 0.0f };
    player.t.scale = (Vector3){ 1.0f, 1.0f, 1.0f };

    //Initialize player state
    //player.hpos = 0.0f;
    //player.eff = Vector3Zero();
    //player.acc = Vector3Zero();
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
            if (pwp->grounded == false) 
            {
                pwp->pl_state = AIRBORNE;
            }
            break;
        case WALK:
            PGetAcceleration(pwp);
            HardcodedCollision(pwp);
            break;
        case SKID:
            PGetSkidSpeed(pwp);
            HardcodedCollision(pwp);
            break;
        case SPINDASH:
            PGetRotation(pwp);
            PGetBrake(pwp);
            HardcodedCollision(pwp);
            break;
        case ROLL:
            PGetRotation(pwp);
            PGetInertia(pwp);
            HardcodedCollision(pwp);
            break;
        case AIRBORNE:
            PGetAccelerationAir(pwp);
            if (pwp->timer.spring_timer <= 0 && pwp->timer.dashring_timer <= 0) {
                AlignToGravity(pwp);
            }
            float fall_ysp = -pwp->spd.y;
            pwp->grounded = false;
            HardcodedCollision(pwp);
            
            if (pwp->grounded) 
            {
                //landed
                if (fabsf(pwp->spd.x) < pwp->p.jog_speed) 
                {
                    pwp->spd.x = 0.0f;
                    pwp->pl_state = IDLE;
                }
                else {
                    pwp->pl_state = GetWalkState(pwp);
                } 
            }
            break;
        //case HOMING:
        //    
        //    HardcodedCollision(pwp);
        //    break;
        //case BOUNCE:
        //    
        //    HardcodedCollision(pwp);
        //    break;
        //case RAIL:
        //    break;
        //case LIGHTDASH:
        //    
        //    HardcodedCollision(pwp);
        //    break;
        //case AIRKICK:
        //    
        //    HardcodedCollision(pwp);
        //    break;
        //case HURT:
        //    
        //    HardcodedCollision(pwp);
        //    break;
    }
    */
    
    //pwp->t.translation = Vector3Add(pwp->t.translation, SpdToPos(pwp->spd));
}
