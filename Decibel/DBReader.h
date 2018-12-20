#ifndef DBREADER_H_
#define DBREADER_H

#include <mmdeviceapi.h>
#include <winerror.h>
#include <endpointvolume.h>
#include <string>
#include <atlstr.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <propvarutil.h>

class DBReader {
public:	//DBReader class selects the default output audio device and tries to obtain its current volume in decibels (dBF).
	DBReader() {
		CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
		IID_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume);
		CoInitialize(NULL);
	}


	~DBReader() {

	}

	float GetCurrentdBLevel() {
		IAudioEndpointVolume *endpoint;
		HRESULT hr;
		if (_currentDevice != NULL) {
			hr = _currentDevice->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&endpoint);
			if (FAILED(hr)) {
				std::cerr << "[DBREADER::GetCurrentdBLevel()]" << " Activate() failed." << std::endl;
				return -1;
			}
			float result;
			hr = endpoint->GetMasterVolumeLevel(&result);
			if (FAILED(hr)) {
				std::cerr << "[DBREADER::GetCurrentdBLevel()]" << "GetMasterVolumeFailed" << std::endl;
				return -1;
			}
			//Results = 0 always
			return result;
		}
		return -1;
	}
	


	bool SetCurrentDevice() {
		HRESULT hr = S_OK;
		IMMDeviceEnumerator *pEnumerator = NULL;

		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,
								CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
		if (FAILED(hr)) {
			std::cerr << "[DBREADER::SetCurrentDevice()]" << "CoCreateInstance" << std::endl;
			return false;
		}
		hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &_deviceCollection);
		if (FAILED(hr)) {
			std::cerr << "[DBREADER::SetCurrentDevice()]" << "EnumAudioEndpoints bug" << std::endl;
			return false;
		}
		UINT count;
		hr = _deviceCollection->GetCount(&count);
		if (FAILED(hr)) {
			std::cerr << "_deviceCollection->GetCount() bug" << std::endl;
			return false;
		}
		std::cout << "[DBREADER::SetCurrentDevice()] " << "Found " << count << " devices." << std::endl;
		LPWSTR pwszID = NULL;
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &_currentDevice);
		if (FAILED(hr)) {
			std::cerr << "[DBREADER::SetCurrentDevice()] " << "GetDefaultAudioEndpoint failed" << std::endl;
			return false;
		}
		IPropertyStore *store;
		_currentDevice->OpenPropertyStore(STGM_READ, &store);
		PROPVARIANT varName;
		PropVariantInit(&varName);
		hr = store->GetValue(
			PKEY_Device_FriendlyName, &varName);
		if (FAILED(hr)) {
			std::cerr << "[DBREADER::SetCurrentDevice()] " << "GetStoreValue failed." << std::endl;
			return false;
		}
		WCHAR wc[260];
		PropVariantToString(varName, wc, ARRAYSIZE(wc));
		char ch[260];
		char DefChar = ' ';
		WideCharToMultiByte(CP_ACP, 0, wc, -1, ch, 260, &DefChar, NULL);
		_currentDeviceName = std::string(ch);
		std::cout << "[DBREADER::SetCurrentDevice()] " << " Selected Device: " << _currentDeviceName  << std::endl;
		return true;
	}

private:
	//selectedAudioOutputDevice
	IMMDevice *_currentDevice;
	IMMDeviceCollection *_deviceCollection;
	CLSID CLSID_MMDeviceEnumerator;
	IID IID_IMMDeviceEnumerator;
	IID IID_IAudioEndpointVolume;
	std::string _currentDeviceName;
};

#endif //DBREADER_H
