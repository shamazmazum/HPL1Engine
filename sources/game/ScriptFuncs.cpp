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
#include "game/ScriptFuncs.h"

#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "game/Game.h"
#include "graphics/Graphics.h"
#include "sound/Sound.h"
#include "resources/Resources.h"
#include "math/Math.h"
#include "scene/Scene.h"
#include "scene/Light3D.h"
#include "scene/World3D.h"
#include "scene/SoundEntity.h"
#include "scene/PortalContainer.h"
#include "input/Input.h"
#include "system/System.h"
#include "system/LowLevelSystem.h"
#include "sound/MusicHandler.h"
#include "resources/SoundManager.h"
#include "resources/SoundEntityManager.h"
#include "resources/TextureManager.h"
#include "sound/SoundData.h"
#include "sound/SoundChannel.h"
#include "sound/SoundHandler.h"
#include "sound/SoundEntityData.h"
#include "system/String.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointScrew.h"
#include "physics/PhysicsJointSlider.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsController.h"
#include "system/Script.h"
#include "graphics/ParticleSystem3D.h"
#include "scene/MeshEntity.h"
#include "graphics/BillBoard.h"
#include "graphics/Beam.h"
#include "graphics/Renderer3D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// JOINT CALLBACK
	//////////////////////////////////////////////////////////////////////////

	cScriptJointCallback::cScriptJointCallback(cScene *apScene)
	{
		mpScene = apScene;

		msMaxFunc = "";
		msMinFunc = "";
	}

	void cScriptJointCallback::OnMinLimit(iPhysicsJoint *apJoint)
	{
		if(msMinFunc!="")
		{
			iScript *pScript = mpScene->GetWorld3D()->GetScript();

			tString sCommand = msMinFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}

		}
	}

	void cScriptJointCallback::OnMaxLimit(iPhysicsJoint *apJoint)
	{
		if(msMaxFunc!="")
		{
			iScript *pScript = mpScene->GetWorld3D()->GetScript();

			tString sCommand = msMaxFunc + "(\"" + apJoint->GetName() + "\")";
			if(pScript->Run(sCommand)==false){
				Warning("Couldn't run script command '%s'\n",sCommand.c_str());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	static cResources *gpResources=NULL;
	static cSystem *gpSystem=NULL;
	static cInput *gpInput=NULL;
	static cGraphics *gpGraphics=NULL;
	static cScene *gpScene=NULL;
	static cSound *gpSound=NULL;
	static cGame *gpGame=NULL;

	//-----------------------------------------------------------------------

	static void __stdcall Print(std::string asText)
	{
		Log(asText.c_str());
	}

	static std::string __stdcall FloatToString(float afX)
	{
		char sTemp[30];
		sprintf(sTemp,"%f",afX);
		return (std::string) sTemp;
	}

	static std::string __stdcall IntToString(int alX)
	{
		char sTemp[30];
		sprintf(sTemp,"%d",alX);
		return (std::string) sTemp;
	}

	static float __stdcall RandFloat(float afMin, float afMax)
	{
		return cMath::RandRectf(afMin,afMax);
	}

	static int __stdcall RandInt(int alMin, int alMax)
	{
		return cMath::RandRectl(alMin,alMax);
	}

	static bool __stdcall StringContains(std::string asString, std::string asSubString)
	{
		return cString::GetLastStringPos(asString,asSubString)>=0;
	}

	static void __stdcall ResetLogicTimer()
	{
		gpGame->ResetLogicTimer();
	}

	/////////////////////////////////////////////////////////////////////////
	/////// RENDERER //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Sets ambient light color-
	**/
	static void __stdcall SetAmbientColor(float afR, float afG, float afB)
	{
		gpGraphics->GetRenderer3D()->SetAmbientColor(cColor(afR,afG,afB,1.0f));
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if the skybox should be active
	**/
	static void __stdcall SetSkyboxActive(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetSkyBoxActive(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets the skybox color.
	**/
	static void __stdcall SetSkyboxColor(float afR, float afG, float afB, float afA)
	{
		gpGraphics->GetRenderer3D()->SetSkyBoxColor(cColor(afR,afG,afB,afA));
	}

	//-----------------------------------------------------------------------


	/**
	* Sets the skybox
	* \param asTexture Name of the cube map texture to use
	**/
	static void __stdcall SetSkybox(std::string asTexture)
	{
		if(asTexture!="")
		{
			iTexture *pTex = gpResources->GetTextureManager()->CreateCubeMap(asTexture,false);
			gpGraphics->GetRenderer3D()->SetSkyBox(pTex,true);
		}
		else
		{
			gpGraphics->GetRenderer3D()->SetSkyBox(NULL,false);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Creates a particle system and attaches it to the camera.
	* \param asName Name of particle system
	* \param asType The type of particle system (file)
	**/
	static void __stdcall CreateParticleSystemOnCamera(std::string asName,std::string asType)
	{
		cParticleSystem3D *pPS = gpScene->GetWorld3D()->CreateParticleSystem(asName,asType,
																			1,cMatrixf::Identity);
		if(pPS)
		{
			cCamera3D *pCam = static_cast<cCamera3D*>(gpScene->GetCamera());
			pCam->AttachEntity(pPS);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if fog should be active
	* \param abX If the fog is active or not.
	**/
	static void __stdcall SetFogActive(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetFogActive(abX);
	}

	/**
	* Sets if the fog should be used to cull non-visible objects
	* \param abX If the culling is active or not.
	**/
	static void __stdcall SetFogCulling(bool abX)
	{
		gpGraphics->GetRenderer3D()->SetFogCulling(abX);
	}

	/**
	* Creates a particle system and attaches it to the camera.
	* \param afStart Start of fog color
	* \param afStart End of fog fade. After this limit all geometry is full fog color.
	* \param afR, afG, afB Color of Fog.
	**/
	static void __stdcall SetFogProperties(float afStart, float afEnd, float afR,float afG, float afB)
	{
		gpGraphics->GetRenderer3D()->SetFogStart(afStart);
		gpGraphics->GetRenderer3D()->SetFogEnd(afEnd);
		gpGraphics->GetRenderer3D()->SetFogColor(cColor(afR,afG,afB,1.0f));
	}

	//-----------------------------------------------------------------------

	static void __stdcall SetSectorProperties(std::string asSector, float afAmbR,float afAmbG, float afAmbB)
	{
		cPortalContainer *pContainer = gpScene->GetWorld3D()->GetPortalContainer();

		cSector *pSector = pContainer->GetSector(asSector);
		if(pSector == NULL){
			Warning("Could not find sector '%s'\n", asSector.c_str());
			return;
		}

		pSector->SetAmbientColor(cColor(afAmbR, afAmbG, afAmbB,1));
	}

	//-----------------------------------------------------------------------

	static void __stdcall SetSectorPortalActive(std::string asSector, int alPortal, bool abActive)
	{
		cPortalContainer *pContainer = gpScene->GetWorld3D()->GetPortalContainer();

		cSector *pSector = pContainer->GetSector(asSector);
		if(pSector == NULL){
			Warning("Could not find sector '%s'\n", asSector.c_str());
			return;
		}

		cPortal *pPortal = pSector->GetPortal(alPortal);
		if(pPortal==NULL)
		{
			Warning("Could not find portal %d in sector '%s'\n",alPortal,asSector.c_str());
			return;
		}

		pPortal->SetActive(abActive);
	}

	/////////////////////////////////////////////////////////////////////////
	/////// RESOURCES //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	/**
	 * Gets a string in the current language.
	 * \param asCat The translation category
	 * \param asName The name of the category entry.
	 **/
	static std::string __stdcall Translate(std::string asCat, std::string asName)
	{
		tWString wsText = gpResources->Translate(asCat,asName);
		return cString::To8Char(wsText);
	}

	//-----------------------------------------------------------------------

	/**
	* Preloads the data for a sound.
	* \param asFile This can be a wav, ogg, mp3 or snt file.
	**/
	static void __stdcall PreloadSound(std::string asFile)
	{
		tString sExt = cString::ToLowerCase(cString::GetFileExt(asFile));
		if(sExt=="snt")
		{
			cSoundEntityData *pData = gpResources->GetSoundEntityManager()->CreateSoundEntity(asFile);
			if(pData == NULL) {
				Warning("Couldn't preload sound '%s'\n",asFile.c_str());
				return;
			}

			if(pData->GetMainSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetMainSoundName(),0);
				hplDelete(pChannel);
			}
			if(pData->GetStartSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetStartSoundName(),0);
				hplDelete(pChannel);
			}
			if(pData->GetStopSoundName() != ""){
				iSoundChannel *pChannel = gpSound->GetSoundHandler()->CreateChannel(pData->GetStopSoundName(),0);
				hplDelete(pChannel);
			}
		}
		else
		{
			iSoundData *pSound = gpResources->GetSoundManager()->CreateSoundData(asFile,false);
			if(pSound){
				Warning("Couldn't preload sound '%s'\n",asFile.c_str());
			}
		}
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// MESH ENTITY //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static void __stdcall SetMeshActive(std::string asName, bool abActive)
	{
		cMeshEntity *pEnt = gpScene->GetWorld3D()->GetMeshEntity(asName);
		if(pEnt==NULL){
			Warning("Didn't find mesh entity '%s'\n",asName.c_str());
			return;
		}

		pEnt->SetActive(abActive);
		pEnt->SetVisible(abActive);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// PARTICLE SYSTEM //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Create a particle system at the position of an area
	* \param asName The name of the particle system.
	* \param abActive If it should be active or not.
	**/
	static void __stdcall SetParticleSystemActive(std::string asName, bool abActive)
	{
		cParticleSystem3D *pPS = gpScene->GetWorld3D()->GetParticleSystem(asName);
		if(pPS==NULL){
			Warning("Didn't find particle system '%s'\n",asName.c_str());
			return;
		}

		pPS->SetActive(abActive);
		pPS->SetVisible(abActive);
	}

	//-----------------------------------------------------------------------

	/**
	* Create a particle system at the position of an area
	* \param asName The name of the particle system.
	* \param asType The type of aprticle system
	* \param asArea The name of the area
	* \param X Y and Z the variables of the particle system.
	**/
	static void __stdcall CreateParticleSystem(std::string asName, std::string asType, std::string asArea,
												float afX, float afY, float afZ)
	{
		cAreaEntity* pArea = gpScene->GetWorld3D()->GetAreaEntity(asArea);
		if(pArea==NULL){
			Warning("Couldn't find area '%s'\n",asArea.c_str());
			return;
		}

		cParticleSystem3D *pPS = gpScene->GetWorld3D()->CreateParticleSystem(asName,asType,
														cVector3f(afX,afY,afZ),pArea->m_mtxTransform);
		if(pPS==NULL){
			Warning("No particle system of type '%s'\n",asType.c_str());
			return;
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Kill a particle system
	* \param asName The name of the particle system.
	**/
	static void __stdcall KillParticleSystem(std::string asName)
	{
		/*cParticleSystem3D *pPS = gpScene->GetWorld3D()->GetParticleSystem(asName);
		if(pPS==NULL){
			Warning("Didn't find particle system '%s'\n",asName.c_str());
			return;
		}
		pPS->Kill();*/

		bool bFound = false;
		cParticleSystem3DIterator it = gpScene->GetWorld3D()->GetParticleSystemIterator();
		while(it.HasNext())
		{
			cParticleSystem3D *pPS = it.Next();

			if(pPS->GetName() == asName)
			{
				pPS->Kill();
				bFound = true;
			}
		}

		if(!bFound) Warning("Didn't find particle system '%s'\n",asName.c_str());
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// BEAM //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	/**
	 * Creates an a beam between two areas
	 * \param asName
	 * \param asFile
	 * \param asStartArea
	 * \param asEndArea
	 */
	static void __stdcall CreateBeam(std::string asName, std::string asFile,
									std::string asStartArea, std::string asEndArea)
	{
		cAreaEntity* pStartArea = gpScene->GetWorld3D()->GetAreaEntity(asStartArea);
		if(pStartArea==NULL){
			Warning("Couldn't find area '%s'\n",asStartArea.c_str());
			return;
		}

		cAreaEntity* pEndArea = gpScene->GetWorld3D()->GetAreaEntity(asEndArea);
		if(pEndArea==NULL){
			Warning("Couldn't find area '%s'\n",asEndArea.c_str());
			return;
		}

		cBeam *pBeam = gpScene->GetWorld3D()->CreateBeam(asName);

		if(pBeam->LoadXMLProperties(asFile)==false)
		{
			Error("Couldn't create beam from file '%s'\n",asFile.c_str());
			gpScene->GetWorld3D()->DestroyBeam(pBeam);
			return;
		}

		pBeam->SetPosition(pStartArea->m_mtxTransform.GetTranslation());
		pBeam->GetEnd()->SetPosition(pEndArea->m_mtxTransform.GetTranslation());
	}

	//-----------------------------------------------------------------------

	/**
	 * Destroys a beam
	 * \param asName
	 */
	static void __stdcall DestroyBeam(std::string asName)
	{
		cBeam* pBeam = gpScene->GetWorld3D()->GetBeam(asName);
		if(pBeam==NULL)
		{
			Warning("Couldn't find beam '%s'\n",asName.c_str());
			return;
		}

		gpScene->GetWorld3D()->DestroyBeam(pBeam);
	}

	//-----------------------------------------------------------------------



	/////////////////////////////////////////////////////////////////////////
	/////// LIGHT //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Fades a sound to a color and a radius
	* \param asName The name of the light
	* \param afR The red channel to fade to.
	* \param afG The green channel to fade to.
	* \param afB The blue channel to fade to.
	* \param afA The alpha channel to fade to.
	* \param afRadius The radius to fade to.
	* \param afTime The amount of seconds the fade should last.
	**/
	static void __stdcall FadeLight3D(std::string asName, float afR,float afG, float afB, float afA,
										float afRadius, float afTime)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->FadeTo(cColor(afR,afG,afB,afA),afRadius, afTime);
		pLight->SetVisible(true);
		pLight->UpdateLight(2.0f/60.0f);
	}

	//-----------------------------------------------------------------------

	/**
	* Attaches a billboard to a light
	* \param asBillboardName The billbaord name
	* \param asLightName The light name
	* \param abX True if it should be attached, false if you want to remove.
	**/
	static void __stdcall AttachBillboardToLight3D(std::string asBillboardName, std::string asLightName,bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asLightName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asLightName.c_str());
			return;
		}

		cBillboard *pBillboard = gpScene->GetWorld3D()->GetBillboard(asBillboardName);
		if(pBillboard==NULL)
		{
			Warning("Couldn't find billboard '%s'\n",asBillboardName.c_str());
			return;
		}

		if(abX)
			pLight->AttachBillboard(pBillboard);
		else
			pLight->RemoveBillboard(pBillboard);
	}

	//-----------------------------------------------------------------------


	/**
	* Sets on/off a light
	* \param asName The light name
	* \param abX if the light should be on or off.
	**/
	static void __stdcall SetLight3DVisible(std::string asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetVisible(abX);
	}

		//-----------------------------------------------------------------------

	/**
	* Sets on/off for affect only in sector where centre is.
	* \param asName The light name
	* \param abX if the light should only affects objects in same sector or not.
	**/
	static void __stdcall SetLight3DOnlyAffectInSector(std::string asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetOnlyAffectInSector(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets flickering on/off a light
	* \param asName The light name
	* \param abX if the light flicker should be on or off.
	**/
	static void __stdcall SetLight3DFlickerActive(std::string asName, bool abX)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetFlickerActive(abX);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets flickering parameters
	* \param asName The light name
	* \param abR, afG, afB, afA The color of the light when off
	* \param afRadius The radius of the light when off.
	* \param afOnMinLength Minimum time before going from off to on.
	* \param afOnMaxLength Maximum time before going from off to on.
	* \param asOnSound Name of the sound played when going from off to on. "" means no sound.
	* \param asOnPS Name of the particle system played when going from off to on. "" means none.
	* \param afOffMinLength Minimum time before going from on to off.
	* \param afOffMaxLength Maximum time before going from on to off.
	* \param asOffSound Name of the sound played when going from on to off. "" means no sound.
	* \param asOffPS Name of the particle system played when going from on to off. "" means none.
	* \param abFade If there should be a fade between off and on.
	* \param afOnFadeLength Fade length from off to on.
	* \param afOffFadeLength Fade length from on to off.
	**/
	static void __stdcall SetLight3DFlicker(std::string asName,
											float afR,float afG, float afB, float afA,
											float afRadius,

											float afOnMinLength, float afOnMaxLength,
											std::string asOnSound,std::string asOnPS,

											float afOffMinLength, float afOffMaxLength,
											std::string asOffSound,std::string asOffPS,

											bool abFade,
											float afOnFadeLength, float afOffFadeLength)
	{
		iLight3D *pLight = gpScene->GetWorld3D()->GetLight(asName);
		if(pLight==NULL)
		{
			Warning("Couldn't find light '%s'\n",asName.c_str());
			return;
		}

		pLight->SetFlicker(cColor(afR,afG,afB,afA),afRadius,
							afOnMinLength, afOnMaxLength,asOnSound,asOnPS,
							afOffMinLength, afOffMaxLength,asOffSound,asOffPS,
							abFade,afOnFadeLength,afOffFadeLength);
	}

	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	/////// SOUND //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	/**
	 * Creates a sound entity at the postion of an area.
	 * \param asName Name of the sound area
	 * \param asFile The snt file to load.
	 * \param asArea The area to create at.
	 */
	static void __stdcall CreateSoundEntity(std::string asName, std::string asFile,
												std::string asArea)
	{
		cAreaEntity* pArea = gpScene->GetWorld3D()->GetAreaEntity(asArea);
		if(pArea==NULL){
			Warning("Couldn't find area '%s'\n",asArea.c_str());
			return;
		}

		cSoundEntity *pSound = gpScene->GetWorld3D()->CreateSoundEntity(asName,asFile,true);
		if(pSound==NULL)
		{
			Warning("Couldn't create sound entity '%s'\n",asFile.c_str());
			return;
		}

		pSound->SetPosition(pArea->m_mtxTransform.GetTranslation());
	}
	//-----------------------------------------------------------------------

	/**
	* Play a sound entity
	* \param asName The entity name
	* \param abPlayStart If the start sound should be played.
	**/
	static void __stdcall PlaySoundEntity(std::string asName, bool abPlayStart)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->Play(abPlayStart);
	}

	/**
	* Stop a sound entity
	* \param asName The entity name
	* \param abPlayEnd If the end sound should be played.
	**/
	static void __stdcall StopSoundEntity(std::string asName, bool abPlayEnd)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->Stop(abPlayEnd);
	}

	//-----------------------------------------------------------------------

	/**
	* Play a sound entity fading it
	* \param asName The entity name
	* \param afSpeed Volume increase per second.
	**/
	static void __stdcall FadeInSoundEntity(std::string asName, float afSpeed)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->FadeIn(afSpeed);
	}

	/**
	* Stop a sound entity fading it
	* \param asName The entity name
	* \param afSpeed Volume decrease per second.
	**/
	static void __stdcall FadeOutSoundEntity(std::string asName, float afSpeed)
	{
		cSoundEntity *pSound = gpScene->GetWorld3D()->GetSoundEntity(asName);
		if(pSound==NULL)
		{
			Warning("Couldn't find sound entity '%s'\n",asName.c_str());
			return;
		}

		pSound->FadeOut(afSpeed);
	}

	//-----------------------------------------------------------------------

	static void __stdcall PlayMusic(std::string asName, float afVol, float afStepSize, bool abLoop)
	{
		gpSound->GetMusicHandler()->Play(asName,afVol,afStepSize,abLoop);
	}

	//-----------------------------------------------------------------------

	static void __stdcall StopMusic(float afStepSize)
	{
		gpSound->GetMusicHandler()->Stop(afStepSize);
	}

	//-----------------------------------------------------------------------

	/**
	* Play a sound gui sound, with out any position.
	* \param asName The sound name
	* \param afVol Volume of the sound
	**/
	static void __stdcall PlayGuiSound(std::string asName, float afVol)
	{
		gpSound->GetSoundHandler()->PlayGui(asName,false,afVol);
	}

	/////////////////////////////////////////////////////////////////////////
	/////// PHYSICS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	* Sets a callback for a joint.
	* The syntax for the function is: void MyFunction(string asJointName)
	* \param asJointName The joint name
	* \param asType The type, can be: "OnMax" or "OnMin".
	* \param asFunc The script function to be called. Must be in the current script file. "" = disabled.
	**/
	static void __stdcall SetJointCallback(std::string asJointName, std::string asType,
											std::string asFunc)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		int lType = 0;
		tString sLowName = cString::ToLowerCase(asType);
		if(sLowName == "onmax") lType = 1;
		if(sLowName == "onmin") lType = 2;

		if(lType ==0){
			Warning("Joint callback type '%s' does not exist\n",asType.c_str()); return;
		}

		cScriptJointCallback *pCallback = static_cast<cScriptJointCallback*>(pJoint->GetCallback());
		if(pCallback==NULL)
		{
			pCallback = hplNew( cScriptJointCallback, (gpScene) );
			pJoint->SetCallback(pCallback,true);
		}

		if(lType==1) pCallback->msMaxFunc = asFunc;
		if(lType==2) pCallback->msMinFunc = asFunc;
	}

	//-----------------------------------------------------------------------

	/**
	* Breaks a joint.
	* \param asJointName The joint name
	**/
	static void __stdcall BreakJoint(std::string asJointName)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		pJoint->Break();
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if a joint controller is active or not.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	* \param abActive If the controller is to be active or not.
	**/
	static void __stdcall SetJointControllerActive(std::string asJointName,std::string asCtrlName, bool abActive)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		iPhysicsController *pCtrl = pJoint->GetController(asCtrlName);
		if(pCtrl==NULL){
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());return;
		}

		pCtrl->SetActive(abActive);
	}

	//-----------------------------------------------------------------------

	/**
	* Change the active controller. All other controllers are set to false.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	**/
	static void __stdcall ChangeJointController(std::string asJointName,std::string asCtrlName)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		if(pJoint->ChangeController(asCtrlName)==false)
		{
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());
			return;
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Sets if a joint controller is active or not.
	* \param asJointName The joint name
	* \param asCtrlName The controller name
	* \param asProperty Property to change, can be "DestValue"
	* \param afValue Value to set it to.
	**/
	static void __stdcall SetJointControllerPropertyFloat(std::string asJointName,std::string asCtrlName,
													std::string asProperty, float afValue)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		iPhysicsController *pCtrl = pJoint->GetController(asCtrlName);
		if(pCtrl==NULL){
			Warning("Couldn't find controller %s in joint '%s'\n",asCtrlName.c_str(),asJointName.c_str());return;
		}

		if(asProperty == "DestValue")
		{
			pCtrl->SetDestValue(afValue);
		}
	}

	//-----------------------------------------------------------------------


	/**
	* Gets a property from the joint.
	* Valid properties are:
	* "Angle" The angle between the bodies (in degrees) (Not working for ball joint)
	* "Distance" The distance between the bodies (in meter)
	* "LinearSpeed" The relative linear speed between the bodies (in m/s)
	* "AngularSpeed" The relative angular speed between the bodies (in m/s)
	* "Force" The size of the force (in newton, kg*m/s^2).
	* "MaxLimit" The max limit (meters or degrees)
	* "MinLimit" The in limit (meters or degrees)
	* \param asJointName The joint name
	* \param asProp The property to get
	**/
	static float __stdcall GetJointProperty(std::string asJointName, std::string asProp)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return 0;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "angle")
		{
			return cMath::ToDeg(pJoint->GetAngle());
		}
		else if(sLowProp == "distance")
		{
			return pJoint->GetDistance();
		}
		else if(sLowProp == "linearspeed")
		{
			return pJoint->GetVelocity().Length();
		}
		else if(sLowProp == "angularspeed")
		{
			return pJoint->GetAngularVelocity().Length();
		}
		else if(sLowProp == "force")
		{
			return pJoint->GetForce().Length();
		}
		/////////////////////////////
		// Min Limit
		else if(sLowProp == "minlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					return cMath::ToDeg(pHingeJoint->GetMinAngle());
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					return pScrewJoint->GetMinDistance();
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					return pSliderJoint->GetMinDistance();
				}
			}
		}
		/////////////////////////////
		// Max Limit
		else if(sLowProp == "maxlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					return cMath::ToDeg(pHingeJoint->GetMaxAngle());
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					return pScrewJoint->GetMaxDistance();
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					return pSliderJoint->GetMaxDistance();
				}
			}

		}

		Warning("Joint property '%s' does not exist!\n",asProp.c_str());
		return 0;
	}

	//-----------------------------------------------------------------------

	/**
	* Gets a property from the body.
	* Valid properties are:
	* "Mass" The mass of the body (in kg)
	* "LinearSpeed" The linear speed the body has (in m/s)
	* "AngularSpeed" The angular speed the body has (in m/s)
	* \param asBodyName The body name
	* \param asProp The property to get
	**/
	static float __stdcall GetBodyProperty(std::string asBodyName, std::string asProp)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return 0;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "mass")
		{
			return pBody->GetMass();
		}
		else if(sLowProp == "linearspeed")
		{
			return pBody->GetLinearVelocity().Length();
		}
		else if(sLowProp == "angularspeed")
		{
			return pBody->GetAngularVelocity().Length();
		}

		Warning("Body property '%s' does not exist!\n",asProp.c_str());
		return 0;
	}

	//-----------------------------------------------------------------------

	/**
	* Sets a property to the body.
	* Valid properties are:
	* "Mass" The mass of the body (in kg)
	* "CollideCharacter"	0 = false 1=true
	* \param asBodyName The body name
	* \param asProp The property to get
	* \param afVal The new value of the property
	**/
	static void __stdcall SetBodyProperty(std::string asBodyName, std::string asProp, float afVal)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(sLowProp == "mass")
		{
			pBody->SetMass(afVal);
			pBody->SetEnabled(true);

			if(afVal == 0)
			{
				pBody->SetLinearVelocity(0);
				pBody->SetAngularVelocity(0);
			}

			return;
		}
		else if(sLowProp == "collidecharacter")
		{
			pBody->SetCollideCharacter(afVal <0.05 ? false : true);
			return;
		}
		else if(sLowProp == "hasgravity")
		{
			pBody->SetCollideCharacter(afVal <0.05 ? false : true);
			return;
		}


		Warning("Body property '%s' does not exist!\n",asProp.c_str());
	}

	//-----------------------------------------------------------------------

	static void __stdcall AttachBodiesWithJoint(std::string asParentName, std::string asChildName, std::string asJointName)
	{
		iPhysicsBody *pParent = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asParentName);
		if(pParent==NULL){
			Warning("Couldn't find Body '%s'\n",asParentName.c_str());return;
		}

		iPhysicsBody *pChild = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asChildName);
		if(pChild==NULL){
			Warning("Couldn't find Body '%s'\n",asChildName.c_str());return;
		}

		iPhysicsWorld *pPhysicsWorld = gpScene->GetWorld3D()->GetPhysicsWorld();

		cVector3f vPivot = (pParent->GetLocalPosition() + pChild->GetLocalPosition()) * 0.5f;
		cVector3f vDir = cMath::Vector3Normalize(pChild->GetLocalPosition() - pParent->GetLocalPosition());

		iPhysicsJointSlider *pJoint = pPhysicsWorld->CreateJointSlider(asJointName,vPivot,vDir,pParent,pChild);

		pJoint->SetMinDistance(-0.01f);
		pJoint->SetMaxDistance(0.01f);
	}

	//-----------------------------------------------------------------------

	/**
	* Sets a property to the joint.
	* Valid properties are:
	* "MinLimit" The min limit (depends on joint, does not work on ball)
	* "MaxLimit" The max limit (depends on joint, does not work on ball)
	* \param asJointName The body name
	* \param asProp The property to get
	* \param afVal The new value of the property
	**/
	static void __stdcall SetJointProperty(std::string asJointName, std::string asProp, float afVal)
	{
		iPhysicsJoint *pJoint = gpScene->GetWorld3D()->GetPhysicsWorld()->GetJoint(asJointName);
		if(pJoint==NULL){
			Warning("Couldn't find joint '%s'\n",asJointName.c_str());return;
		}

		tString sLowProp = cString::ToLowerCase(asProp);

		if(pJoint->GetChildBody()) pJoint->GetChildBody()->SetEnabled(true);
		if(pJoint->GetParentBody()) pJoint->GetParentBody()->SetEnabled(true);

		/////////////////////////////
		// Min Limit
		if(sLowProp == "minlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					pHingeJoint->SetMinAngle(cMath::ToRad(afVal));
					break;
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					pScrewJoint->SetMinDistance(afVal);
					break;
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					pSliderJoint->SetMinDistance(afVal);
					break;
				}
			}
		}
		/////////////////////////////
		// Max Limit
		else if(sLowProp == "maxlimit")
		{
			switch(pJoint->GetType())
			{
			case ePhysicsJointType_Hinge:
				{
					iPhysicsJointHinge *pHingeJoint = static_cast<iPhysicsJointHinge*>(pJoint);
					pHingeJoint->SetMaxAngle(cMath::ToRad(afVal));
					break;
				}
			case ePhysicsJointType_Screw:
				{
					iPhysicsJointScrew *pScrewJoint = static_cast<iPhysicsJointScrew*>(pJoint);
					pScrewJoint->SetMaxDistance(afVal);
					break;
				}
			case ePhysicsJointType_Slider:
				{
					iPhysicsJointSlider *pSliderJoint = static_cast<iPhysicsJointSlider*>(pJoint);
					pSliderJoint->SetMaxDistance(afVal);
					break;
				}
			}

		}
		else
		{
			Warning("Joint property '%s' does not exist!\n",asProp.c_str());
		}
	}


	//-----------------------------------------------------------------------

	/**
	* Adds a force to the body. This can either be in the bodies local coord system or the world's.
	* \param asBodyName The body name
	* \param asCoordType The coordinate system type. "World" or "Local".
	* \param afX force in the x direction. (in newton, kg*m/s^2)
	* \param afY force in the y direction. (in newton, kg*m/s^2)
	* \param afZ force in the z direction. (in newton, kg*m/s^2)
	**/
	static void __stdcall AddBodyForce(std::string asBodyName, std::string asCoordType,
										float afX, float afY, float afZ)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		int lType =0;
		tString sLowType = cString::ToLowerCase(asCoordType);

		if(sLowType == "world") lType = 1;
		else if(sLowType == "local") lType =2;

		if(lType==0){
			Warning("Coord system type '%s' is not valid.\n",asCoordType.c_str());
			return;
		}

		if(lType==1)
		{
			pBody->AddForce(cVector3f(afX,afY,afZ));
		}
		else if(lType==2)
		{
			cVector3f vWorldForce = cMath::MatrixMul(pBody->GetLocalMatrix().GetRotation(),
														cVector3f(afX,afY,afZ));
			pBody->AddForce(vWorldForce);
		}
	}

	//-----------------------------------------------------------------------

	/**
	* Adds an impule (a change in velocity) to the body. This can either be in the bodies local coord system or the world's.
	* \param asBodyName The body name
	* \param asCoordType The coordinate system type. "World" or "Local".
	* \param afX velocity in the x direction. (in m/s)
	* \param afY velocity in the y direction. (in m/s)
	* \param afZ velocity in the z direction. (in m/s)
	**/
	static void __stdcall AddBodyImpulse(std::string asBodyName, std::string asCoordType,
		float afX, float afY, float afZ)
	{
		iPhysicsBody *pBody = gpScene->GetWorld3D()->GetPhysicsWorld()->GetBody(asBodyName);
		if(pBody==NULL){
			Warning("Couldn't find Body '%s'\n",asBodyName.c_str());return;
		}

		int lType =0;
		tString sLowType = cString::ToLowerCase(asCoordType);

		if(sLowType == "world") lType = 1;
		else if(sLowType == "local") lType =2;

		if(lType==0){
			Warning("Coord system type '%s' is not valid.\n",asCoordType.c_str());
			return;
		}

		if(lType==1)
		{
			pBody->AddImpulse(cVector3f(afX,afY,afZ));
		}
		else if(lType==2)
		{
			cVector3f vWorldForce = cMath::MatrixMul(pBody->GetLocalMatrix().GetRotation(),
				cVector3f(afX,afY,afZ));
			pBody->AddImpulse(vWorldForce);
		}
	}

	//-----------------------------------------------------------------------


	/////////////////////////////////////////////////////////////////////////
	/////// LOCAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void __stdcall CreateLocalVar(std::string asName, int alVal)
	{
		if(gpScene->GetLocalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpScene->CreateLocalVar(asName);
			pVar->mlVal = alVal;
		}
	}

	static void __stdcall SetLocalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateLocalVar(asName);
		pVar->mlVal = alVal;
	}

	static void __stdcall AddLocalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateLocalVar(asName);
		pVar->mlVal += alVal;
	}

	static int __stdcall GetLocalVar(std::string asName)
	{
		cScriptVar* pVar = gpScene->GetLocalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find local var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}

	//-----------------------------------------------------------------------
	/////////////////////////////////////////////////////////////////////////
	/////// GLOBAL VARS //////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static void __stdcall CreateGlobalVar(std::string asName, int alVal)
	{
		if(gpScene->GetGlobalVar(asName)==NULL)
		{
			cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
			pVar->mlVal = alVal;
		}
	}

	static void __stdcall SetGlobalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
		pVar->mlVal = alVal;
	}


	static void __stdcall AddGlobalVar(std::string asName, int alVal)
	{
		cScriptVar* pVar = gpScene->CreateGlobalVar(asName);
		pVar->mlVal += alVal;
	}


	static int __stdcall GetGlobalVar(std::string asName)
	{
		cScriptVar* pVar = gpScene->GetGlobalVar(asName);
		if(pVar==NULL)
		{
			Error("Couldn't find global var '%s'\n",asName.c_str());
			return 0;
		}
		return pVar->mlVal;
	}

	//-----------------------------------------------------------------------



	void cScriptFuncs::Init(	cGraphics* apGraphics,
				cResources *apResources,
				cSystem *apSystem,
				cInput *apInput,
				cScene *apScene,
				cSound *apSound,
				cGame *apGame
			)
	{
		gpGraphics = apGraphics;
		gpResources = apResources;
		gpSystem = apSystem;
		gpInput = apInput;
		gpScene = apScene;
		gpSound = apSound;
		gpGame = apGame;

#define AddFunc(signature, ptr) gpSystem->GetLowLevel()->AddScriptFunc(signature, (void*)ptr)
		//General
		AddFunc("void Print(string &in asText)", Print);
		AddFunc("string& FloatToString(float afX)", FloatToString);
		AddFunc("string& IntToString(int alX)", IntToString);
		AddFunc("float RandFloat(float afMin, float afMax)", RandFloat);
		AddFunc("int RandInt(int alMin, int alMax)", RandInt);
		AddFunc("bool StringContains(string &in asString, string &in asSubString)", StringContains);
		AddFunc("void ResetLogicTimer()", ResetLogicTimer);

		//Renderer
		AddFunc("void SetAmbientColor(float afR, float afG, float afB)", SetAmbientColor);

		AddFunc("void SetSkybox(string &in asTexture)", SetSkybox);
		AddFunc("void SetSkyboxActive(bool abX)", SetSkyboxActive);
		AddFunc("void SetSkyboxColor(float afR, float afG, float afB, float afA)", SetSkyboxColor);

		AddFunc("void CreateParticleSystemOnCamera(string &in asName, string &in asType)",
				CreateParticleSystemOnCamera);
		AddFunc("void SetFogActive(bool abX)", SetFogActive);
		AddFunc("void SetFogCulling(bool abX)", SetFogCulling);
		AddFunc("void SetFogProperties(float afStart, float afEnd, float afR, float afG, float afB)",
				SetFogProperties);

		AddFunc("void SetSectorProperties(string &in asSector,float afAmbR, float afAmbG, float afAmbB)",
				SetSectorProperties);
		AddFunc("void SetSectorPortalActive(string &in asSector, int alPortal, bool abActive)",
				SetSectorPortalActive);

		//Resources
		AddFunc("void PreloadSound(string &in asFile)", PreloadSound);
		AddFunc("string& Translate(string &in asCat, string &in asName)", Translate);

		//Mesh Entity
		AddFunc("void SetMeshActive(string &in asName, bool abActive)", SetMeshActive);

		//Beams
		AddFunc("void CreateBeam(string &in asName, string &in asFile, "
				"string &in asStartArea, string &in asEndArea)", CreateBeam);
		AddFunc("void DestroyBeam(string &in asName)", DestroyBeam);

		//Particle systems
		AddFunc("void CreateParticleSystem(string &in asName, string &in asType, string &in asArea, "
				"float afX, float afY, float afZ)", CreateParticleSystem);
		AddFunc("void SetParticleSystemActive(string &in asName, bool abActive)",
				SetParticleSystemActive);
		AddFunc("void KillParticleSystem(string &in asName)", KillParticleSystem);

		//Light
		AddFunc("void FadeLight3D(string &in asName, "
				"float afR, float afG, float afB, float afA, float afRadius, float afTime)",
				FadeLight3D);
		AddFunc("void AttachBillboardToLight3D(string &in asBillboardName, "
				"string &in asLightName, bool abX)", AttachBillboardToLight3D);

		AddFunc("void SetLight3DVisible(string &in asName, bool abX)", SetLight3DVisible);
		AddFunc("void SetLight3DFlickerActive(string &in asName, bool abX)", SetLight3DFlickerActive);
		AddFunc("void SetLight3DFlicker(string &in asName, "
				"float afR, float afG, float afB, float afA, float afRadius, "
				"float afOnMinLength, float afOnMaxLength, "
				"string &in asOnSound, string &in asOnPS, "
				"float afOffMinLength, float afOffMaxLength, "
				"string &in asOffSound, string &in asOffPS, "
				"bool abFade, float afOnFadeLength, float afOffFadeLength)",
				SetLight3DFlicker);
		AddFunc("void SetLight3DOnlyAffectInSector(string &in asName, bool abX)", SetLight3DOnlyAffectInSector);

		//Sound
		AddFunc("void PlayMusic(string &in asName, float afVol, float afStepSize, bool abLoop)",
				PlayMusic);
		AddFunc("void StopMusic(float afStepSize)", StopMusic);
		AddFunc("void PlaySoundEntity(string &in asName, bool abPlayStart)", PlaySoundEntity);
		AddFunc("void StopSoundEntity(string &in asName, bool abPlayEnd)", StopSoundEntity);
		AddFunc("void FadeInSoundEntity(string &in asName, float afSpeed)", FadeInSoundEntity);
		AddFunc("void FadeOutSoundEntity(string &in asName, float afSpeed)", FadeOutSoundEntity);
		AddFunc("void PlayGuiSound(string &in asName, float afVol)", PlayGuiSound);
		AddFunc("void CreateSoundEntity(string &in asName, string &in asFile, string &in asArea)",
				CreateSoundEntity);

		//Physics
		AddFunc("void SetJointCallback(string &in asJointName, string &in asType, string &in asFunc)",
				SetJointCallback);
		AddFunc("void BreakJoint(string &in asJointName)", BreakJoint);
		AddFunc("float GetJointProperty(string &in asJointName, string &in asProp)",
				GetJointProperty);
		AddFunc("float GetBodyProperty(string &in asJointName, string &in asProp)",
				GetBodyProperty);
		AddFunc("void SetBodyProperty(string &in asJointName, string &in asProp, float afVal)",
				SetBodyProperty);
		AddFunc("void SetJointProperty(string &in asJointName, string &in asProp, float afVal)",
				SetJointProperty);
		AddFunc("void AttachBodiesWithJoint(string &in asParentName, string &in asChildName, "
				"string &in asJointName)", AttachBodiesWithJoint);


		AddFunc("void SetJointControllerActive(string &in asJointName, string &in asCtrlName, "
				"bool abActive)", SetJointControllerActive);
		AddFunc("void ChangeJointController(string &in asJointName, string &in asCtrlName)",
				ChangeJointController);
		AddFunc("void SetJointControllerPropertyFloat(string &in asJointName, string &in asCtrlName, "
				"string &in asProperty, float afValue)", SetJointControllerPropertyFloat);


		AddFunc("void AddBodyForce(string &in asBodyName, string &in asCoordType, "
				"float afX, float afY, float afZ)", AddBodyForce);
		AddFunc("void AddBodyImpulse(string &in asBodyName, string &in asCoordType, "
				"float afX, float afY, float afZ)", AddBodyImpulse);

		//Local vars
		AddFunc("void CreateLocalVar(string &in asName, int alVal)", CreateLocalVar);
		AddFunc("void SetLocalVar(string &in asName, int alVal)", SetLocalVar);
		AddFunc("void AddLocalVar(string &in asName, int alVal)", AddLocalVar);
		AddFunc("int GetLocalVar(string &in asName)", GetLocalVar);

		//Global vars
		AddFunc("void CreateGlobalVar(string &in asName, int alVal)", CreateGlobalVar);
		AddFunc("void SetGlobalVar(string &in asName, int alVal)", SetGlobalVar);
		AddFunc("void AddGlobalVar(string &in asName, int alVal)", AddGlobalVar);
		AddFunc("int GetGlobalVar(string &in asName)", GetGlobalVar);
#undef AddFunc
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------

}
