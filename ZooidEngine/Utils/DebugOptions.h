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
	};

	extern DebugOptions gDebugOptions;
};
#endif
