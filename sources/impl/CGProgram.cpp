/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "impl/CGProgram.h"
#include "impl/SDLTexture.h"
#include "system/LowLevelSystem.h"

#include "system/String.h"

namespace hpl{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cCGProgram::cCGProgram(tString asName,CGcontext aContext,eGpuProgramType aType)
	: iGpuProgram(asName, aType)
	{
	}

	cCGProgram::~cCGProgram()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cCGProgram::Reload()
	{
		return false;
	}

	//-----------------------------------------------------------------------

	void cCGProgram::Unload()
	{
	}

	//-----------------------------------------------------------------------

	void cCGProgram::Destroy()
	{
	}

	//-----------------------------------------------------------------------

	bool cCGProgram::CreateFromFile(const tString &asFile, const tString &asEntry)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	void cCGProgram::Bind()
	{
	}

	//-----------------------------------------------------------------------

	void cCGProgram::UnBind()
	{
	}

	//-----------------------------------------------------------------------

	bool  cCGProgram::SetFloat(const tString& asName, float afX)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cCGProgram::SetVec2f(const tString& asName, float afX,float afY)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cCGProgram::SetVec3f(const tString& asName, float afX,float afY,float afZ)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool  cCGProgram::SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGProgram::SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
		eGpuProgramMatrixOp mOp)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGProgram::SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cCGProgram::SetTextureToUnit(int alUnit, iTexture* apTexture)
	{
		return true;
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	CGparameter cCGProgram::GetParam(const tString& asName,CGtype aType)
	{
        return NULL;
	}

	//-----------------------------------------------------------------------

}
