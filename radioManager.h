#ifndef RADIO_MANAGER
#define RADIO_MANAGER

#include "commonTypes.h"

class RadioManager
{
	public:
	RadioManager();
	~RadioManager();

	enum class conversations
	{
		noResponse,
		campelDefault,
		campelCrouchInfo,
	};

	enum class RadioActors
	{
		snake,
		campel,
		meryl,
	};

	struct voiceLine
	{
		RadioActors actor;
		DymArr<char*>* lines;
	};

	struct conversationStruct
	{
		DymArr<voiceLine>* voiceLines;
	};

	const conversationStruct* GetConversation(conversations p_specificConversations) const;

	private:


	conversationStruct allConversation[3];


};


#endif // !RADIO_MANAGER
