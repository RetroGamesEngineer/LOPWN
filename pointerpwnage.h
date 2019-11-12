#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <string>

/*  Pointer Pwnage classes by RetroGamesEngineer on github (C) 11/11/2019
	Can be used for whatever if desired, no restrictions...
*/

using string=std::string;

template<class T> class Pointer
{
private:
	

public:
	T *pointsTo,nullT;
	string name;
	std::vector<size_t> offsets;
	uintptr_t pointerBase;
	bool errorFree;

	Pointer() { init(); }
	Pointer(string Name,string Path="") { init(); name=Name; if(Path!="") setPointerPath(Path); }

	void init()
	{
		pointsTo=nullptr;
		ZeroMemory(&nullT,sizeof(T));
		name.clear();
		offsets.clear();
		pointerBase=(uintptr_t)GetModuleHandleA(0);
		resetError();
	}
	void setError() { errorFree=false; }
	void resetError() { errorFree=true; }
	void setBaseByModuleName(string moduleName="")
	{ 
		if(moduleName=="") pointerBase=(uintptr_t)GetModuleHandleA(0);
		else pointerBase=(uintptr_t)GetModuleHandleA(moduleName.c_str()); 
	}
	void setBaseByAddress(uintptr_t baseAddress) { pointerBase=baseAddress; }
	void addOffsets() {};
	template<class O,class... O2> void addOffsets(O first,O2... next)
	{
		offsets.push_back((size_t)first);
		addOffsets(next...);
	}

	T read()
	{
		calculatePointer();
		if(pointsTo!=nullptr)
		{
			__try
			{
				return *pointsTo;
			}
			__except(1)
			{
				pointsTo=nullptr; setError();
			}
		}
		return nullT;
	}

	bool write(T value)
	{
		calculatePointer();
		if(pointsTo!=nullptr)
		{
			__try
			{
				*pointsTo=value;
			}
			__except(1)
			{ 
				pointsTo=nullptr; setError(); return false;
			}
		}
		return true;
	}

	void calculatePointer()
	{
		uintptr_t pointer=pointerBase;

		__try
		{ 
			for(size_t i=0; i<offsets.size(); i++)
			{
				if(i==(offsets.size()-1)) { pointer+=offsets[i]; break; }
				pointer=*((uintptr_t*)(pointer+offsets[i]));
			}
		}
		__except(1)
		{ 
			pointsTo=nullptr; return setError();
		}

		pointsTo=(T*)pointer;
		resetError();
	}

	void setPointerPath(string pointerPath)
	{
		size_t i=0;

		const char *p=pointerPath.c_str();
		printf_s("start: %s\n",p);
		while(p[i]=='[') i++;
		if(p[i]=='0')
		{
			pointerBase=(uintptr_t)GetModuleHandleA(0);
			while(p[i]!=']' && p[i]!=0) i++;
			if(p[i]!=0) i++;
			printf_s("after [0]: %s\n",&p[i]);
		}
		else if(p[i]=='\'')
		{
			string moduleName=substrUntilCharacter(pointerPath,i+1,'\'');
			printf_s("ModuleName: %s\n",moduleName.c_str());
			setBaseByModuleName(moduleName);
			i+=(moduleName.length()+3);
			printf_s("after ['modulename.dll']: %s\n",&p[i]);
		}

		offsets.clear();
		for(;;)
		{
			if(p[i]!='+' && p[i]!='-') break;

			string offsetString=substrUntilCharacter(pointerPath,i+1,']');

			size_t currentOffset;
			if(sizeof(size_t)==4) currentOffset=strtoul(offsetString.c_str(),0,16);
			else if(sizeof(size_t)==8) currentOffset=(size_t)strtoull(offsetString.c_str(),0,16);

			if(p[i]=='+') addOffsets(currentOffset);
			else if(p[i]=='-') addOffsets(-(__int64)currentOffset);

			printf_s("offsetString: %s, currentOffset: %04x, &p[i]: %s\n",offsetString.c_str(),currentOffset, &p[i]);

			i+=(offsetString.length()+2);
			if(i>=pointerPath.length()) break;
		}
	}

	string substrUntilCharacter(string& str,size_t startOffset,char character)
	{
		return str.substr(startOffset,str.find(character,startOffset)-startOffset);
	}
};

template<class T> class Pointerz
{
private:
	static std::unique_ptr<Pointerz<T>> pointersObject;
public:
	std::vector<std::shared_ptr<Pointer<T>>> pointers;

	static Pointerz* sharedInstance()
	{
		if(!pointersObject.get()) pointersObject=std::unique_ptr<Pointerz<T> >(new Pointerz<T>());
		return pointersObject.get();
	}
	static Pointer<T>* get(string pointerName)
	{
		for(auto& i : sharedInstance()->pointers)
			if(i->name==pointerName)
				return i.get();
		return 0;
	}

	template<class... O> static Pointer<T>* createWithArgs(string pointerName,string baseFromModule,O... pointerOffsets)
	{
		if(get(pointerName)) return 0;

		auto ptr=std::make_shared<Pointer<T>>(pointerName);
		if(!ptr.get()) return 0;

		ptr->setBaseByModuleName(baseFromModule);
		ptr->addOffsets(pointerOffsets...);
		sharedInstance()->pointers.push_back(ptr);
		return ptr.get();
	}

	static Pointer<T>* createWithString(string pointerName,string pointerPath)
	{
		if(get(pointerName)) return 0;

		auto ptr=std::make_shared<Pointer<T>>(pointerName,pointerPath);
		if(!ptr.get()) return 0;

		sharedInstance()->pointers.push_back(ptr);
		return ptr.get();
	}

	static T read(string pointerName)
	{
		auto rwptr=get(pointerName);
		if(rwptr) return rwptr->read();
		return rwptr->nullT;
	}

	static bool write(string pointerName,T value)
	{
		Pointer<T>* rwptr=get(pointerName);
		if(rwptr) return rwptr->write(value);
		return false;
	}

	static void clear()
	{
		for(auto i : sharedInstance()->pointers)
		{
			i->clear();
			i.reset();
		}
		sharedInstance()->pointers.clear();
	}
};

template<class T> std::unique_ptr<Pointerz<T>> Pointerz<T>::pointersObject;