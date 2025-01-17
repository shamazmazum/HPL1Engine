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
#include "system/String.h"
#include <stdlib.h>

#include "system/LowLevelSystem.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	tWString cString::To16Char(const tString &asString)
	{
		tWString wsTemp;
		size_t needed = mbstowcs(NULL,&asString[0],0);
		wsTemp.resize(needed);
		mbstowcs(&wsTemp[0],&asString[0],needed);

		return wsTemp;
	}

	//-----------------------------------------------------------------------

	tString cString::To8Char(const tWString &awsString)
	{
		tString sTemp;
		size_t needed = wcstombs(NULL,&awsString[0],0);
		sTemp.resize(needed);
		wcstombs(&sTemp[0],&awsString[0],needed);

		return sTemp;
	}

	tWString cString::UTF8ToWChar(const tString& asString)
	{
		tWString sTemp;

		const char *pCur = asString.c_str();
		const char *pEnd = pCur + asString.size();

		while (pCur < pEnd) {
			uint32_t uTemp;
			if ( (*pCur & 0x80) == 0 ) {
				uTemp = pCur[0];
				pCur+=1;
			}
			else if ( (*pCur & 0xe0) == 0xc0 ) {
				uTemp = (pCur[0] & 0x1f) << 6 |
						(pCur[1] & 0x3f);
				pCur+=2;
			}
			else if ( (*pCur & 0xf0) == 0xe0 ) {
				uTemp = (pCur[0] & 0x0f) << 12 |
						(pCur[1] & 0x3f) << 6 |
						(pCur[2] & 0x3f);
				pCur+=3;
			}
#if SIZEOF_WCHAR == 4
			else if ( (*pCur & 0xf8) == 0xf0 ) {
				uTemp = (pCur[0] & 0x07) << 18 |
						(pCur[1] & 0x3f) << 12 |
						(pCur[2] & 0x3f) << 6 |
						(pCur[3] & 0x3f);
				pCur+=4;
			}
			else if ( (*pCur & 0xfc) == 0xf8 ) {
				uTemp = (pCur[0] & 0x03) << 24 |
						(pCur[1] & 0x3f) << 18 |
						(pCur[2] & 0x3f) << 12 |
						(pCur[3] & 0x3f) << 6 |
						(pCur[4] & 0x3f);
				pCur+=5;
			}
			else if ( (*pCur & 0xfe) == 0xfc ) {
				uTemp = (pCur[0] & 0x01) << 30 |
						(pCur[1] & 0x3f) << 24 |
						(pCur[2] & 0x3f) << 18 |
						(pCur[3] & 0x3f) << 12 |
						(pCur[4] & 0x3f) << 6 |
						(pCur[5] & 0x3f);
				pCur+=6;
			}
#endif
			sTemp.push_back((wchar_t)uTemp);
		}

		return sTemp;
	}

	//-----------------------------------------------------------------------

	tWString cString::Get16BitFromArray(const tString &asArray)
	{
		// TODOD: needs to be rewritten to be more portal as wchar_t is not ALWAYS 2 bytes. (it's 4 on linux and os x)
		tIntVec vVals;
		GetIntVec(asArray,vVals,NULL);

		tWString wsString=_W(""); wsString.resize(vVals.size());

		for(size_t i=0; i< vVals.size(); ++i)
		{
			wsString[i] = (wchar_t)vVals[i];
		}

		return wsString;
	}

	//-----------------------------------------------------------------------

	tString cString::Sub(const tString& asString,int alStart,int alCount)
	{
		int lStringSize = (int)asString.size();
		if(alStart >= lStringSize) return "";
		if(alStart + alCount > lStringSize) alCount = lStringSize - alStart;

		if(alCount<0)	return asString.substr(alStart);
		else			return asString.substr(alStart, alCount);
	}
	tWString cString::SubW(const tWString& asString,int alStart,int alCount)
	{
		int lStringSize = (int)asString.size();
		if(lStringSize==0) return _W("");
		if(alStart >= lStringSize) return _W("");
		if(alStart + alCount > lStringSize) alCount = lStringSize - alStart;

		if(alCount<0)	return asString.substr(alStart);
		else			return asString.substr(alStart, alCount);
	}

	//-----------------------------------------------------------------------

	//Get the file extension of a string
	tString cString::GetFileExt(tString aString)
	{
		int pos = GetLastStringPos(aString,".");

		if(pos<0)
			return "";
		else
			return aString.substr(pos+1);
	}

	tWString cString::GetFileExtW(tWString aString)
	{
		int pos = GetLastStringPosW(aString,_W("."));

		if(pos<0)
			return _W("");
		else
			return aString.substr(pos+1);
	}


	//-----------------------------------------------------------------------

	tString cString::ToLowerCase(tString aString)
	{
		for(int i=0;i<(int)aString.size();i++)
		{
			aString[i] = tolower(aString[i]);
		}
		return aString;
	}

	tWString cString::ToLowerCaseW(tWString aString)
	{
		for(int i=0;i<(int)aString.size();i++)
		{
			aString[i] = tolower(aString[i]);
		}
		return aString;
	}

	//-----------------------------------------------------------------------


	//Set the file extension
	tString cString::SetFileExt(tString  aString,tString  aExt)
	{
		if(aExt.substr(0,1)==".")aExt = aExt.substr(1);
		if(GetFileExt(aString)!="")
		{
			aString = aString.substr(0,GetLastStringPos(aString,"."));
		}

		if(aExt!="")
			aString = aString + "." + aExt;

		return aString;
	}

	tWString cString::SetFileExtW(tWString  aString,tWString  aExt)
	{
		if(aExt.substr(0,1)==_W("."))aExt = aExt.substr(1);
		if(GetFileExtW(aString)!=_W(""))
		{
			aString = aString.substr(0,GetLastStringPosW(aString,_W(".")));
		}

		if(aExt!=_W(""))
			aString = aString + _W(".") + aExt;

		return aString;
	}

	//-----------------------------------------------------------------------

	tString cString::SetFilePath(tString aString,tString aPath)
	{
		if(GetLastChar(aPath)!="/" && GetLastChar(aPath)!="\\")
			aPath +="/";

		aString = GetFileName(aString);

		return aPath + aString;
	}

	tWString cString::SetFilePathW(tWString aString,tWString aPath)
	{
		if(GetLastCharW(aPath)!=_W("/") && GetLastCharW(aPath)!=_W("\\"))
			aPath +=_W("/");

		aString = GetFileNameW(aString);

		return aPath + aString;
	}


	//-----------------------------------------------------------------------


	//Gets the filename in a path
	tString cString::GetFileName(tString  aString)
	{
		int pos1 = GetLastStringPos(aString,"\\");
		int pos2 = GetLastStringPos(aString,"/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return aString;
		else
			return aString.substr(pos+1);
	}
	tWString cString::GetFileNameW(tWString  aString)
	{
		int pos1 = GetLastStringPosW(aString,_W("\\"));
		int pos2 = GetLastStringPosW(aString,_W("/"));
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return aString;
		else
			return aString.substr(pos+1);
	}

	//-----------------------------------------------------------------------

	tString cString::GetFilePath(tString aString)
	{
		if(GetLastStringPos(aString,".")<0)return aString;

		int pos1 = GetLastStringPos(aString,"\\");
		int pos2 = GetLastStringPos(aString,"/");
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return "";
		else
			return aString.substr(0, pos+1);

	}

	tWString cString::GetFilePathW(tWString aString)
	{
		if(GetLastStringPosW(aString,_W("."))<0) return aString;

		int pos1 = GetLastStringPosW(aString,_W("\\"));
		int pos2 = GetLastStringPosW(aString,_W("/"));
		int pos = pos1>pos2 ? pos1 : pos2;

		if(pos<0)
			return _W("");
		else
			return aString.substr(0, pos+1);

	}

	//-----------------------------------------------------------------------

	tString cString::ReplaceCharTo(tString aString, tString asOldChar,tString asNewChar)
	{
		if(asNewChar !="")
		{
			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] == asOldChar[0]) aString[i] = asNewChar[0];
			}
			return aString;
		}
		else
		{
			tString sNewString;
			sNewString.reserve(aString.size());

			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] != asOldChar[0]) sNewString.push_back(aString[i]);
			}
			return sNewString;
		}
	}

	tWString cString::ReplaceCharToW(tWString aString, tWString asOldChar,tWString asNewChar)
	{
		if(asNewChar !=_W(""))
		{
			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] == asOldChar[0]) aString[i] = asNewChar[0];
			}
			return aString;
		}
		else
		{
			tWString sNewString;
			sNewString.reserve(aString.size());

			for(int i=0;i<(int)aString.size();i++)
			{
				if(aString[i] != asOldChar[0]) sNewString.push_back(aString[i]);
			}
			return sNewString;
		}
	}

	//-----------------------------------------------------------------------

	tString cString::ReplaceStringTo(tString asString, tString asOldString,tString asNewString)
	{
		tString sNewString = "";

		for(size_t i=0;i<asString.size();i++)
		{
			bool bFound = true;
			//Search for old string
			if(asString.size() >= i + asOldString.size())
			{
				for(size_t j=0; j<asOldString.size(); ++j)
				{
					if(asString[i+j] != asOldString[j]) {
						bFound = false;
						break;
					}
				}
			}
			else
			{
				bFound = false;
			}

			//Insert new string
			if(bFound)
			{
				sNewString += asNewString;
				i += asOldString.size()-1;
			}
			//Just add the character
			else {
				sNewString += asString[i];
			}
		}
		return sNewString;
	}

	//-----------------------------------------------------------------------

	//gets the last char in the string
	tString cString::GetLastChar(tString aString)
	{
		if(aString.size()==0) return "";
		return aString.substr(aString.size()-1);
	}

	tWString cString::GetLastCharW(tWString aString)
	{
		if(aString.size()==0) return _W("");
		return aString.substr(aString.size()-1);
	}

	//-----------------------------------------------------------------------
	tString cString::ToString(const char* asString,tString asDefault)
	{
		if(asString==NULL)return asDefault;

		return asString;
	}

	//-----------------------------------------------------------------------

	int cString::ToInt(const char* asString,int alDefault)
	{
		if(asString==NULL)return alDefault;

		return atoi(asString);
	}

	//-----------------------------------------------------------------------

	float cString::ToFloat(const char* asString,float afDefault)
	{
		if(asString==NULL)return afDefault;

		return (float)atof(asString);
	}

	//-----------------------------------------------------------------------

	bool cString::ToBool(const char* asString, bool abDefault)
	{
		if(asString==NULL)return abDefault;

		tString asTempString = ToLowerCase(asString);
		return asTempString == "true"?true:false;
	}

	//-----------------------------------------------------------------------

	cColor cString::ToColor(const char* asString, const cColor& aDefault)
	{
		if(asString==NULL)return aDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 4) return aDefault;

		return cColor(vValues[0],vValues[1],vValues[2],vValues[3]);
	}

	//-----------------------------------------------------------------------

	cVector2f cString::ToVector2f(const char* asString, const cVector2f& avDefault)
	{
		if(asString==NULL) return avDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 2) return avDefault;

		return cVector2f(vValues[0],vValues[1]);
	}

	//-----------------------------------------------------------------------

	cVector3f cString::ToVector3f(const char* asString, const cVector3f& avDefault)
	{
		if(asString==NULL) return avDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 3) return avDefault;

		return cVector3f(vValues[0],vValues[1],vValues[2]);
	}

	//-----------------------------------------------------------------------

	cVector2l cString::ToVector2l(const char* asString, const cVector2l& avDefault)
	{
		if(asString==NULL) return avDefault;

		tIntVec vValues;

		GetIntVec(asString,vValues,NULL);

		if(vValues.size() != 2) return avDefault;

		return cVector2l(vValues[0],vValues[1]);
	}

	//-----------------------------------------------------------------------

	cVector3l cString::ToVector3l(const char* asString, const cVector3l& avDefault)
	{
		if(asString==NULL) return avDefault;

		tIntVec vValues;

		GetIntVec(asString,vValues,NULL);

		if(vValues.size() != 3) return avDefault;

		return cVector3l(vValues[0],vValues[1],vValues[2]);
	}

	//-----------------------------------------------------------------------

	cMatrixf cString::ToMatrixf(const char* asString, const cMatrixf& a_mtxDefault)
	{
		if(asString==NULL) return a_mtxDefault;

		tFloatVec vValues;

		GetFloatVec(asString,vValues,NULL);

		if(vValues.size() != 16) return a_mtxDefault;

		return cMatrixf(vValues[0],vValues[1],vValues[2],vValues[3],
						vValues[4],vValues[5],vValues[6],vValues[7],
						vValues[8],vValues[9],vValues[10],vValues[11],
						vValues[12],vValues[13],vValues[14],vValues[15]);
	}

	//-----------------------------------------------------------------------

	tIntVec& cString::GetIntVec(const tString &asData, tIntVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr, apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToInt(mvStr[i].c_str(),0) );
		}

		return avVec;
	}

	//-----------------------------------------------------------------------

	tUIntVec& cString::GetUIntVec(const tString &asData, tUIntVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr, apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToInt(mvStr[i].c_str(),0) );
		}

		return avVec;
	}


	//-----------------------------------------------------------------------

	tFloatVec& cString::GetFloatVec(const tString &asData, tFloatVec& avVec,tString *apSeparators)
	{
		tStringVec mvStr;
		GetStringVec(asData, mvStr,apSeparators);

		for(int i=0;i<(int)mvStr.size();i++)
		{
			avVec.push_back(ToFloat(mvStr[i].c_str(),0) );
		}

		return avVec;
	}

	//-----------------------------------------------------------------------

	tString cString::ToString(int alX)
	{
		char buff[256];

		sprintf(buff, "%d",alX);

		return buff;
	}

	tString cString::ToString(float afX)
	{
		char buff[256];

		sprintf(buff, "%f",afX);

		return buff;
	}

	//-----------------------------------------------------------------------

	tWString cString::ToStringW(int alX)
	{
		wchar_t buff[256];

		swprintf(buff, 256, _W("%d"),alX);

		return buff;
	}

	tWString cString::ToStringW(float afX)
	{
		wchar_t buff[256];

		swprintf(buff, 256, _W("%f"),afX);

		return buff;
	}

	//-----------------------------------------------------------------------

	tStringVec& cString::GetStringVec(const tString &asData, tStringVec& avVec,tString *apSeparators)
	{
		tString str = "";
		bool start = false;
		tString c = "";

		for(int i=0;i<(int)asData.length();i++)
		{
			c = asData.substr(i,1);
			bool bNewWord = false;

			//Check if the current char is a separator
			if(apSeparators)
			{
				for(size_t j=0; j< apSeparators->size(); j++)
				{
					if((*apSeparators)[j] == c[0])
					{
						bNewWord = true;
						break;
					}
				}
			}
			else
			{
				if(c[0]==' ' || c[0]=='\n' || c[0]=='\t' || c[0]==',')
				{
					bNewWord = true;
				}
			}

			if(bNewWord)
			{
				if(start)
				{
					start = false;
					avVec.push_back(str);
					str = "";
				}
			}
			else
			{
				start = true;
				str +=c;
				if(i==asData.length()-1)avVec.push_back(str);
			}
		}

		return avVec;
	}

	//-----------------------------------------------------------------------

	///Helper
	//returns last char in a string
	int cString::GetLastStringPos(tString aString, tString  aChar)
	{
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
				pos=i;
		}
		return pos;
	}

	///Helper
	//returns last char in a string
	int cString::GetLastStringPosW(tWString aString, tWString  aChar)
	{
		int pos=-1;
		for(int i=0;i<(int)aString.size();i++)
		{
			if(aString.substr(i,aChar.size())==aChar)
				pos=i;
		}
		return pos;
	}

	//-----------------------------------------------------------------------

	void cString::UIntStringToArray(unsigned int *apArray, const char* apString,int alSize)
	{
		char vTempChar[20];
		int lTempCharCount=0;

		int lArrayCount=0;
		int lStringCount =0;

		while(lArrayCount < alSize)
		{
			char c= apString[lStringCount];

			//if a space is found, convert the previous characters to a float.
			if(c == ' ' || c == 0)
			{
				if(lTempCharCount>0)
				{
					vTempChar[lTempCharCount] =0;
					apArray[lArrayCount] = (unsigned int) atoi(vTempChar);

					lTempCharCount=0;
					lArrayCount++;
				}
			}
			//If character is not a space, add to temp char.
			else
			{
				vTempChar[lTempCharCount] = c;
				lTempCharCount++;
			}

			lStringCount++;
		}
	}

	//-----------------------------------------------------------------------

	void cString::FloatStringToArray(float *apArray, const char* apString,int alSize)
	{
		char vTempChar[20];
		int lTempCharCount=0;

		int lArrayCount=0;
		int lStringCount =0;

		while(lArrayCount < alSize)
		{
			char c= apString[lStringCount];
			//if a space is found, convert the previous characters to a float.
			if(c == ' ' || c == 0)
			{
				if(lTempCharCount>0)
				{
					vTempChar[lTempCharCount] =0;
					apArray[lArrayCount] = (float)atof(vTempChar);

					lTempCharCount=0;
					lArrayCount++;
				}
			}
			//If character is not a space, add to temp char.
			else
			{
				vTempChar[lTempCharCount] = apString[lStringCount];
				lTempCharCount++;
			}

			lStringCount++;
		}
	}

	//-----------------------------------------------------------------------

}
