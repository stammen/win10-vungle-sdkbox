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

#pragma once

#include <string>
#include <map>

namespace sdkbox
{
	class VungleListener;

	namespace PluginVungle
	{
		void init(void* platformData = nullptr);
		void show(const std::string & name);
		void setListener(VungleListener * listener);
		VungleListener * getListener();
		void removeListener();
		void setDebug(bool enable);
		bool isCacheAvailable();
		void setUserID(const std::string & userID);
	};
	
	ref class Vungle sealed
	{
	public:
		void Init(Windows::UI::Core::CoreDispatcher^ dispatcher);

		bool IsCacheAvailable()
		{
			return mCacheAvailable;
		};

		static Vungle^ getInstance() {
			static Vungle^ instance = ref new Vungle();
			return instance;
		}

	internal:
		void PlayAdAsync(const std::string& name);

		void SetListener(VungleListener* listener) {
			mListener = listener;
		};

		void SetDebug(bool enable) {
			mDebugMode = enable;
		};

		VungleListener * getListener()
		{
			return mListener;
		}


	private:
		Vungle();
		void LoadJson();
		void OnOnAdPlayableChanged(Platform::Object ^sender, VungleSDK::AdPlayableEventArgs^ args);
		void OnAdStart(Platform::Object ^sender, VungleSDK::AdEventArgs^ e);
		void OnAdEnd(Platform::Object ^sender, VungleSDK::AdEndEventArgs^ e);
		void OnVideoView(Platform::Object ^sender, VungleSDK::AdViewEventArgs^ e);
		void Diagnostic(Platform::Object ^sender, VungleSDK::DiagnosticLogEvent^ e);

		bool mCacheAvailable;
		bool mDebugMode;
		VungleSDK::VungleAd^ m_sdkInstance;
		VungleListener* mListener;
		std::map<std::string, VungleSDK::AdConfig^> mAds;
		Platform::String^ mVungleAppId;
		Platform::Agile<Windows::UI::Core::CoreDispatcher> mDispatcher;
	};

	class VungleListener
	{
		friend ref class Vungle;

	private:
		virtual void onVungleCacheAvailable(bool isAvailable) = 0;
		virtual void onVungleStarted() = 0;
		virtual void onVungleFinished() = 0;
		virtual void onVungleAdViewed(bool isComplete) = 0;
		// not sure how to implement the onVungleAdReward callback
		virtual void onVungleAdReward(const std::string& name) = 0;
	};
};


