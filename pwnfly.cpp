#include "pointerpwnage.h"
#include <math.h>

#define PI 3.1415927f

HMODULE flyModule=0;
float flySpeed=33.33f;
bool flyEnabled=false;

class rotation
{
public:
	float pitch,yaw;
	rotation(float Pitch=0,float Yaw=0) : pitch(Pitch),yaw(Yaw) { }
};

class vec3
{
public:
	float x,y,z;
	vec3(float X=0,float Y=0,float Z=0) : x(X),y(Y),z(Z) { }
	vec3 operator+=(vec3 addthis)
	{
		x+=addthis.x;
		y+=addthis.y;
		z+=addthis.z;
		return *this;
	}
};

float DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}

void MoveForwardInCameraDirection(rotation rot,vec3 src,float flyspeed)
{
	vec3 dest=src;

	/*
	GuidedHacking's code for his answer to 'Need help with reverse engineering camera-related information in a video game', turns out not correct for Pwn Adventure
	https://reverseengineering.stackexchange.com/questions/18144/need-help-with-reverse-engineering-camera-related-information-in-a-video-game
	dest.x+=cosf(DegreesToRadians(rot.pitch-90.0f)) * flyspeed;
	dest.y+=sinf(DegreesToRadians(rot.yaw-90.0f)) * flyspeed;
	dest.z+=sinf(DegreesToRadians(rot.pitch)) * flyspeed;
	*/

	/*
	DMGregory's answer on the referenced initial question 'What am I missing for this flycam to work correctly?'
	which the original poster didn't find useful but turned out to be very useful... (At least for Pwn Adventure)
	"Let's assume your Y axis points forward (Yaw = 0), your X axis points right (Yaw = -90 degrees), and your Z axis pints up (a right-handed coordinate system).

	Then you can construct a unit vector in spherical coordinates that points in the direction your camera is looking:

	x = -sin(yaw) * cos(pitch)
	y =  cos(yaw) * cos(pitch)
	z =  sin(pitch)

	Multiply this vector by your desired speed (and delta time step) to get the displacement to move your camera forward.

	Run the same formulas with yaw - 90 to get the camera's right vector."
	https://gamedev.stackexchange.com/questions/158050/what-am-i-missing-for-this-flycam-to-work-correctly
	*/

	dest.x+=(-sinf(DegreesToRadians(rot.yaw-90.0f)) * cosf(DegreesToRadians(rot.pitch))) * flyspeed;
	dest.y+=(cosf(DegreesToRadians(rot.yaw-90.0f)) * cosf(DegreesToRadians(rot.pitch))) * flyspeed;
	dest.z+=(sinf(DegreesToRadians(rot.pitch))) * flyspeed;

	printf_s("New Position: %.02f : %.02f : %.02f\n",dest.x,dest.y,dest.z);

	Pointerz<vec3>::write("PlayerPosition",dest);
}

void FlyThread()
{
	AllocConsole();
	FILE* f=new FILE;
	freopen_s(&f,"CONOUT$","w",stdout);

	printf_s("PWNFLY Started!\n");
	
	/* Pointer class can be used directly like so */
	//auto pos=std::make_unique<Pointer<vec3>>("PlayerPosition");
	//pos->setBaseByModuleName("GameLogic.dll");
	//pos->addOffsets(0x97d7c,0x1c,0x04,0x114,0x98);

	/* Pointerz creator and manager class is the main way it can used */
	auto pos=Pointerz<vec3>::createWithString("PlayerPosition","[[[[[['GameLogic.dll']+97d7c]+1c]+4]+114]+90]");

	/*LiveOverflow's pointer which doesn't work for me: at start of pointer path "[0]" means current exe as module / GetModuleHandle(NULL); */
	//auto pos=Pointerz<vec3>::createWithString("PlayerPosition","[[[[[[0]+18ffde4]+0]+464]+1d4]+3e4]+98]");

	auto rot=Pointerz<rotation>::createWithString("PitchYaw","[[[[[['GameLogic.dll']+97d80]+58]+1cc]+294]+2a0]");

	/* Can also be created with passed arugments instead of as a pointer string if preferred
	passing an empty string "" as module name is current exe as module / GetModuleHandle(NULL); */

	//Pointerz<vec3>::createWithArgs("PlayerPosition","GameLogic.dll",0x97d7c,0x1c,0x4,0x114,0x90);
	//Pointerz<rotation>::createWithArgs("PitchYaw","GameLogic.dll",0x97d80,0x58,0x1cc,0x294,0x2a0);

	/*And read / write like this: */
	/*
	vec3 v=Pointerz<vec3>::read("PlayerPosition");
	rotation r=Pointerz<rotation>::read("PitchYaw");

	Pointerz<vec3>::write("PlayerPosition",v);
	Pointerz<rotation>::write("PitchYaw",r);

	Pointer<vec3>* p2p_vec3=Pointerz<vec3>::get("PlayerPosition");
	auto p2p_rot=Pointerz<vec3>::get("PitchYaw");

	vec3 v2=p2p_vec3->read();
	p2p_vec3->write(v2);

	auto r2=p2p_rot->read();
	p2p_rot->write(r2);
	*/
	/* etc, etc, etc...*/

	for(;; Sleep(1)) //Sleep in loop containing GetAsyncKeyState's so they don't hog cpu resources uneccesarily
	{
		//Correction for testing GetAsyncKeyState: & 1 is not the best choice is it doesn't equal true if the key is being held down, only when it's initially pressed... 
		//See docs:
		/*
		Return Value
		Type: SHORT
		If the function succeeds, the return value specifies whether the key was pressed since the last call to GetAsyncKeyState,
		and whether the key is currently up or down. If the most significant bit is set, the key is down, and if the least significant bit is set,
		the key was pressed after the previous call to GetAsyncKeyState. However, you should not rely on this last behavior; for more information, see the Remarks.
		*/
		//F1 == Toggle Fly on/off, Middle Mouse Button == Fly forward, F12 == Disable Fly and unload dll
		if(GetAsyncKeyState(VK_F1) & 0x8000)
		{
			flyEnabled^=1;
			printf_s("flyEnabled: %02x\n",flyEnabled);

			if(flyEnabled)
			{
				printf_s("Player Position pointer base: %08x Offsets",pos->pointerBase);
				for(auto i : pos->offsets)
					printf_s(", %04x",i);

				printf_s("\nPitch & Yaw pointer base: %08x Offsets",rot->pointerBase);
				for(auto i : rot->offsets)
					printf_s(", %04x",i);

				//Explicitly calling 'calculatePointer' is not required as it does so automatically on every read/write
				//It's merely for printing them out to show they're accurate upon enabling
				pos->calculatePointer();
				printf_s("\nPlayer Position Pointer=%08x\n",(uintptr_t)pos->pointsTo);
				rot->calculatePointer();
				printf_s("Pitch & Yaw Pointer=%08x\n",(uintptr_t)rot->pointsTo);
			}
			Sleep(337); //Sleep after toggling something so merely pressing key doesn't instantly re-toggle it a bunch of times
		}
		else if(flyEnabled && (GetAsyncKeyState(VK_MBUTTON) & 0x8000))
		{
			rotation currentAngle=rot->read();
			vec3 currentPosition=pos->read();
			printf_s("Pitch: %.02f, Yaw: %.02f, Current Position: %.02f : %.02f : %.02f\n",currentAngle.pitch,currentAngle.yaw,
				currentPosition.x,currentPosition.y,currentPosition.z);

			MoveForwardInCameraDirection(currentAngle,currentPosition,flySpeed);
		}
		else if(GetAsyncKeyState(VK_F12) & 0x8000)
			break;
	}

	//Pressing F12, breaking out of the loop, and unloading the dll is useful for quicker development not having to restart the game for every modification and test.
	printf_s("Freeing and unloading PWNFLY.dll...\n");
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(flyModule,0);
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		flyModule=hModule;
		DisableThreadLibraryCalls(flyModule);
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)&FlyThread,0,0,0);
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

