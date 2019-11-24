#ifndef __ZE_RENDER_QUERY_H__
#define __ZE_RENDER_QUERY_H__

#include "Renderer/IRenderer.h"

namespace ZE
{
	class RenderQuery
	{
	public:

		RenderQuery() : m_bIsBegin(false), m_queryId(0) {}

		void BeginQuery(IRenderer* renderer, ERenderQueryType queryType)
		{
			m_type = queryType;
			m_renderer = renderer;

			m_queryId = m_renderer->BeginQuery(m_type);
			m_bIsBegin = true;
		}

		void EndQuery()
		{
			if (m_bIsBegin && m_queryId > 0)
			{
				m_renderer->EndQuery(m_type, m_queryId);
				m_bIsBegin = false;
			}
		}

		bool IsResultAvailable()
		{
			return m_queryId > 0 && m_renderer->CheckQueryAvailable(m_queryId);
		}

		bool GetBoolResult()
		{
			ZCHECK(m_queryId > 0);

			UInt32 result = 0;
			m_renderer->RetreiveQueryResult(m_type, m_queryId, &result);
			return result;
		}

		UInt32 GetUintResult()
		{
			ZCHECK(m_queryId > 0);

			UInt32 result;
			m_renderer->RetreiveQueryResult(m_type, m_queryId, &result);
			return result;
		}

	protected:

		bool m_bIsBegin;
		UInt32 m_queryId;
		ERenderQueryType m_type;
		IRenderer* m_renderer;
	};
}

#endif
