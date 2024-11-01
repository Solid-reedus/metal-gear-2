#include "precomp.h"
#include "radioManager.h"

RadioManager::RadioManager()
{

	voiceLine v1 = { RadioActors::snake, new DymArr<char*>{ new char*[3],3} };
	v1.lines->data[0] = "this is solid";
	v1.lines->data[1] = "snake your";
	v1.lines->data[2] = "reply please...";

	conversationStruct conversation1 = { new DymArr<voiceLine>{ new voiceLine[1], 1 } };
	conversation1.voiceLines->data[0] = v1;
	allConversation[0] = conversation1;

	voiceLine v2 = { RadioActors::campel, new DymArr<char*>{ new char*[4],4} };
	v2.lines->data[0] = "orders are to";
	v2.lines->data[1] = "avoid contact";
	v2.lines->data[2] = "not to get rid";
	v2.lines->data[3] = "of the enemy";

	voiceLine v3 = { RadioActors::campel, new DymArr<char*>{ new char* [4],4} };
	v3.lines->data[0] = "forget those";
	v3.lines->data[1] = "action movies ";
	v3.lines->data[2] = "you saw";
	v3.lines->data[3] = "...over";

	conversationStruct conversation2 = { new DymArr<voiceLine>{ new voiceLine[3], 3 } };
	conversation2.voiceLines->data[0] = v1;
	conversation2.voiceLines->data[1] = v2;
	conversation2.voiceLines->data[2] = v3;

	allConversation[1] = conversation2;

	voiceLine v4 = { RadioActors::campel, new DymArr<char*>{ new char* [4],4} };
	v4.lines->data[0] = "crawl and sneak";
	v4.lines->data[1] = "trough the gap";
	v4.lines->data[2] = "in the fence";
	v4.lines->data[3] = "...over";

	conversationStruct conversation3 = { new DymArr<voiceLine>{ new voiceLine[1], 1 } };
	conversation3.voiceLines->data[0] = v4;

	allConversation[2] = conversation3;

}

RadioManager::~RadioManager()
{
	//for (size_t i = 0; i < length; i++)
	//{
	//
	//}

}

const RadioManager::conversationStruct* RadioManager::GetConversation(conversations p_specificConversations) const
{
	switch (p_specificConversations)
	{
		case RadioManager::conversations::noResponse:
		{
			return &allConversation[0];
		}
		case RadioManager::conversations::campelDefault:
		{
			return &allConversation[1];
		}
		case RadioManager::conversations::campelCrouchInfo:
		{
			return &allConversation[2];
		}
		break;
		default:
		return nullptr;
	}


}
