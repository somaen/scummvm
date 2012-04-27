/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2011 Jan Nedoma

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "engines/wintermute/dcgf.h"
#include "engines/wintermute/PartEmitter.h"
#include "engines/wintermute/Vector2.h"
#include "engines/wintermute/Matrix4.h"
#include "engines/wintermute/scriptables/ScValue.h"
#include "engines/wintermute/scriptables/ScStack.h"
#include "engines/wintermute/BGame.h"
#include "engines/wintermute/BRegion.h"
#include "engines/wintermute/BFileManager.h"
#include "engines/wintermute/utils.h"
#include "engines/wintermute/PlatformSDL.h"
#include "common/str.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CPartEmitter, false)

//////////////////////////////////////////////////////////////////////////
CPartEmitter::CPartEmitter(CBGame *inGame, CBScriptHolder *Owner) : CBObject(inGame) {
	_width = _height = 0;

	CBPlatform::SetRectEmpty(&_border);
	_borderThicknessLeft = _borderThicknessRight = _borderThicknessTop = _borderThicknessBottom = 0;

	_angle1 = _angle2 = 0;

	_velocity1 = _velocity2 = 0.0f;
	_velocityZBased = false;

	_scale1 = _scale2 = 100.0f;
	_scaleZBased = false;

	_maxParticles = 100;

	_lifeTime1 = _lifeTime2 = 1000;
	_lifeTimeZBased = false;

	_lastGenTime = 0;
	_genInterval = 0;
	_genAmount = 1;

	_overheadTime = 0;
	_running = false;

	_maxBatches = 0;
	_batchesGenerated = 0;

	_fadeInTime = _fadeOutTime = 0;

	_alpha1 = _alpha2 = 255;
	_alphaTimeBased = false;

	_rotation1 = _rotation2 = 0.0f;
	_angVelocity1 = _angVelocity2 = 0.0f;

	_growthRate1 = _growthRate2 = 0.0f;
	_exponentialGrowth = false;

	_useRegion = false;

	_emitEvent = NULL;
	_owner = Owner;
}


//////////////////////////////////////////////////////////////////////////
CPartEmitter::~CPartEmitter(void) {
	for (int i = 0; i < _particles.GetSize(); i++) {
		delete _particles[i];
	}
	_particles.RemoveAll();

	for (int i = 0; i < _forces.GetSize(); i++) {
		delete _forces[i];
	}
	_forces.RemoveAll();


	for (int i = 0; i < _sprites.GetSize(); i++) {
		delete [] _sprites[i];
	}
	_sprites.RemoveAll();

	delete[] _emitEvent;
	_emitEvent = NULL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::AddSprite(char *Filename) {
	if (!Filename) return E_FAIL;

	// do we already have the file?
	for (int i = 0; i < _sprites.GetSize(); i++) {
		if (scumm_stricmp(Filename, _sprites[i]) == 0) return S_OK;
	}

	// check if file exists
	CBFile *File = Game->_fileManager->OpenFile(Filename);
	if (!File) {
		Game->LOG(0, "Sprite '%s' not found", Filename);
		return E_FAIL;
	} else Game->_fileManager->CloseFile(File);

	char *Str = new char[strlen(Filename) + 1];
	strcpy(Str, Filename);
	_sprites.Add(Str);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::RemoveSprite(char *Filename) {
	for (int i = 0; i < _sprites.GetSize(); i++) {
		if (scumm_stricmp(Filename, _sprites[i]) == 0) {
			delete [] _sprites[i];
			_sprites.RemoveAt(i);
			return S_OK;
		}
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::InitParticle(CPartParticle *Particle, uint32 CurrentTime, uint32 TimerDelta) {
	if (!Particle) return E_FAIL;
	if (_sprites.GetSize() == 0) return E_FAIL;

	int PosX = CBUtils::RandomInt(_posX, _posX + _width);
	int PosY = CBUtils::RandomInt(_posY, _posY + _height);
	float PosZ = CBUtils::RandomFloat(0.0f, 100.0f);

	float Velocity;
	if (_velocityZBased) Velocity = _velocity1 + PosZ * (_velocity2 - _velocity1) / 100;
	else Velocity = CBUtils::RandomFloat(_velocity1, _velocity2);

	float Scale;
	if (_scaleZBased) Scale = _scale1 + PosZ * (_scale2 - _scale1) / 100;
	else Scale = CBUtils::RandomFloat(_scale1, _scale2);

	int LifeTime;
	if (_lifeTimeZBased) LifeTime = _lifeTime2 - PosZ * (_lifeTime2 - _lifeTime1) / 100;
	else LifeTime = CBUtils::RandomInt(_lifeTime1, _lifeTime2);

	float Angle = CBUtils::RandomAngle(_angle1, _angle2);
	int SpriteIndex = CBUtils::RandomInt(0, _sprites.GetSize() - 1);

	float Rotation = CBUtils::RandomAngle(_rotation1, _rotation2);
	float AngVelocity = CBUtils::RandomFloat(_angVelocity1, _angVelocity2);
	float GrowthRate = CBUtils::RandomFloat(_growthRate1, _growthRate2);

	if (!CBPlatform::IsRectEmpty(&_border)) {
		int ThicknessLeft   = _borderThicknessLeft   - (float)_borderThicknessLeft   * PosZ / 100.0f;
		int ThicknessRight  = _borderThicknessRight  - (float)_borderThicknessRight  * PosZ / 100.0f;
		int ThicknessTop    = _borderThicknessTop    - (float)_borderThicknessTop    * PosZ / 100.0f;
		int ThicknessBottom = _borderThicknessBottom - (float)_borderThicknessBottom * PosZ / 100.0f;

		Particle->_border = _border;
		Particle->_border.left += ThicknessLeft;
		Particle->_border.right -= ThicknessRight;
		Particle->_border.top += ThicknessTop;
		Particle->_border.bottom -= ThicknessBottom;
	}

	Vector2 VecPos((float)PosX, (float)PosY);
	Vector2 VecVel(0, Velocity);

	Matrix4 MatRot;
	MatRot.RotationZ(DegToRad(CBUtils::NormalizeAngle(Angle - 180)));
	MatRot.TransformVector2(VecVel);

	if (_alphaTimeBased) {
		Particle->_alpha1 = _alpha1;
		Particle->_alpha2 = _alpha2;
	} else {
		int Alpha = CBUtils::RandomInt(_alpha1, _alpha2);
		Particle->_alpha1 = Alpha;
		Particle->_alpha2 = Alpha;
	}

	Particle->_creationTime = CurrentTime;
	Particle->_pos = VecPos;
	Particle->_posZ = PosZ;
	Particle->_velocity = VecVel;
	Particle->_scale = Scale;
	Particle->_lifeTime = LifeTime;
	Particle->_rotation = Rotation;
	Particle->_angVelocity = AngVelocity;
	Particle->_growthRate = GrowthRate;
	Particle->_exponentialGrowth = _exponentialGrowth;
	Particle->_isDead = FAILED(Particle->SetSprite(_sprites[SpriteIndex]));
	Particle->FadeIn(CurrentTime, _fadeInTime);


	if (Particle->_isDead) return E_FAIL;
	else return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::Update() {
	if (!_running) return S_OK;
	else return UpdateInternal(Game->_timer, Game->_timerDelta);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::UpdateInternal(uint32 CurrentTime, uint32 TimerDelta) {
	int NumLive = 0;

	for (int i = 0; i < _particles.GetSize(); i++) {
		_particles[i]->Update(this, CurrentTime, TimerDelta);

		if (!_particles[i]->_isDead) NumLive++;
	}


	// we're understaffed
	if (NumLive < _maxParticles) {
		bool NeedsSort = false;
		if (CurrentTime - _lastGenTime > _genInterval) {
			_lastGenTime = CurrentTime;
			_batchesGenerated++;

			if (_maxBatches > 0 && _batchesGenerated > _maxBatches) {
				return S_OK;
			}

			int ToGen = std::min(_genAmount, _maxParticles - NumLive);
			while (ToGen > 0) {
				int FirstDeadIndex = -1;
				for (int i = 0; i < _particles.GetSize(); i++) {
					if (_particles[i]->_isDead) {
						FirstDeadIndex = i;
						break;
					}
				}

				CPartParticle *Particle;
				if (FirstDeadIndex >= 0) Particle = _particles[FirstDeadIndex];
				else {
					Particle = new CPartParticle(Game);
					_particles.Add(Particle);
				}
				InitParticle(Particle, CurrentTime, TimerDelta);
				NeedsSort = true;

				ToGen--;
			}
		}
		if (NeedsSort && (_scaleZBased || _velocityZBased || _lifeTimeZBased))
			SortParticlesByZ();

		// we actually generated some particles and we're not in fast-forward mode
		if (NeedsSort && _overheadTime == 0) {
			if (_owner && _emitEvent) _owner->ApplyEvent(_emitEvent);
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::Display(CBRegion *Region) {
	if (_sprites.GetSize() <= 1) Game->_renderer->StartSpriteBatch();

	for (int i = 0; i < _particles.GetSize(); i++) {
		if (Region != NULL && _useRegion) {
			if (!Region->PointInRegion(_particles[i]->_pos.x, _particles[i]->_pos.y)) continue;
		}

		_particles[i]->Display(this);
	}

	if (_sprites.GetSize() <= 1) Game->_renderer->EndSpriteBatch();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::Start() {
	for (int i = 0; i < _particles.GetSize(); i++) {
		_particles[i]->_isDead = true;
	}
	_running = true;
	_batchesGenerated = 0;


	if (_overheadTime > 0) {
		uint32 Delta = 500;
		int Steps = _overheadTime / Delta;
		uint32 CurrentTime = Game->_timer - _overheadTime;

		for (int i = 0; i < Steps; i++) {
			UpdateInternal(CurrentTime, Delta);
			CurrentTime += Delta;
		}
		_overheadTime = 0;
	}


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::SortParticlesByZ() {
	// sort particles by _posY
	qsort(_particles.GetData(), _particles.GetSize(), sizeof(CPartParticle *), CPartEmitter::CompareZ);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
int CPartEmitter::CompareZ(const void *Obj1, const void *Obj2) {
	CPartParticle *P1 = *(CPartParticle **)Obj1;
	CPartParticle *P2 = *(CPartParticle **)Obj2;

	if (P1->_posZ < P2->_posZ) return -1;
	else if (P1->_posZ > P2->_posZ) return 1;
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::SetBorder(int X, int Y, int Width, int Height) {
	CBPlatform::SetRect(&_border, X, Y, X + Width, Y + Height);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::SetBorderThickness(int ThicknessLeft, int ThicknessRight, int ThicknessTop, int ThicknessBottom) {
	_borderThicknessLeft = ThicknessLeft;
	_borderThicknessRight = ThicknessRight;
	_borderThicknessTop = ThicknessTop;
	_borderThicknessBottom = ThicknessBottom;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CPartForce *CPartEmitter::AddForceByName(char *Name) {
	CPartForce *Force = NULL;

	for (int i = 0; i < _forces.GetSize(); i++) {
		if (scumm_stricmp(Name, _forces[i]->_name) == 0) {
			Force = _forces[i];
			break;
		}
	}
	if (!Force) {
		Force = new CPartForce(Game);
		if (Force) {
			Force->SetName(Name);
			_forces.Add(Force);
		}
	}
	return Force;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::AddForce(char *Name, CPartForce::TForceType Type, int PosX, int PosY, float Angle, float Strength) {
	CPartForce *Force = AddForceByName(Name);
	if (!Force) return E_FAIL;

	Force->_type = Type;
	Force->_pos = Vector2(PosX, PosY);

	Force->_direction = Vector2(0, Strength);
	Matrix4 MatRot;
	MatRot.RotationZ(DegToRad(CBUtils::NormalizeAngle(Angle - 180)));
	MatRot.TransformVector2(Force->_direction);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::RemoveForce(char *Name) {
	for (int i = 0; i < _forces.GetSize(); i++) {
		if (scumm_stricmp(Name, _forces[i]->_name) == 0) {
			delete _forces[i];
			_forces.RemoveAt(i);
			return S_OK;
		}
	}
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::ScCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, char *Name) {
	//////////////////////////////////////////////////////////////////////////
	// SetBorder
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "SetBorder") == 0) {
		Stack->CorrectParams(4);
		int BorderX      = Stack->Pop()->GetInt();
		int BorderY      = Stack->Pop()->GetInt();
		int BorderWidth  = Stack->Pop()->GetInt();
		int BorderHeight = Stack->Pop()->GetInt();

		Stack->PushBool(SUCCEEDED(SetBorder(BorderX, BorderY, BorderWidth, BorderHeight)));

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// SetBorderThickness
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "SetBorderThickness") == 0) {
		Stack->CorrectParams(4);
		int Left   = Stack->Pop()->GetInt();
		int Right  = Stack->Pop()->GetInt();
		int Top    = Stack->Pop()->GetInt();
		int Bottom = Stack->Pop()->GetInt();

		Stack->PushBool(SUCCEEDED(SetBorderThickness(Left, Right, Top, Bottom)));

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// AddSprite
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddSprite") == 0) {
		Stack->CorrectParams(1);
		char *SpriteFile = Stack->Pop()->GetString();
		Stack->PushBool(SUCCEEDED(AddSprite(SpriteFile)));

		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// RemoveSprite
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "RemoveSprite") == 0) {
		Stack->CorrectParams(1);
		char *SpriteFile = Stack->Pop()->GetString();
		Stack->PushBool(SUCCEEDED(RemoveSprite(SpriteFile)));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Start
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Start") == 0) {
		Stack->CorrectParams(1);
		_overheadTime = Stack->Pop()->GetInt();
		Stack->PushBool(SUCCEEDED(Start()));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Stop
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Stop") == 0) {
		Stack->CorrectParams(0);

		for (int i = 0; i < _particles.GetSize(); i++) {
			delete _particles[i];
		}
		_particles.RemoveAll();

		_running = false;
		Stack->PushBool(true);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Pause
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Pause") == 0) {
		Stack->CorrectParams(0);
		_running = false;
		Stack->PushBool(true);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Resume
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Resume") == 0) {
		Stack->CorrectParams(0);
		_running = true;
		Stack->PushBool(true);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddGlobalForce
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddGlobalForce") == 0) {
		Stack->CorrectParams(3);
		char *Name = Stack->Pop()->GetString();
		float Angle = Stack->Pop()->GetFloat();
		float Strength = Stack->Pop()->GetFloat();

		Stack->PushBool(SUCCEEDED(AddForce(Name, CPartForce::FORCE_GLOBAL, 0, 0, Angle, Strength)));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddPointForce
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddPointForce") == 0) {
		Stack->CorrectParams(5);
		char *Name = Stack->Pop()->GetString();
		int PosX = Stack->Pop()->GetInt();
		int PosY = Stack->Pop()->GetInt();
		float Angle = Stack->Pop()->GetFloat();
		float Strength = Stack->Pop()->GetFloat();

		Stack->PushBool(SUCCEEDED(AddForce(Name, CPartForce::FORCE_GLOBAL, PosX, PosY, Angle, Strength)));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// RemoveForce
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "RemoveForce") == 0) {
		Stack->CorrectParams(1);
		char *Name = Stack->Pop()->GetString();

		Stack->PushBool(SUCCEEDED(RemoveForce(Name)));

		return S_OK;
	}

	else return CBObject::ScCallMethod(Script, Stack, ThisStack, Name);
}

//////////////////////////////////////////////////////////////////////////
CScValue *CPartEmitter::ScGetProperty(char *Name) {
	_scValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Type") == 0) {
		_scValue->SetString("particle-emitter");
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// X
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "X") == 0) {
		_scValue->SetInt(_posX);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Y
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Y") == 0) {
		_scValue->SetInt(_posY);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Width
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Width") == 0) {
		_scValue->SetInt(_width);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Height
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Height") == 0) {
		_scValue->SetInt(_height);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Scale1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Scale1") == 0) {
		_scValue->SetFloat(_scale1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Scale2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Scale2") == 0) {
		_scValue->SetFloat(_scale2);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// ScaleZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ScaleZBased") == 0) {
		_scValue->SetBool(_scaleZBased);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Velocity1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Velocity1") == 0) {
		_scValue->SetFloat(_velocity1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Velocity2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Velocity2") == 0) {
		_scValue->SetFloat(_velocity2);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// VelocityZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "VelocityZBased") == 0) {
		_scValue->SetBool(_velocityZBased);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// LifeTime1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTime1") == 0) {
		_scValue->SetInt(_lifeTime1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// LifeTime2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTime2") == 0) {
		_scValue->SetInt(_lifeTime2);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// LifeTimeZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTimeZBased") == 0) {
		_scValue->SetBool(_lifeTimeZBased);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Angle1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Angle1") == 0) {
		_scValue->SetInt(_angle1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Angle2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Angle2") == 0) {
		_scValue->SetInt(_angle2);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// AngVelocity1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AngVelocity1") == 0) {
		_scValue->SetFloat(_angVelocity1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// AngVelocity2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AngVelocity2") == 0) {
		_scValue->SetFloat(_angVelocity2);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Rotation1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Rotation1") == 0) {
		_scValue->SetFloat(_rotation1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Rotation2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Rotation2") == 0) {
		_scValue->SetFloat(_rotation2);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Alpha1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Alpha1") == 0) {
		_scValue->SetInt(_alpha1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Alpha2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Alpha2") == 0) {
		_scValue->SetInt(_alpha2);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// AlphaTimeBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AlphaTimeBased") == 0) {
		_scValue->SetBool(_alphaTimeBased);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// MaxParticles
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MaxParticles") == 0) {
		_scValue->SetInt(_maxParticles);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// NumLiveParticles (RO)
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "NumLiveParticles") == 0) {
		int NumAlive = 0;
		for (int i = 0; i < _particles.GetSize(); i++) {
			if (_particles[i] && !_particles[i]->_isDead) NumAlive++;
		}
		_scValue->SetInt(NumAlive);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// GenerationInterval
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GenerationInterval") == 0) {
		_scValue->SetInt(_genInterval);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// GenerationAmount
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GenerationAmount") == 0) {
		_scValue->SetInt(_genAmount);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// MaxBatches
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MaxBatches") == 0) {
		_scValue->SetInt(_maxBatches);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// FadeInTime
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "FadeInTime") == 0) {
		_scValue->SetInt(_fadeInTime);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// FadeOutTime
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "FadeOutTime") == 0) {
		_scValue->SetInt(_fadeOutTime);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// GrowthRate1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GrowthRate1") == 0) {
		_scValue->SetFloat(_growthRate1);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// GrowthRate2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GrowthRate2") == 0) {
		_scValue->SetFloat(_growthRate2);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// ExponentialGrowth
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ExponentialGrowth") == 0) {
		_scValue->SetBool(_exponentialGrowth);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// UseRegion
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "UseRegion") == 0) {
		_scValue->SetBool(_useRegion);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// EmitEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "EmitEvent") == 0) {
		if (!_emitEvent) _scValue->SetNULL();
		else _scValue->SetString(_emitEvent);
		return _scValue;
	}

	else return CBObject::ScGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::ScSetProperty(char *Name, CScValue *Value) {
	//////////////////////////////////////////////////////////////////////////
	// X
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "X") == 0) {
		_posX = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Y
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Y") == 0) {
		_posY = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Width
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Width") == 0) {
		_width = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Height
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Height") == 0) {
		_height = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Scale1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Scale1") == 0) {
		_scale1 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Scale2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Scale2") == 0) {
		_scale2 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// ScaleZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ScaleZBased") == 0) {
		_scaleZBased = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Velocity1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Velocity1") == 0) {
		_velocity1 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Velocity2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Velocity2") == 0) {
		_velocity2 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// VelocityZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "VelocityZBased") == 0) {
		_velocityZBased = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// LifeTime1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTime1") == 0) {
		_lifeTime1 = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// LifeTime2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTime2") == 0) {
		_lifeTime2 = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// LifeTimeZBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "LifeTimeZBased") == 0) {
		_lifeTimeZBased = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Angle1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Angle1") == 0) {
		_angle1 = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Angle2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Angle2") == 0) {
		_angle2 = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AngVelocity1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AngVelocity1") == 0) {
		_angVelocity1 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// AngVelocity2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AngVelocity2") == 0) {
		_angVelocity2 = Value->GetFloat();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Rotation1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Rotation1") == 0) {
		_rotation1 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Rotation2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Rotation2") == 0) {
		_rotation2 = Value->GetFloat();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Alpha1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Alpha1") == 0) {
		_alpha1 = Value->GetInt();
		if (_alpha1 < 0) _alpha1 = 0;
		if (_alpha1 > 255) _alpha1 = 255;
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// Alpha2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Alpha2") == 0) {
		_alpha2 = Value->GetInt();
		if (_alpha2 < 0) _alpha2 = 0;
		if (_alpha2 > 255) _alpha2 = 255;
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// AlphaTimeBased
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AlphaTimeBased") == 0) {
		_alphaTimeBased = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// MaxParticles
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MaxParticles") == 0) {
		_maxParticles = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GenerationInterval
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GenerationInterval") == 0) {
		_genInterval = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GenerationAmount
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GenerationAmount") == 0) {
		_genAmount = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// MaxBatches
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "MaxBatches") == 0) {
		_maxBatches = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// FadeInTime
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "FadeInTime") == 0) {
		_fadeInTime = Value->GetInt();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// FadeOutTime
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "FadeOutTime") == 0) {
		_fadeOutTime = Value->GetInt();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GrowthRate1
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GrowthRate1") == 0) {
		_growthRate1 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// GrowthRate2
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GrowthRate2") == 0) {
		_growthRate2 = Value->GetFloat();
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	// ExponentialGrowth
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ExponentialGrowth") == 0) {
		_exponentialGrowth = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// UseRegion
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "UseRegion") == 0) {
		_useRegion = Value->GetBool();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// EmitEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "EmitEvent") == 0) {
		SAFE_DELETE_ARRAY(_emitEvent);
		if (!Value->IsNULL()) CBUtils::SetString(&_emitEvent, Value->GetString());
		return S_OK;
	}

	else return CBObject::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char *CPartEmitter::ScToString() {
	return "[particle emitter]";
}




//////////////////////////////////////////////////////////////////////////
HRESULT CPartEmitter::Persist(CBPersistMgr *PersistMgr) {
	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(_width));
	PersistMgr->Transfer(TMEMBER(_height));

	PersistMgr->Transfer(TMEMBER(_angle1));
	PersistMgr->Transfer(TMEMBER(_angle2));

	PersistMgr->Transfer(TMEMBER(_velocity1));
	PersistMgr->Transfer(TMEMBER(_velocity2));
	PersistMgr->Transfer(TMEMBER(_velocityZBased));

	PersistMgr->Transfer(TMEMBER(_scale1));
	PersistMgr->Transfer(TMEMBER(_scale2));
	PersistMgr->Transfer(TMEMBER(_scaleZBased));

	PersistMgr->Transfer(TMEMBER(_maxParticles));

	PersistMgr->Transfer(TMEMBER(_lifeTime1));
	PersistMgr->Transfer(TMEMBER(_lifeTime2));
	PersistMgr->Transfer(TMEMBER(_lifeTimeZBased));

	PersistMgr->Transfer(TMEMBER(_genInterval));
	PersistMgr->Transfer(TMEMBER(_genAmount));

	PersistMgr->Transfer(TMEMBER(_running));
	PersistMgr->Transfer(TMEMBER(_overheadTime));

	PersistMgr->Transfer(TMEMBER(_border));
	PersistMgr->Transfer(TMEMBER(_borderThicknessLeft));
	PersistMgr->Transfer(TMEMBER(_borderThicknessRight));
	PersistMgr->Transfer(TMEMBER(_borderThicknessTop));
	PersistMgr->Transfer(TMEMBER(_borderThicknessBottom));

	PersistMgr->Transfer(TMEMBER(_fadeInTime));
	PersistMgr->Transfer(TMEMBER(_fadeOutTime));

	PersistMgr->Transfer(TMEMBER(_alpha1));
	PersistMgr->Transfer(TMEMBER(_alpha2));
	PersistMgr->Transfer(TMEMBER(_alphaTimeBased));

	PersistMgr->Transfer(TMEMBER(_angVelocity1));
	PersistMgr->Transfer(TMEMBER(_angVelocity2));

	PersistMgr->Transfer(TMEMBER(_rotation1));
	PersistMgr->Transfer(TMEMBER(_rotation2));

	PersistMgr->Transfer(TMEMBER(_growthRate1));
	PersistMgr->Transfer(TMEMBER(_growthRate2));
	PersistMgr->Transfer(TMEMBER(_exponentialGrowth));

	PersistMgr->Transfer(TMEMBER(_useRegion));

	PersistMgr->Transfer(TMEMBER_INT(_maxBatches));
	PersistMgr->Transfer(TMEMBER_INT(_batchesGenerated));

	PersistMgr->Transfer(TMEMBER(_emitEvent));
	PersistMgr->Transfer(TMEMBER(_owner));


	_sprites.Persist(PersistMgr);

	int NumForces;
	if (PersistMgr->_saving) {
		NumForces = _forces.GetSize();
		PersistMgr->Transfer(TMEMBER(NumForces));
		for (int i = 0; i < _forces.GetSize(); i++) {
			_forces[i]->Persist(PersistMgr);
		}
	} else {
		PersistMgr->Transfer(TMEMBER(NumForces));
		for (int i = 0; i < NumForces; i++) {
			CPartForce *Force = new CPartForce(Game);
			Force->Persist(PersistMgr);
			_forces.Add(Force);
		}
	}

	int NumParticles;
	if (PersistMgr->_saving) {
		NumParticles = _particles.GetSize();
		PersistMgr->Transfer(TMEMBER(NumParticles));
		for (int i = 0; i < _particles.GetSize(); i++) {
			_particles[i]->Persist(PersistMgr);
		}
	} else {
		PersistMgr->Transfer(TMEMBER(NumParticles));
		for (int i = 0; i < NumParticles; i++) {
			CPartParticle *Particle = new CPartParticle(Game);
			Particle->Persist(PersistMgr);
			_particles.Add(Particle);
		}
	}

	return S_OK;
}

} // end of namespace WinterMute
