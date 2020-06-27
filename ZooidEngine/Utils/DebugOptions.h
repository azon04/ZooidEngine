#ifndef __ZE_DEBUG_OPTIONS_H__
#define __ZE_DEBUG_OPTIONS_H__

namespace ZE
{
	struct DebugDraw
	{
		bool bDrawCullShapes = false;
		bool bDrawPhysicsShapes = false;
	};

	struct DebugOptions
	{
		DebugDraw DebugDrawOptions;
		bool bShowFPSStats = false;
	};

	extern DebugOptions gDebugOptions;
};
#endif
