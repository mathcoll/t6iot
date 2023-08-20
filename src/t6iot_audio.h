/*
  Created by Mathieu Lory <mathieu@internetcollaboratif.info>.
  - t6 website: https://www.internetcollaboratif.info
  - t6 iot: https://api.internetcollaboratif.info
  - Api doc: https://api.internetcollaboratif.info/docs/
*/

#ifndef t6iotAudio_h
	#define t6iotAudio_h

	class t6iot_Audio {
		public:
			t6iot_Audio();
			void audio_loop();
			bool audioListenTo(const char* url);
			bool audioSetVol(int volume);
			int audioGetVol();

		private:
			int _volume;
	};

	extern t6iot_Audio t6iotAudio;
#endif
