#include "input.h"

#include <math.h>

void UpdateSonicInput(playerwk* pwp)
{
    float stick_x = GetGamepadAxisMovement(pwp->input.gamepad, GAMEPAD_AXIS_LEFT_X);
    float stick_y = GetGamepadAxisMovement(pwp->input.gamepad, GAMEPAD_AXIS_LEFT_Y);

    int gamepad = pwp->input.gamepad;
    int jump_map = pwp->input.jump.keyMap;
    int roll_map = pwp->input.roll.keyMap;

    pwp->input.jump.down = IsGamepadButtonDown(gamepad, jump_map);
    pwp->input.jump.press = IsGamepadButtonPressed(gamepad, jump_map);
    pwp->input.jump.release = IsGamepadButtonReleased(gamepad, jump_map);

    pwp->input.roll.down = IsGamepadButtonDown(gamepad, roll_map);
    pwp->input.roll.press = IsGamepadButtonPressed(gamepad, roll_map);
    pwp->input.roll.release = IsGamepadButtonReleased(gamepad, roll_map);

    pwp->input.stick_mag = sqrtf((stick_x * stick_x) + (stick_y * stick_y));
    pwp->input.stick_angle = atan2f(-stick_x, stick_y);
    pwp->input.stick_turn = 0.0f;

    if (pwp->input.stick_mag > 0.15f)
    {
        if (pwp->input.stick_mag > 1.0f) 
        {
            pwp->input.stick_x = stick_x / pwp->input.stick_mag;
            pwp->input.stick_y = stick_y / pwp->input.stick_mag;
            pwp->input.stick_mag = 1.0f;
        }
        else
        {
            pwp->input.stick_x = stick_x;
            pwp->input.stick_y = stick_y;
        }
    }
    else
    {
        pwp->input.stick_x = 0.0f;
        pwp->input.stick_y = 0.0f;
        pwp->input.stick_mag = 0.0f;
    }
}

float GetAnalog_Turn(playerwk *pwp) 
{
    if (!FloatEquals(pwp->input.stick_mag, 0.0f))
    {
        Vector3 tgt_up = (Vector3){ 0.0f, 1.0f, 0.0f };
        Vector3 look = GetLook(pwp->t.rotation);
        Vector3 up = GetUp(pwp->t.rotation);

        Vector3 cam_look = PlaneProject(pwp->cam.up, tgt_up);

        if (!FloatEquals(Vector3Length(cam_look), 0.0f)) {
            cam_look = Vector3Normalize(cam_look);
        } 
        else {
            cam_look = look;
        }
        Vector3 cam_move = { 0 };
        cam_move = Vector3Multiply(Vector3RotateByAxisAngle(cam_move, tgt_up, atan2f(-pwp->input.stick_x, -pwp->input.stick_y)), cam_look);

        if (Vector3Equals(pwp->last_up, (Vector3){ 0 }) || Vector3DotProduct(tgt_up, up) >= 0.999f) {
            pwp->last_up = up;
        }

        Quaternion final_rotation = QuaternionFromVector3ToVector3(tgt_up, pwp->last_up);
        Vector3 final_move = PlaneProject(Vector3RotateByQuaternion(cam_move, final_rotation), up);

        if (!FloatEquals(Vector3Length(final_move), 0.0f)) {
            final_move = Vector3Normalize(final_move);
        } 
        else {
            final_move = look;
        }

        pwp->last_turn = Vector3SignedAngle(look, final_move, up);
        return pwp->last_turn;
    }
    pwp->last_turn = 0.0f;
    return pwp->last_turn;
}

float GetAnalog_Mag(playerwk* pwp) 
{
    return pwp->input.stick_mag;
}

bool GetAnalog(playerwk* pwp) 
{
    float turn = GetAnalog_Turn(pwp);
    float mag = GetAnalog_Mag(pwp);
    return !FloatEquals(mag, 0.0f);
}
