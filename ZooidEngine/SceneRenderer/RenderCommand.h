#ifndef __ZE_RENDER_COMMAND_H__
#define __ZE_RENDER_COMMAND_H__

#include "Utils/Array.h"

namespace ZE
{
	class NativeRenderCommandDelegate
	{
	public:
		typedef void(*RenderCommandFunc)(void*);

		NativeRenderCommandDelegate()
		{}

		NativeRenderCommandDelegate(const RenderCommandFunc& command, void* _data)
			: m_renderCommand(command), m_data(_data)
		{}

		// Call binded function
		void call()
		{
			if (m_renderCommand)
			{
				(m_renderCommand)(m_data);
			}
		}

	protected:
		RenderCommandFunc m_renderCommand = NULL;
		void* m_data = NULL;
	};

	class RenderCommandList
	{

	public:
		RenderCommandList() {}

		void registerCommand(NativeRenderCommandDelegate::RenderCommandFunc _renderCommand, void* _data);
		void executeCommands();
		void clearCommands();

	protected:
		Array<NativeRenderCommandDelegate> m_commands;
	};
}
#endif
