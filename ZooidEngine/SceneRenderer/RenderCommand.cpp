#include "RenderCommand.h"

#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	void RenderCommandList::registerCommand(NativeRenderCommandDelegate::RenderCommandFunc _renderCommand, void* _data)
	{
		m_commands.push_back(NativeRenderCommandDelegate(_renderCommand, _data));
	}

	void RenderCommandList::executeCommands()
	{
		for (Int32 i = 0; i < m_commands.length(); i++)
		{
			m_commands[i].call();
		}
	}

	void RenderCommandList::clearCommands()
	{
		m_commands.clear();
	}

}