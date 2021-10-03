#include <assert.h>
#include <string.h> // strstr
#include <stdio.h>  // sprintf
#include "impl/scriptstring.h"
using namespace std;

BEGIN_AS_NAMESPACE

//--------------
// constructors
//--------------

asCScriptString::asCScriptString()
{
	// Count the first reference
	refCount = 1;
}

asCScriptString::asCScriptString(const char *s, unsigned int len)
{
	refCount = 1;
	buffer.assign(s, len);
}

asCScriptString::asCScriptString(const string &s)
{
	refCount = 1;
	buffer = s;
}

asCScriptString::asCScriptString(const asCScriptString &s)
{
	refCount = 1;
	buffer = s.buffer;
}

asCScriptString::~asCScriptString()
{
	assert( refCount == 0 );
}

//--------------------
// reference counting
//--------------------

void asCScriptString::AddRef() const
{
	refCount++;
}

static void StringAddRef_Generic(asIScriptGeneric *gen)
{
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	thisPointer->AddRef();
}

void asCScriptString::Release() const
{
	if( --refCount == 0 )
		delete this;
}

static void StringRelease_Generic(asIScriptGeneric *gen)
{
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	thisPointer->Release();
}

//-----------------
// string = string
//-----------------

asCScriptString &asCScriptString::operator=(const asCScriptString &other)
{
	// Copy only the buffer, not the reference counter
	buffer = other.buffer;

	// Return a reference to this object
	return *this;
}

static void AssignString_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	*thisPointer = *a;
	gen->SetReturnAddress(thisPointer);
}

//------------------
// string += string
//------------------

asCScriptString &asCScriptString::operator+=(const asCScriptString &other)
{
	buffer += other.buffer;
	return *this;
}

static void AddAssignString_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

//-----------------
// string opCmp string
//-----------------

static int StringCmp(const string &a, const string &b)
{
	int cmp = 0;
	if( a < b ) cmp = -1;
	else if( a > b ) cmp = 1;
	return cmp;
}

static void StringCmp_Generic(asIScriptGeneric * gen)
{
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));

  int cmp = 0;
  if( *a < *b ) cmp = -1;
  else if( *a > *b ) cmp = 1;

  *(int*)gen->GetAddressOfReturnLocation() = cmp;
}

//-----------------
// string + string
//-----------------

asCScriptString *operator+(const asCScriptString &a, const asCScriptString &b)
{
	// Return a new object as a script handle
	asCScriptString *str = new asCScriptString();

	// Avoid unnecessary memory copying by first reserving the full memory buffer, then concatenating
	str->buffer.reserve(a.buffer.length() + b.buffer.length());
	str->buffer += a.buffer;
	str->buffer += b.buffer;

	return str;
}

static void ConcatenateStrings_Generic(asIScriptGeneric *gen)
{
	asCScriptString *a = (asCScriptString*)gen->GetObject();
	asCScriptString *b = (asCScriptString*)gen->GetArgAddress(0);
	asCScriptString *out = *a + *b;
	gen->SetReturnAddress(out);
}

//----------------
// string = value
//----------------

static asCScriptString &AssignUIntToString(unsigned int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%u", i);
	dest.buffer = buf;
	return dest;
}

static void AssignUIntToString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignUIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignIntToString(int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%d", i);
	dest.buffer = buf;
	return dest;
}

static void AssignIntToString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignFloatToString(float f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer = buf;
	return dest;
}

static void AssignFloatToString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignFloatToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignDoubleToString(double f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer = buf;
	return dest;
}

static void AssignDoubleToString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignDoubleToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AssignBoolToString(bool b, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%s", b ? "true" : "false");
	dest.buffer = buf;
	return dest;
}

static void AssignBoolToString_Generic(asIScriptGeneric *gen)
{
	bool b = gen->GetArgByte(0) ? true : false;
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AssignBoolToString(b, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

//-----------------
// string += value
//-----------------

static asCScriptString &AddAssignUIntToString(unsigned int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%u", i);
	dest.buffer += buf;
	return dest;
}

static void AddAssignUIntToString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignUIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignIntToString(int i, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%d", i);
	dest.buffer += buf;
	return dest;
}

static void AddAssignIntToString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignIntToString(i, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignFloatToString(float f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer += buf;
	return dest;
}

static void AddAssignFloatToString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignFloatToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignDoubleToString(double f, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%g", f);
	dest.buffer += buf;
	return dest;
}

static void AddAssignDoubleToString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignDoubleToString(f, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

static asCScriptString &AddAssignBoolToString(bool b, asCScriptString &dest)
{
	char buf[100];
	sprintf(buf, "%s", b ? "true" : "false");
	dest.buffer += buf;
	return dest;
}

static void AddAssignBoolToString_Generic(asIScriptGeneric *gen)
{
	bool b = gen->GetArgByte(0) ? true : false;
	asCScriptString *thisPointer = (asCScriptString*)gen->GetObject();
	AddAssignBoolToString(b, *thisPointer);
	gen->SetReturnAddress(thisPointer);
}

//----------------
// string + value
//----------------

static asCScriptString *AddStringUInt(const asCScriptString &str, unsigned int i)
{
	char buf[100];
	sprintf(buf, "%u", i);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringUInt_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *out = AddStringUInt(*str, i);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringInt(const asCScriptString &str, int i)
{
	char buf[100];
	sprintf(buf, "%d", i);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringInt_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	int i = gen->GetArgDWord(0);
	asCScriptString *out = AddStringInt(*str, i);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringFloat(const asCScriptString &str, float f)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringFloat_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	float f = gen->GetArgFloat(0);
	asCScriptString *out = AddStringFloat(*str, f);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringDouble(const asCScriptString &str, double f)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(str.buffer + buf);
}

static void AddStringDouble_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	double f = gen->GetArgDouble(0);
	asCScriptString *out = AddStringDouble(*str, f);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddStringBool(const asCScriptString &str, bool b)
{
	char buf[100];
	sprintf(buf, "%s", b ? "true" : "false");
	return new asCScriptString(str.buffer + buf);
}

static void AddStringBool_Generic(asIScriptGeneric *gen)
{
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	bool b = gen->GetArgByte(0) ? true : false;
	asCScriptString *out = AddStringBool(*str, b);
	gen->SetReturnAddress(out);
}

//----------------
// value + string
//----------------

static asCScriptString *AddIntString(int i, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%d", i);
	return new asCScriptString(buf + str.buffer);
}

static void AddIntString_Generic(asIScriptGeneric *gen)
{
	int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	asCScriptString *out = AddIntString(i, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddUIntString(unsigned int i, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%u", i);
	return new asCScriptString(buf + str.buffer);
}

static void AddUIntString_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	asCScriptString *out = AddUIntString(i, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddFloatString(float f, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(buf + str.buffer);
}

static void AddFloatString_Generic(asIScriptGeneric *gen)
{
	float f = gen->GetArgFloat(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	asCScriptString *out = AddFloatString(f, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddDoubleString(double f, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%g", f);
	return new asCScriptString(buf + str.buffer);
}

static void AddDoubleString_Generic(asIScriptGeneric *gen)
{
	double f = gen->GetArgDouble(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	asCScriptString *out = AddDoubleString(f, *str);
	gen->SetReturnAddress(out);
}

static asCScriptString *AddBoolString(bool b, const asCScriptString &str)
{
	char buf[100];
	sprintf(buf, "%s", b ? "true" : "false");
	return new asCScriptString(buf + str.buffer);
}

static void AddBoolString_Generic(asIScriptGeneric *gen)
{
	bool b = gen->GetArgByte(0) ? true : false;
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	asCScriptString *out = AddBoolString(b, *str);
	gen->SetReturnAddress(out);
}

//----------
// string[]
//----------

static char *StringCharAt(unsigned int i, asCScriptString &str)
{
	if( i >= str.buffer.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		return 0;
	}

	return &str.buffer[i];
}

static void StringCharAt_Generic(asIScriptGeneric *gen)
{
	unsigned int i = gen->GetArgDWord(0);
	asCScriptString *str = (asCScriptString*)gen->GetObject();
	char *ch = StringCharAt(i, *str);
	gen->SetReturnAddress(ch);
}

//-----------------------
// AngelScript functions
//-----------------------

// This is the string factory that creates new strings for the script based on string literals
static asCScriptString *StringFactory(asUINT length, const char *s)
{
	return new asCScriptString(s, length);
}

static void StringFactory_Generic(asIScriptGeneric *gen)
{
	asUINT length = gen->GetArgDWord(0);
	const char *s = (const char*)gen->GetArgAddress(1);
	asCScriptString *str = StringFactory(length, s);
	gen->SetReturnAddress(str);
}

// This is the default string factory, that is responsible for creating empty string objects, e.g. when a variable is declared
static asCScriptString *StringDefaultFactory()
{
	// Allocate and initialize with the default constructor
	return new asCScriptString();
}

static asCScriptString *StringCopyFactory(const asCScriptString &other)
{
	// Allocate and initialize with the copy constructor
	return new asCScriptString(other);
}

static void StringDefaultFactory_Generic(asIScriptGeneric *gen)
{
	*(asCScriptString**)gen->GetAddressOfReturnLocation() = StringDefaultFactory();
}

static void StringCopyFactory_Generic(asIScriptGeneric *gen)
{
	asCScriptString *other = (asCScriptString *)gen->GetArgObject(0);
	*(asCScriptString**)gen->GetAddressOfReturnLocation() = StringCopyFactory(*other);
}

static void StringEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a == *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringEquals_Generic(asIScriptGeneric * gen)
{
	string * a = static_cast<string *>(gen->GetObject());
	string * b = static_cast<string *>(gen->GetArgAddress(0));
	*(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}

static void StringNotEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a != *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringLesserOrEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a <= *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringGreaterOrEqual_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a >= *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringLesser_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a < *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringGreater_Generic(asIScriptGeneric *gen)
{
	string *a = (string*)gen->GetArgAddress(0);
	string *b = (string*)gen->GetArgAddress(1);
	bool r = *a > *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void StringLength_Generic(asIScriptGeneric *gen)
{
	string *s = (string*)gen->GetObject();
	size_t l = s->size();
	if( sizeof(size_t) == 4 )
		gen->SetReturnDWord((asUINT)l);
	else
		gen->SetReturnQWord((asQWORD)l);
}

static void StringResize_Generic(asIScriptGeneric *gen)
{
	string *s = (string*)gen->GetObject();
	size_t v = *(size_t*)gen->GetAddressOfArg(0);
	s->resize(v);
}

static bool StringEquals(const std::string& lhs, const std::string& rhs)
{
	return lhs == rhs;
}

// This is where we register the string type
void RegisterScriptString_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("string", 0, asOBJ_REF); assert( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f()",                 asFUNCTION(StringDefaultFactory), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f(const string &in)", asFUNCTION(StringCopyFactory), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADDREF,     "void f()",                    asMETHOD(asCScriptString,AddRef), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_RELEASE,    "void f()",                    asMETHOD(asCScriptString,Release), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asMETHODPR(asCScriptString, operator =, (const asCScriptString&), asCScriptString&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asMETHODPR(asCScriptString, operator+=, (const asCScriptString&), asCScriptString&), asCALL_THISCALL); assert( r >= 0 );

	// Register the factory to return a handle to a new string
	// Note: We must register the string factory after the basic behaviours,
	// otherwise the library will not allow the use of object handles for this type
	r = engine->RegisterStringFactory("string@", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Need to use a wrapper for operator== otherwise gcc 4.7+ fails to compile
	r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTIONPR(StringEquals, (const string &, const string &), bool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmp), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(const string &in) const", asFUNCTIONPR(operator +, (const asCScriptString &, const asCScriptString &), asCScriptString*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the object methods
	if( sizeof(size_t) == 4 )
	{
		r = engine->RegisterObjectMethod("string", "uint length() const", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint)", asMETHODPR(string,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}
	else
	{
		r = engine->RegisterObjectMethod("string", "uint64 length() const", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint64)", asMETHODPR(string,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}

    // TODO: Add factory  string(const string &in str, int repeatCount)

	// TODO: Add explicit type conversion via constructor and value cast

	// TODO: Add parseInt and parseDouble. Two versions, one without parameter, one with an outparm that returns the number of characters parsed.

	// Automatic conversion from values
	r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(double) const", asFUNCTION(AddStringDouble), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(double) const", asFUNCTION(AddDoubleString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(float)", asFUNCTION(AssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(float)", asFUNCTION(AddAssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(float) const", asFUNCTION(AddStringFloat), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(float) const", asFUNCTION(AddFloatString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(int) const", asFUNCTION(AddStringInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(int) const", asFUNCTION(AddIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(uint) const", asFUNCTION(AddStringUInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(uint) const", asFUNCTION(AddUIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(bool) const", asFUNCTION(AddStringBool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(bool) const", asFUNCTION(AddBoolString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
}

void RegisterScriptString_Generic(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("string", 0, asOBJ_REF); assert( r >= 0 );

	// Register the object operator overloads
	// Note: We don't have to register the destructor, since the object uses reference counting
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f()",                 asFUNCTION(StringDefaultFactory_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_FACTORY,    "string @f(const string &in)", asFUNCTION(StringCopyFactory_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADDREF,     "void f()",                    asFUNCTION(StringAddRef_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_RELEASE,    "void f()",                    asFUNCTION(StringRelease_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(AssignString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asFUNCTION(AddAssignString_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the factory to return a handle to a new string
	// Note: We must register the string factory after the basic behaviours,
	// otherwise the library will not allow the use of object handles for this type
	r = engine->RegisterStringFactory("string@", asFUNCTION(StringFactory_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTION(StringEquals_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmp_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(const string &in) const", asFUNCTION(ConcatenateStrings_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the index operator, both as a mutator and as an inspector
	r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAt_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the object methods
	if( sizeof(size_t) == 4 )
	{
		r = engine->RegisterObjectMethod("string", "uint length() const", asFUNCTION(StringLength_Generic), asCALL_GENERIC); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint)", asFUNCTION(StringResize_Generic), asCALL_GENERIC); assert( r >= 0 );
	}
	else
	{
		r = engine->RegisterObjectMethod("string", "uint64 length() const", asFUNCTION(StringLength_Generic), asCALL_GENERIC); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint64)", asFUNCTION(StringResize_Generic), asCALL_GENERIC); assert( r >= 0 );
	}

	// Automatic conversion from values
	r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDoubleToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDoubleToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(double) const", asFUNCTION(AddStringDouble_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(double) const", asFUNCTION(AddDoubleString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(float)", asFUNCTION(AssignFloatToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(float)", asFUNCTION(AddAssignFloatToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(float) const", asFUNCTION(AddStringFloat_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(float) const", asFUNCTION(AddFloatString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(int) const", asFUNCTION(AddStringInt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(int) const", asFUNCTION(AddIntString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUIntToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(uint) const", asFUNCTION(AddStringUInt_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(uint) const", asFUNCTION(AddUIntString_Generic), asCALL_GENERIC); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBoolToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBoolToString_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd(bool) const", asFUNCTION(AddStringBool_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string@ opAdd_r(bool) const", asFUNCTION(AddBoolString_Generic), asCALL_GENERIC); assert( r >= 0 );
}

void RegisterScriptString(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		RegisterScriptString_Generic(engine);
	else
		RegisterScriptString_Native(engine);
}

END_AS_NAMESPACE


