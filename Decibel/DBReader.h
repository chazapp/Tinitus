#ifndef DBREADER_H_
#define DBREADER_H

#include <mmdeviceapi.h>
#include <winerror.h>
#include <endpointvolume.h>


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
		hr = _currentDevice->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&endpoint);
		if (FAILED(hr)) {
			std::cout << "[DBREADER::GetCurrentdBLevel()]" << " Activate() failed." << std::endl;
		}
		float result;
		endpoint->GetMasterVolumeLevel(&result);
		return result;
	}
	
	bool SetCurrentDevice() {
		HRESULT hr = S_OK;
		IMMDeviceEnumerator *pEnumerator = NULL;

		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL,
								CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
		if (FAILED(hr)) {
			std::cout << "[DBREADER::SetCurrentDevice()]" << "CoCreateInstance" << std::endl;
			return false;
		}
		hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &_deviceCollection);
		if (FAILED(hr)) {
			std::cout << "[DBREADER::SetCurrentDevice()]" << "EnumAudioEndpoints bug" << std::endl;
			return false;
		}
		UINT count;
		hr = _deviceCollection->GetCount(&count);
		if (FAILED(hr)) {
			std::cout << "_deviceCollection->GetCount() bug" << std::endl;
			return false;
		}
		std::cout << "[DBREADER::SetCurrentDevice()] " << "Found " << count << " devices." << std::endl;
		LPWSTR pwszID = NULL;
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &_currentDevice);
		if (FAILED(hr)) {
			std::cout << "[DBREADER::SetCurrentDevice()] " << "GetDefaultAudioEndpoint bug" << std::endl;
			return false;
		}

		return true;
		/*
		for (ULONG i = 0; i < count; i++) {
			hr = _deviceCollection->Item(i, &_currentDevice);
			if (FAILED(hr)) {
				std::cout << GetLastError() << std::endl;
				return false;
			}
			_currentDevice->GetId(&pwszID);
			DWORD pdwState;
			_currentDevice->GetState(&pdwState);
			if (pdwState == DEVICE_STATE_ACTIVE) {
				std::cout << "[DBREADER::SetCurrentDevice()] " << "Found active device: " << pwszID << std::endl;
				return true;
			}
			std::cout << "[DBREADER::SetCurrentDevice()]" << "Device #" << i << " ID: " << pwszID << std::endl;
		}
		*/
	}

private:
	//selectedAudioOutputDevice
	IMMDevice *_currentDevice;
	IMMDeviceCollection *_deviceCollection;
	CLSID CLSID_MMDeviceEnumerator;
	IID IID_IMMDeviceEnumerator;
	IID IID_IAudioEndpointVolume;
};

#endif //DBREADER_H
