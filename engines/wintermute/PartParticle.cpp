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
#include "engines/wintermute/PartParticle.h"
#include "engines/wintermute/PartEmitter.h"
#include "engines/wintermute/BSprite.h"
#include "engines/wintermute/BGame.h"
#include "engines/wintermute/utils.h"
#include "engines/wintermute/PlatformSDL.h"
#include "common/str.h"
#include <math.h>

namespace WinterMute {

//////////////////////////////////////////////////////////////////////////
CPartParticle::CPartParticle(CBGame *inGame) : CBBase(inGame) {
	_pos = Vector2(0.0f, 0.0f);
	_posZ = 0.0f;
	_velocity = Vector2(0.0f, 0.0f);
	_scale = 100.0f;
	_sprite = NULL;
	_creationTime = 0;
	_lifeTime = 0;
	_isDead = true;
	CBPlatform::SetRectEmpty(&_border);

	_state = PARTICLE_NORMAL;
	_fadeStart = 0;
	_fadeTime = 0;
	_currentAlpha = 255;

	_alpha1 = _alpha2 = 255;

	_rotation = 0.0f;
	_angVelocity = 0.0f;

	_growthRate = 0.0f;
	_exponentialGrowth = false;
}


//////////////////////////////////////////////////////////////////////////
CPartParticle::~CPartParticle(void) {
	delete _sprite;
	_sprite = NULL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::SetSprite(char *Filename) {
	if (_sprite && _sprite->_filename && scumm_stricmp(Filename, _sprite->_filename) == 0) {
		_sprite->Reset();
		return S_OK;
	}

	delete _sprite;
	_sprite = NULL;

	CSysClassRegistry::GetInstance()->_disabled = true;
	_sprite = new CBSprite(Game, Game);
	if (_sprite && SUCCEEDED(_sprite->LoadFile(Filename))) {
		CSysClassRegistry::GetInstance()->_disabled = false;
		return S_OK;
	} else {
		delete _sprite;
		_sprite = NULL;
		CSysClassRegistry::GetInstance()->_disabled = false;
		return E_FAIL;
	}

}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::Update(CPartEmitter *Emitter, uint32 CurrentTime, uint32 TimerDelta) {
	if (_state == PARTICLE_FADEIN) {
		if (CurrentTime - _fadeStart >= _fadeTime) {
			_state = PARTICLE_NORMAL;
			_currentAlpha = _alpha1;
		} else _currentAlpha = ((float)CurrentTime - (float)_fadeStart) / (float)_fadeTime * _alpha1;

		return S_OK;
	} else if (_state == PARTICLE_FADEOUT) {
		if (CurrentTime - _fadeStart >= _fadeTime) {
			_isDead = true;
			return S_OK;
		} else _currentAlpha = _fadeStartAlpha - ((float)CurrentTime - (float)_fadeStart) / (float)_fadeTime * _fadeStartAlpha;

		return S_OK;
	} else {
		// time is up
		if (_lifeTime > 0) {
			if (CurrentTime - _creationTime >= _lifeTime) {
				if (Emitter->_fadeOutTime > 0)
					FadeOut(CurrentTime, Emitter->_fadeOutTime);
				else
					_isDead = true;
			}
		}

		// particle hit the border
		if (!_isDead && !CBPlatform::IsRectEmpty(&_border)) {
			POINT p;
			p.x = _pos.x;
			p.y = _pos.y;
			if (!CBPlatform::PtInRect(&_border, p)) FadeOut(CurrentTime, Emitter->_fadeOutTime);
		}
		if (_state != PARTICLE_NORMAL) return S_OK;

		// update alpha
		if (_lifeTime > 0) {
			int Age = CurrentTime - _creationTime;
			int AlphaDelta = _alpha2 - _alpha1;

			_currentAlpha = _alpha1 + ((float)AlphaDelta / (float)_lifeTime * (float)Age);
		}

		// update position
		float ElapsedTime = (float)TimerDelta / 1000.f;

		for (int i = 0; i < Emitter->_forces.GetSize(); i++) {
			CPartForce *Force = Emitter->_forces[i];
			switch (Force->_type) {
			case CPartForce::FORCE_GLOBAL:
				_velocity += Force->_direction * ElapsedTime;
				break;

			case CPartForce::FORCE_POINT: {
				Vector2 VecDist = Force->_pos - _pos;
				float Dist = fabs(VecDist.Length());

				Dist = 100.0f / Dist;

				_velocity += Force->_direction * Dist * ElapsedTime;
			}
			break;
			}
		}
		_pos += _velocity * ElapsedTime;

		// update rotation
		_rotation += _angVelocity * ElapsedTime;
		_rotation = CBUtils::NormalizeAngle(_rotation);

		// update scale
		if (_exponentialGrowth)
			_scale += _scale / 100.0f * _growthRate * ElapsedTime;
		else
			_scale += _growthRate * ElapsedTime;

		if (_scale <= 0.0f) _isDead = true;


		return S_OK;
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::Display(CPartEmitter *Emitter) {
	if (!_sprite) return E_FAIL;
	if (_isDead) return S_OK;

	_sprite->GetCurrentFrame();
	return _sprite->Display(_pos.x, _pos.y,
	                         NULL,
	                         _scale, _scale,
	                         DRGBA(255, 255, 255, _currentAlpha),
	                         _rotation,
	                         Emitter->_blendMode);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::FadeIn(uint32 CurrentTime, int FadeTime) {
	_currentAlpha = 0;
	_fadeStart = CurrentTime;
	_fadeTime = FadeTime;
	_state = PARTICLE_FADEIN;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::FadeOut(uint32 CurrentTime, int FadeTime) {
	//_currentAlpha = 255;
	_fadeStartAlpha = _currentAlpha;
	_fadeStart = CurrentTime;
	_fadeTime = FadeTime;
	_state = PARTICLE_FADEOUT;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CPartParticle::Persist(CBPersistMgr *PersistMgr) {
	PersistMgr->Transfer(TMEMBER(_alpha1));
	PersistMgr->Transfer(TMEMBER(_alpha2));
	PersistMgr->Transfer(TMEMBER(_border));
	PersistMgr->Transfer(TMEMBER(_pos));
	PersistMgr->Transfer(TMEMBER(_posZ));
	PersistMgr->Transfer(TMEMBER(_velocity));
	PersistMgr->Transfer(TMEMBER(_scale));
	PersistMgr->Transfer(TMEMBER(_creationTime));
	PersistMgr->Transfer(TMEMBER(_lifeTime));
	PersistMgr->Transfer(TMEMBER(_isDead));
	PersistMgr->Transfer(TMEMBER_INT(_state));
	PersistMgr->Transfer(TMEMBER(_fadeStart));
	PersistMgr->Transfer(TMEMBER(_fadeTime));
	PersistMgr->Transfer(TMEMBER(_currentAlpha));
	PersistMgr->Transfer(TMEMBER(_angVelocity));
	PersistMgr->Transfer(TMEMBER(_rotation));
	PersistMgr->Transfer(TMEMBER(_growthRate));
	PersistMgr->Transfer(TMEMBER(_exponentialGrowth));
	PersistMgr->Transfer(TMEMBER(_fadeStartAlpha));

	if (PersistMgr->_saving) {
		PersistMgr->Transfer(TMEMBER(_sprite->_filename));
	} else {
		char *Filename;
		PersistMgr->Transfer(TMEMBER(Filename));
		CSysClassRegistry::GetInstance()->_disabled = true;
		SetSprite(Filename);
		CSysClassRegistry::GetInstance()->_disabled = false;
		delete[] Filename;
		Filename = NULL;
	}

	return S_OK;
}

} // end of namespace WinterMute
