/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) Microsoft Corporation

http://www.cocos2d-x.org

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
****************************************************************************/

#include "pch.h"
#include "Vungle-winrt.h"
#include <codecvt>

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;
using namespace Windows::ApplicationModel::Core;

//Using VungleSDK namespace
using namespace VungleSDK;
using namespace sdkbox;
using namespace cocos2d;

namespace sdkbox
{
	namespace PluginVungle
	{
		void init(void* platformData)
		{
			Windows::UI::Core::CoreDispatcher^ dispatcher = reinterpret_cast<Windows::UI::Core::CoreDispatcher^>(platformData);
			Vungle^ v = Vungle::getInstance();
			v->Init(dispatcher);
		}

		void show(const std::string & name)
		{
			Vungle^ v = Vungle::getInstance();
			v->PlayAdAsync(name);
		}

		void setListener(VungleListener * listener)
		{
			Vungle^ v = Vungle::getInstance();
			v->SetListener(listener);
		}

		VungleListener * getListener()
		{
			VungleListener* listener = nullptr;
			return listener;
		}

		void removeListener()
		{
			Vungle^ v = Vungle::getInstance();
			v->SetListener(nullptr);
		}

		void setDebug(bool enable)
		{
			Vungle^ v = Vungle::getInstance();
			v->SetDebug(enable);
		}

		bool isCacheAvailable()
		{
			Vungle^ v = Vungle::getInstance();
			return v->IsCacheAvailable();
		}

		void setUserID(const std::string & userID)
		{

		}
	};
};

Platform::String^ stringToPlatformString(const std::string& inputString) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring intermediateForm = converter.from_bytes(inputString);
	Platform::String^ retVal = ref new Platform::String(intermediateForm.c_str());
	return retVal;
}

Vungle::Vungle()
	: mListener(nullptr)
	, mCacheAvailable(false)
	, mDebugMode(false)
	, mVungleAppId(L"vungleTest")
{

}

// throws Platform::Exception if error
void Vungle::LoadJson()
{
	std::string s = FileUtils::getInstance()->getStringFromFile(FileUtils::getInstance()->fullPathForFilename("sdkbox_config.json"));
	Platform::String^ jsonData = stringToPlatformString(s);
	JsonValue^ root = JsonValue::Parse(jsonData);
	JsonObject^ winrtObject = root->GetObject()->GetNamedObject("WinRT");
	JsonObject^ vungleObject = winrtObject->GetObject()->GetNamedObject("Vungle");
	mVungleAppId = vungleObject->GetNamedString("id");
	JsonObject^ ads = vungleObject->GetNamedObject("ads");

	if (ads->HasKey("video"))
	{
		JsonObject^ ad = ads->GetNamedObject("video");
		AdConfig^ config = ref new AdConfig;
		if (ad->HasKey("sound"))
		{
			config->SoundEnabled = ad->GetNamedBoolean("sound");
		}
		mAds["video"] = config;
	}

	if (ads->HasKey("reward"))
	{
		JsonObject^ ad = ads->GetNamedObject("reward");
		AdConfig^ config = ref new AdConfig;
		if (ad->HasKey("sound"))
		{
			config->SoundEnabled = ad->GetNamedBoolean("sound");
		}
		if (ad->HasKey("incentivized"))
		{
			config->Incentivized = ad->GetNamedBoolean("incentivized");
		}
		mAds["reward"] = config;
	}
}

void Vungle::Init(Windows::UI::Core::CoreDispatcher^ dispatcher)
{
	mDispatcher = dispatcher;
	try
	{
		LoadJson();

		//Obtain Vungle SDK instance
		m_sdkInstance = AdFactory::GetInstance(mVungleAppId);

		//Register Vungle event handlers
		m_sdkInstance->OnAdPlayableChanged += ref new Windows::Foundation::EventHandler<VungleSDK::AdPlayableEventArgs ^>(this, &Vungle::OnOnAdPlayableChanged);
		m_sdkInstance->OnAdStart += ref new Windows::Foundation::EventHandler<VungleSDK::AdEventArgs ^>(this, &Vungle::OnAdStart);
		m_sdkInstance->OnAdEnd += ref new Windows::Foundation::EventHandler<VungleSDK::AdEndEventArgs ^>(this, &Vungle::OnAdEnd);
		m_sdkInstance->OnVideoView += ref new Windows::Foundation::EventHandler<VungleSDK::AdViewEventArgs ^>(this, &Vungle::OnVideoView);
		m_sdkInstance->Diagnostic += ref new Windows::Foundation::EventHandler<VungleSDK::DiagnosticLogEvent ^>(this, &Vungle::Diagnostic);
	}
	catch (Platform::Exception^ ex)
	{
		if (mDebugMode)
		{
			OutputDebugStringW(ex->Message->Data());
		}
	}
}

void Vungle::PlayAdAsync(const std::string& name)
{
	auto iter = mAds.find(name);
	if (iter == mAds.end())
	{
		if (mDebugMode)
		{
			CCLOG("PlayAdAsync: Ad named %s does not exist in vungle.json", name.c_str());
		}
		return;
	}

    auto dispatcher = mDispatcher.Get();
	if (dispatcher)
	{
		AdConfig^ ad = iter->second;
		dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, ad]() {
			//Play ad with default configuration
			m_sdkInstance->PlayAdAsync(ad);
		}));
	}
}

//Event handler for OnAdPlayableChanged event
void Vungle::OnOnAdPlayableChanged(Platform::Object ^sender, VungleSDK::AdPlayableEventArgs^ args)
{
	mCacheAvailable = args->AdPlayable == true;

	if (mListener)
	{
		mListener->onVungleCacheAvailable(mCacheAvailable);
	}
}

void Vungle::OnAdStart(Platform::Object ^sender, AdEventArgs^ e)
{
	if (mListener)
	{
		mListener->onVungleStarted();
	}
}

void Vungle::OnAdEnd(Platform::Object ^sender, AdEndEventArgs^ e)
{
	if (mListener)
	{
		mListener->onVungleFinished();
	}
}

void Vungle::OnVideoView(Platform::Object ^sender, AdViewEventArgs^ e)
{
	if (mListener)
	{
		mListener->onVungleAdViewed((bool)e->IsCompletedView);
	}
}

void Vungle::Diagnostic(Platform::Object ^sender, DiagnosticLogEvent^ e)
{
	if (mDebugMode)
	{
		OutputDebugStringW(e->Message->Data());
		OutputDebugStringW(L"\n");
	}
}

