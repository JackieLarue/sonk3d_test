#include "main.h"

bool CheckJump(playerwk* pwp)
{
	//Check for jumping
	if (pwp->input.jump.press)
	{
		pwp->spd.y = pwp->p.jmp_y_spd;
		//UseFloorMove();
		pwp->timer.jump_timer = pwp->p.jump2_timer;
		pwp->grounded = false;
		//setrail(none)
		pwp->pl_state = AIRBORNE;
		//Enterball();

		//Set anims and stuff
		//pwp->mS.animIndex = 0;
		return true;
	}
	return false;
}

bool CheckSpinDash(playerwk* pwp)
{
	if (pwp->input.roll.press)
	{
		pwp->pl_state = SPINDASH;
		//EnterBall();
		pwp->timer.spindash_speed = fmaxf(pwp->spd.x, 2.0f);
		//Playsound
		return true;
	}
	return false;
}

bool CheckUncurl(playerwk* pwp)
{
	if (pwp->input.roll.press)
	{
		return true;
	}
	return false;
}

/*
bool CheckLightSpeedDash(self, object_instance)
	--Check for light speed dash
	self.secondary_action = "LightSpeedDash"
	if self.input.button_press.secondary_action and lsd.CheckStartLSD(self, object_instance) {
		--Start light speed dash
		self.animation = "LSD"
		self.state = constants.state.light_speed_dash
		self:ExitBall()
		self:ResetObjectState()
		return true
	}
	return false
}

bool CheckHomingAttack(self, object_instance)
	--Check for homing attack
	if self.flag.ball_aura {
		self.jump_action = "HomingAttack"
		if self.input.button_press.jump {
			if homing_attack.CheckStartHoming(self, object_instance) {
				--Homing attack
				self.animation = "Roll"
				self:EnterBall()
			else
				--Jump dash
				self.spd = vector.SetX(self.spd, 5)
				self.animation = "Fall"
				self:ExitBall()
				self.flag.dash_aura = true
				sound.PlaySound(self, "Dash")
			}
			
			--Enter homing attack state
			self.state = constants.state.homing
			self.homing_timer = 0
			sound.PlaySound(self, "Dash")
			return true
		}
	}
	return false
}

bool CheckBounce(self)
	--Check for bounce
	if self.flag.ball_aura {
		self.roll_action = "Bounce"
		if self.input.button_press.roll {
			--Bounce
			self.state = constants.state.bounce
			self.animation = "Roll"
			self.spd = vector.MulX(self.spd, 0.75)
			if self.flag.bounce2 == true {
				self.spd = vector.SetY(self.spd, -7)
			else
				self.spd = vector.SetY(self.spd, -5)
			}
			self.anim_speed = -self.spd.Y
			return true
		}
	}
	return false
}

bool CheckAirKick(self)
	--Check for air kick
	if self.flag.air_kick {
		self.tertiary_action = "AirKick"
		if self.input.button_press.tertiary_action {
			--Air kick
			self:GiveScore(200)
			self.state = constants.state.air_kick
			self:ExitBall()
			if input.GetAnalogue_Mag(self) <= 0 {
				self.animation = "AirKickUp"
				self.spd = Vector3.new(0.2, 2.65, 0)
				self.air_kick_timer = 60
			else
				self.animation = "AirKick"
				self.spd = Vector3.new(4.5, 1.425, 0)
				self.air_kick_timer = 120
			}
			return true
		}
	}
	return false
}

bool CheckSkid(self)
	local has_control, analogue_turn, _ = input.GetAnalogue(self)
	if has_control {
		return math.abs(analogue_turn) > math.rad(135)
	}
	return false
}

local CheckStopSkid(self)
	if self.spd.X <= 0.01 {
		--We've stopped, stop skidding
		self.spd = vector.SetX(self.spd, 0)
		return true
	else
		--If holding forward, stop skidding
		local has_control, analogue_turn, _ = input.GetAnalogue(self)
		if has_control {
			return math.abs(analogue_turn) <= math.rad(135)
		}
		return false
	}
}

local CheckStartWalk(self)
	local has_control, _, _ = input.GetAnalogue(self)
	if has_control or math.abs(self.spd.X) > self.p.slide_speed {
		self.state = constants.state.walk
		return true
	}
	return false
}

local CheckStopWalk(self)
	local has_control, _, _ = input.GetAnalogue(self)
	if has_control or math.abs(self.spd.X) > 0.01 {
		return false
	}
	
	self.state = constants.state.idle
	return true
}

local CheckMoves(self, object_instance)
	if self.do_ragdoll {
		self.state = constants.state.ragdoll
		self.do_ragdoll = false
		return true
	}
	
	return switch(self.state, {}, {
		[constants.state.idle] = ()
			return CheckLightSpeedDash(self, object_instance) or CheckJump(self) or CheckSpindash(self) or CheckStartWalk(self)
		},
		[constants.state.walk] = ()
			if CheckLightSpeedDash(self, object_instance) or CheckJump(self) or CheckSpindash(self) or CheckStopWalk(self) {
				return true
			else
				--Check if we should start skidding
				if self.spd.X > self.p.jog_speed and CheckSkid(self) {
					--Start skidding
					self.state = constants.state.skid
					sound.PlaySound(self, "Skid")
					return true
				}
			}
			return false
		},
		[constants.state.skid] = ()
			if CheckLightSpeedDash(self, object_instance) or CheckJump(self) or CheckSpindash(self) {
				return true
			else
				--Check if we should stop skidding
				if CheckStopSkid(self) {
					--Stop skidding
					self.state = GetWalkState(self)
					return true
				}
			}
			return false
		},
		[constants.state.roll] = ()
			if CheckLightSpeedDash(self, object_instance) or CheckJump(self) or CheckUncurl(self) {
				return true
			else
				if self.spd.X < self.p.run_speed {
					if self.flag.ceiling_clip {
						--Force us to keep rolling
						self.spd = vector.SetX(self.spd, self.p.run_speed)
					else
						--Uncurl if moving too slow
						self.state = GetWalkState(self)
						self:ExitBall()
						return true
					}
				}
			}
			return false
		},
		[constants.state.spindash] = ()
			if CheckLightSpeedDash(self, object_instance) {
				return true
			else
				self.roll_action = "Spindash"
				if self.input.button.roll {
					--Increase spindash speed
					if self.spindash_speed < 10 or self.v3 == true {
						self.spindash_speed += ((self.v3 == true) and 0.1 or 0.4)
					}
				else
					--Release spindash
					self.state = constants.state.roll
					self:EnterBall()
					self.spd = vector.SetX(self.spd, self.spindash_speed)
					sound.StopSound(self, "SpindashCharge")
					sound.PlaySound(self, "SpindashRelease")
					return true
				}
			}
			return false
		},
		[constants.state.airborne] = ()
			return CheckLightSpeedDash(self, object_instance) or CheckHomingAttack(self, object_instance) or CheckBounce(self) or CheckAirKick(self)
		},
		[constants.state.homing] = ()
			if self.homing_obj == nil {
				if CheckLightSpeedDash(self, object_instance) {
					return true
				}
			else
				self.jump_action = "Jump"
			}
			return false
		},
		[constants.state.bounce] = ()
			self.roll_action = "Bounce"
			return CheckLightSpeedDash(self, object_instance) or CheckHomingAttack(self, object_instance)
		},
		[constants.state.light_speed_dash] = ()
			self.secondary_action = "LightSpeedDash"
			return false
		},
		[constants.state.air_kick] = ()
			return CheckLightSpeedDash(self, object_instance)
		},
		[constants.state.rail] = ()
			self.jump_action = "Jump"
			self.roll_action = "Crouch"
			if self.input.button_press.jump {
				if rail.CheckSwitch(self) {
					--Rail switch jump
					sound.PlaySound(self, "Jump")
					return true
				else if rail.CheckTrick(self) {
					--Trick jump
					sound.PlaySound(self, "Jump")
					return true
				else
					--Normal jump
					return CheckJump(self)
				}
			}
			return false
		},
	}) or false
}
*/