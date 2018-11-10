#include <nan.h>
#include <dwmapi.h>
#include <VersionHelpers.h>

struct ACCENTPOLICY
{
	int nAccentState;
	int nFlags;
	int nColor;
	int nAnimationId;
};

struct WINCOMATTRPDATA {
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
};

enum WINCOMPATTR {
	WCA_ACCENT_POLICY = 19
};

enum ACCENTTYPES {
	ACCENT_DISABLE = 0,
	ACCENT_ENABLE_GRADIENT = 1,
	ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
	ACCENT_ENABLE_BLURBEHIND = 3, // aero glass
	ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, // fluent design acrylic
	ACCENT_INVALID_STATE = 5
};

void Acrylic(const Nan::FunctionCallbackInfo<v8::Value>& info) {

    if (info.Length() < 1) {
        Nan::ThrowTypeError("Wrong number of arguments, expected 1");
        return;
    }

    if (!info[0]->IsObject()) {
        Nan::ThrowTypeError("Bad arguments");
        return;
    }


    v8::Local<v8::Object> handleBuffer =
        info[0].As<v8::Object>();

    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope scope(isolate);

    char* bufferData = node::Buffer::Data(handleBuffer);

    uint32_t handle = *reinterpret_cast<uint32_t*>((unsigned char*) bufferData);

    HWND hwnd = (HWND)handle;
    bool result;

	if (IsWindowsVersionOrGreater(10, 0, 0)) {
		const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
		if (hModule) {
			typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMATTRPDATA*);
			const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
			if (SetWindowCompositionAttribute) {
				ACCENTPOLICY policy;
				policy.nAccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
				// https://stackoverflow.com/a/41433592
                policy.nFlags = 2;
				// (bgOpacity /* 0 - 255 */ << 24) | (bgColor /* BGR */ & 0xFFFFFF)
				policy.nColor = (0x40 << 24) | (0x000000 & 0xFFFFFF);

				WINCOMATTRPDATA data;
				data.nAttribute = WCA_ACCENT_POLICY;
				data.ulDataSize = sizeof(policy);
				data.pData = &policy;
				
				result = SetWindowCompositionAttribute(hwnd, &data);
			}
			FreeLibrary(hModule);
		}
	}

	info.GetReturnValue().Set(true);
}


void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("SetAcrylic").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Acrylic)->GetFunction());
}

NODE_MODULE(Vibrancy, Init)
